#pragma once
// CPU-side scene description that is uploaded to the GPU as flat arrays.
// Everything here is plain-old-data (no virtuals, no shared_ptr) so it can be
// memcpy'd straight into OpenGL shader-storage buffers (std430 layout).

#include <vector>
#include <algorithm>
#include <cstdint>
#include "Utilities.h"   // your vec3 + RandomDouble helpers

// ---- GPU structs: every member is a multiple of 16 bytes so C++ and std430 agree ----
struct GpuMaterial { float a[4]; float b[4]; int32_t info[4]; };   // 48 bytes
struct GpuSphere   { float c0r[4]; float c1[4]; int32_t info[4]; }; // 48 bytes
struct GpuBvhNode  { float bmin[4]; float bmax[4]; int32_t info[4]; }; // 48 bytes
static_assert(sizeof(GpuMaterial) == 48, "layout mismatch");
static_assert(sizeof(GpuSphere)   == 48, "layout mismatch");
static_assert(sizeof(GpuBvhNode)  == 48, "layout mismatch");

enum MatType : int32_t { MAT_LAMBERT = 0, MAT_METAL = 1, MAT_DIELECTRIC = 2, MAT_CHECKER = 3 };

class GpuScene
{
public:
	// Final, GPU-ready data (valid after BuildBvh())
	std::vector<GpuMaterial> materials;
	std::vector<GpuSphere>   spheres;   // re-ordered so BVH leaves reference contiguous ranges
	std::vector<GpuBvhNode>  nodes;

	// One infinite plane, tested outside the BVH
	float planePoint[3]  = { 0.f, -1.f, 0.f };
	float planeNormal[3] = { 0.f,  1.f, 0.f };
	int   planeMat = 0;
	int   hasPlane = 0;

	// ---------- materials ----------
	int AddLambertian(const vec3& albedo)
	{
		GpuMaterial m{};
		m.a[0] = (float)albedo.x; m.a[1] = (float)albedo.y; m.a[2] = (float)albedo.z;
		m.info[0] = MAT_LAMBERT;
		materials.push_back(m);
		return (int)materials.size() - 1;
	}
	int AddMetal(const vec3& albedo, double fuzz)
	{
		GpuMaterial m{};
		m.a[0] = (float)albedo.x; m.a[1] = (float)albedo.y; m.a[2] = (float)albedo.z;
		m.a[3] = (float)(fuzz < 1.0 ? fuzz : 1.0);
		m.info[0] = MAT_METAL;
		materials.push_back(m);
		return (int)materials.size() - 1;
	}
	int AddDielectric(double ior)
	{
		GpuMaterial m{};
		m.a[3] = (float)ior;
		m.info[0] = MAT_DIELECTRIC;
		materials.push_back(m);
		return (int)materials.size() - 1;
	}
	int AddChecker(double scale, const vec3& c1, const vec3& c2)
	{
		GpuMaterial m{};
		m.a[0] = (float)c1.x; m.a[1] = (float)c1.y; m.a[2] = (float)c1.z;
		m.b[0] = (float)c2.x; m.b[1] = (float)c2.y; m.b[2] = (float)c2.z;
		m.b[3] = (float)(1.0 / scale);   // inverse scale, like your CheckerTexture
		m.info[0] = MAT_CHECKER;
		materials.push_back(m);
		return (int)materials.size() - 1;
	}

	// ---------- geometry ----------
	// centre0 == centre1 -> static sphere. Otherwise it moves between them over ray time [0,1] (motion blur).
	void AddSphere(const vec3& centre0, const vec3& centre1, double radius, int mat)
	{
		GpuSphere s{};
		s.c0r[0] = (float)centre0.x; s.c0r[1] = (float)centre0.y; s.c0r[2] = (float)centre0.z; s.c0r[3] = (float)radius;
		s.c1[0]  = (float)centre1.x; s.c1[1]  = (float)centre1.y; s.c1[2]  = (float)centre1.z;
		s.info[0] = mat;
		raw.push_back(s);
	}

	void SetPlane(const vec3& point, const vec3& normal, int mat)
	{
		vec3 n = vec3::Vec3Normalize(normal);
		planePoint[0] = (float)point.x;  planePoint[1] = (float)point.y;  planePoint[2] = (float)point.z;
		planeNormal[0] = (float)n.x;     planeNormal[1] = (float)n.y;     planeNormal[2] = (float)n.z;
		planeMat = mat;
		hasPlane = 1;
	}

	// ---------- BVH ----------
	// Flattened depth-first layout:
	//   interior node i : left child = i + 1, right child = info[0], info[1] == 0
	//   leaf node i     : first sphere = info[0], sphere count = info[1] (> 0)
	void BuildBvh()
	{
		nodes.clear();
		spheres.clear();
		if (raw.empty()) return;

		std::vector<Prim> prims(raw.size());
		for (size_t i = 0; i < raw.size(); i++)
		{
			const GpuSphere& s = raw[i];
			const float r = s.c0r[3];
			Prim p;
			p.s = s;
			for (int k = 0; k < 3; k++)
			{
				// union of the sphere's box at t=0 and t=1 (same idea as your Sphere ctor)
				p.mn[k] = std::min(s.c0r[k], s.c1[k]) - r;
				p.mx[k] = std::max(s.c0r[k], s.c1[k]) + r;
				p.ctr[k] = 0.5f * (p.mn[k] + p.mx[k]);
			}
			prims[i] = p;
		}
		nodes.reserve(raw.size());
		spheres.reserve(raw.size());
		BuildRec(prims, 0, (int)prims.size());
	}

private:
	struct Prim { GpuSphere s; float mn[3], mx[3], ctr[3]; };
	std::vector<GpuSphere> raw;
	static constexpr int kLeafSize = 4;

	int BuildRec(std::vector<Prim>& p, int start, int end)
	{
		const int idx = (int)nodes.size();
		nodes.emplace_back();

		float mn[3]  = {  1e30f,  1e30f,  1e30f };
		float mx[3]  = { -1e30f, -1e30f, -1e30f };
		float cmn[3] = {  1e30f,  1e30f,  1e30f };
		float cmx[3] = { -1e30f, -1e30f, -1e30f };
		for (int i = start; i < end; i++)
			for (int k = 0; k < 3; k++)
			{
				mn[k]  = std::min(mn[k],  p[i].mn[k]);
				mx[k]  = std::max(mx[k],  p[i].mx[k]);
				cmn[k] = std::min(cmn[k], p[i].ctr[k]);
				cmx[k] = std::max(cmx[k], p[i].ctr[k]);
			}

		GpuBvhNode n{};
		for (int k = 0; k < 3; k++) { n.bmin[k] = mn[k]; n.bmax[k] = mx[k]; }

		const int count = end - start;
		if (count <= kLeafSize)
		{
			n.info[0] = (int)spheres.size();
			n.info[1] = count;
			for (int i = start; i < end; i++) spheres.push_back(p[i].s);
			nodes[idx] = n;
			return idx;
		}

		// split on the longest centroid axis at the median
		int axis = 0;
		float ext0 = cmx[0] - cmn[0], ext1 = cmx[1] - cmn[1], ext2 = cmx[2] - cmn[2];
		if (ext1 > ext0 && ext1 >= ext2) axis = 1;
		else if (ext2 > ext0 && ext2 > ext1) axis = 2;

		const int mid = start + count / 2;
		std::nth_element(p.begin() + start, p.begin() + mid, p.begin() + end,
			[axis](const Prim& a, const Prim& b) { return a.ctr[axis] < b.ctr[axis]; });

		nodes[idx] = n;                       // write bounds first: recursion may reallocate 'nodes'
		BuildRec(p, start, mid);              // left child lands at idx + 1
		const int right = BuildRec(p, mid, end);
		nodes[idx].info[0] = right;
		nodes[idx].info[1] = 0;
		return idx;
	}
};
