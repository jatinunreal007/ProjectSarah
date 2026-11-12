// Real-time GPU ray tracer (OpenGL 4.3 compute shader).
// Build with the accompanying CMakeLists.txt. Needs GLFW + GLAD.
//
// Controls:
//   Tab          capture / release mouse
//   W A S D      move,  Space / Left-Ctrl = up / down,  Shift = faster
//   Mouse wheel  zoom (changes FOV)
//   R / F        focus distance + / -
//   1 2 3 4      samples per pixel per frame = 1 / 2 / 4 / 8
//   F5           hot-reload raytrace.comp
//   P            save render_gpu.ppm
//   Esc          quit

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cmath>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#include "GpuScene.h"

#ifndef SHADER_PATH
#define SHADER_PATH "raytrace.comp"
#endif

static const int WIDTH  = 1280;
static const int HEIGHT = 536;          // 1280 / 2.39, same cinematic ratio as your CPU camera

// --------------------------------------------------------------------------- camera
struct CameraState
{
	vec3   pos = vec3(-3.0, 5.8, 24.0);
	double yaw = 0.0, pitch = 0.0;      // filled in from look-at in main()
	double fov = 12.0;
	double focus = 20.8;
	double defocusAngle = 0.6;
};

struct CamUniforms { vec3 origin, pixel00, du, dv, defU, defV; };

static vec3 Forward(const CameraState& c)
{
	return vec3(std::cos(c.pitch) * std::sin(c.yaw), std::sin(c.pitch), -std::cos(c.pitch) * std::cos(c.yaw));
}

// Same maths as your Camera::InitializeViewport()
static CamUniforms ComputeCamera(const CameraState& c)
{
	const vec3 vup(0, 1, 0);
	const vec3 lookAt = c.pos + Forward(c);

	vec3 w = vec3::Vec3Normalize(c.pos - lookAt);
	vec3 u = vec3::Vec3Normalize(vec3::Vec3Cross(vup, w));
	vec3 v = vec3::Vec3Cross(w, u);

	double theta = DegreesToRadians(c.fov);
	double vh = 2.0 * std::tan(theta / 2.0) * c.focus;
	double vw = vh * (double(WIDTH) / double(HEIGHT));

	vec3 vpU = u * vw;
	vec3 vpV = v * vh * (-1.0);

	CamUniforms out;
	out.origin = c.pos;
	out.du = vpU / double(WIDTH);
	out.dv = vpV / double(HEIGHT);
	vec3 upperLeft = c.pos - w * c.focus - vpU / 2.0 - vpV / 2.0;
	out.pixel00 = upperLeft + out.du / 2.0 + out.dv / 2.0;

	double defR = c.focus * std::tan(DegreesToRadians(c.defocusAngle) / 2.0);
	out.defU = u * defR;
	out.defV = v * defR;
	return out;
}

// --------------------------------------------------------------------------- app state / input
static CameraState g_cam;
static bool   g_dirty = true;           // camera changed -> restart accumulation
static bool   g_captured = false;
static bool   g_firstMouse = true;
static double g_lastX = 0, g_lastY = 0;
static int    g_spp = 1;
static int    g_maxDepth = 12;
static bool   g_reload = false, g_screenshot = false;

static void CursorCb(GLFWwindow*, double x, double y)
{
	if (!g_captured) return;
	if (g_firstMouse) { g_lastX = x; g_lastY = y; g_firstMouse = false; }
	double dx = x - g_lastX, dy = y - g_lastY;
	g_lastX = x; g_lastY = y;

	double sens = 0.0025 * (g_cam.fov / 45.0);       // slower look when zoomed in
	g_cam.yaw   += dx * sens;
	g_cam.pitch -= dy * sens;
	g_cam.pitch = std::fmax(-1.55, std::fmin(1.55, g_cam.pitch));
	g_dirty = true;
}

static void ScrollCb(GLFWwindow*, double, double yoff)
{
	g_cam.fov *= (1.0 - 0.1 * yoff);
	g_cam.fov = std::fmax(2.0, std::fmin(120.0, g_cam.fov));
	g_dirty = true;
}

static void KeyCb(GLFWwindow* win, int key, int, int action, int)
{
	if (action != GLFW_PRESS) return;
	switch (key)
	{
	case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(win, GLFW_TRUE); break;
	case GLFW_KEY_TAB:
		g_captured = !g_captured;
		g_firstMouse = true;
		glfwSetInputMode(win, GLFW_CURSOR, g_captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
		break;
	case GLFW_KEY_1: g_spp = 1; g_dirty = true; break;
	case GLFW_KEY_2: g_spp = 2; g_dirty = true; break;
	case GLFW_KEY_3: g_spp = 4; g_dirty = true; break;
	case GLFW_KEY_4: g_spp = 8; g_dirty = true; break;
	case GLFW_KEY_F5: g_reload = true; break;
	case GLFW_KEY_P:  g_screenshot = true; break;
	}
}

// --------------------------------------------------------------------------- GL helpers
static GLuint BuildProgram(const char* path)
{
	std::ifstream in(path);
	if (!in) { std::fprintf(stderr, "Cannot open shader: %s\n", path); return 0; }
	std::stringstream ss; ss << in.rdbuf();
	std::string src = ss.str();
	const char* c = src.c_str();

	GLuint sh = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(sh, 1, &c, nullptr);
	glCompileShader(sh);
	GLint ok = 0;
	glGetShaderiv(sh, GL_COMPILE_STATUS, &ok);
	if (!ok)
	{
		char log[8192]; glGetShaderInfoLog(sh, sizeof(log), nullptr, log);
		std::fprintf(stderr, "Shader compile error:\n%s\n", log);
		glDeleteShader(sh);
		return 0;
	}
	GLuint p = glCreateProgram();
	glAttachShader(p, sh);
	glLinkProgram(p);
	glDeleteShader(sh);
	glGetProgramiv(p, GL_LINK_STATUS, &ok);
	if (!ok)
	{
		char log[8192]; glGetProgramInfoLog(p, sizeof(log), nullptr, log);
		std::fprintf(stderr, "Shader link error:\n%s\n", log);
		glDeleteProgram(p);
		return 0;
	}
	return p;
}

template <typename T>
static GLuint UploadSSBO(const std::vector<T>& data, GLuint binding)
{
	GLuint buf;
	glGenBuffers(1, &buf);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buf);
	glBufferData(GL_SHADER_STORAGE_BUFFER, (GLsizeiptr)(data.size() * sizeof(T)), data.data(), GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, buf);
	return buf;
}

static GLuint MakeTexture(GLenum internalFormat)
{
	GLuint t;
	glGenTextures(1, &t);
	glBindTexture(GL_TEXTURE_2D, t);
	glTexStorage2D(GL_TEXTURE_2D, 1, internalFormat, WIDTH, HEIGHT);
	return t;
}

static void SaveScreenshot(GLuint tex)
{
	std::vector<unsigned char> px((size_t)WIDTH * HEIGHT * 4);
	glBindTexture(GL_TEXTURE_2D, tex);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, px.data());
	std::ofstream f("render_gpu.ppm");
	f << "P3\n" << WIDTH << " " << HEIGHT << "\n255\n";
	for (int y = HEIGHT - 1; y >= 0; --y)
		for (int x = 0; x < WIDTH; ++x)
		{
			size_t i = ((size_t)y * WIDTH + x) * 4;
			f << int(px[i]) << " " << int(px[i + 1]) << " " << int(px[i + 2]) << "\n";
		}
	std::cout << "Saved render_gpu.ppm\n";
}

static void SetVec3(GLint loc, const vec3& v) { glUniform3f(loc, (float)v.x, (float)v.y, (float)v.z); }

// --------------------------------------------------------------------------- demo scene (mirrors your renderer.cpp)
static GpuScene BuildDemoScene()
{
	GpuScene s;
	int m1 = s.AddMetal(vec3(0.95, 0.93, 0.88), 0.05);
	int m2 = s.AddLambertian(vec3(0.88, 0.69, 0.72));
	int m3 = s.AddMetal(vec3(1.0, 0.78, 0.34), 0.0);
	int glass = s.AddDielectric(1.5);
	int ground = s.AddChecker(0.5, vec3(0.1, 0.1, 0.1), vec3(0.9, 0.9, 0.9));

	auto still = [&](vec3 c, double r, int m) { s.AddSphere(c, c, r, m); };
	still(vec3(-1.5, 0.0, 2.0), 1.0, m1);
	still(vec3(4.0, 1.0, -2.0), 2.0, glass);
	still(vec3(-2.0, -0.5, 4.0), 0.5, m3);
	still(vec3(0.5, 0.5, 0.0), 1.5, m2);
	s.SetPlane(vec3(0.0, -1.0, -2.0), vec3(0.0, 1.0, 0.0), ground);

	// NOTE: your CPU scene puts the metal/glass confetti at y = -0.8 + 30 (way above the camera view).
	// I rest them on the ground instead (y = -0.9, radius 0.1, plane at y = -1).
	for (int i = 0; i < 5000; i++)
	{
		double pick = RandomDouble();
		double x = RandomDouble(-1.0, 1.0) * 30.0;
		double z = RandomDouble(-1.0, 1.0) * 30.0;
		if (pick < 0.5)
		{
			int m = s.AddLambertian(RandomVec3() * RandomVec3());
			vec3 c(x, -0.9, z);
			s.AddSphere(c, c + vec3(0, RandomDouble(0, 0.5), 0), 0.1, m);   // bouncing -> motion blur
		}
		else if (pick < 0.85)
		{
			int m = s.AddMetal(RandomVec3(0.5, 1.0), 0.1);
			still(vec3(x, -0.9, z), 0.1, m);
		}
		else
		{
			still(vec3(x, -0.9, z), 0.1, glass);
		}
	}

	s.BuildBvh();
	return s;
}

// --------------------------------------------------------------------------- main
int main()
{
	if (!glfwInit()) { std::cerr << "glfwInit failed\n"; return 1; }
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	GLFWwindow* win = glfwCreateWindow(WIDTH, HEIGHT, "GPU Ray Tracer", nullptr, nullptr);
	if (!win) { std::cerr << "Could not create an OpenGL 4.3 window\n"; return 1; }
	glfwMakeContextCurrent(win);
	glfwSwapInterval(0);                       // vsync off so the FPS number is honest
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { std::cerr << "GLAD failed\n"; return 1; }
	std::cout << "GPU: " << glGetString(GL_RENDERER) << "\n";

	glfwSetKeyCallback(win, KeyCb);
	glfwSetCursorPosCallback(win, CursorCb);
	glfwSetScrollCallback(win, ScrollCb);

	// initial orientation = look from (-3,5.8,24) at (0.5,0.5,0), like your renderer.cpp
	{
		vec3 f = vec3::Vec3Normalize(vec3(0.5, 0.5, 0.0) - g_cam.pos);
		g_cam.pitch = std::asin(f.y);
		g_cam.yaw = std::atan2(f.x, -f.z);
	}

	// --- scene -> GPU
	std::cout << "Building scene + BVH...\n";
	GpuScene scene = BuildDemoScene();
	std::cout << "Spheres: " << scene.spheres.size() << "  BVH nodes: " << scene.nodes.size() << "\n";
	UploadSSBO(scene.spheres, 2);
	UploadSSBO(scene.materials, 3);
	UploadSSBO(scene.nodes, 4);

	// --- render targets
	GLuint accumTex = MakeTexture(GL_RGBA32F);
	GLuint outTex = MakeTexture(GL_RGBA8);
	glBindImageTexture(0, accumTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	glBindImageTexture(1, outTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, outTex, 0);

	GLuint prog = BuildProgram(SHADER_PATH);
	if (!prog) return 1;

	int samples = 0;
	uint32_t seed = 0;
	double lastTime = glfwGetTime(), fpsTimer = lastTime;
	int fpsFrames = 0;

	while (!glfwWindowShouldClose(win))
	{
		glfwPollEvents();
		double now = glfwGetTime();
		double dt = now - lastTime;
		lastTime = now;

		// --- continuous input
		double speed = (glfwGetKey(win, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ? 12.0 : 3.0) * dt;
		vec3 fwd = Forward(g_cam);
		vec3 right = vec3::Vec3Normalize(vec3::Vec3Cross(fwd, vec3(0, 1, 0)));
		vec3 move(0, 0, 0);
		if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS) move += fwd;
		if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS) move += fwd * -1.0;
		if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS) move += right;
		if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS) move += right * -1.0;
		if (glfwGetKey(win, GLFW_KEY_SPACE) == GLFW_PRESS) move += vec3(0, 1, 0);
		if (glfwGetKey(win, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) move += vec3(0, -1, 0);
		if (move.x != 0 || move.y != 0 || move.z != 0) { g_cam.pos += move * speed; g_dirty = true; }
		if (glfwGetKey(win, GLFW_KEY_R) == GLFW_PRESS) { g_cam.focus += 5.0 * dt; g_dirty = true; }
		if (glfwGetKey(win, GLFW_KEY_F) == GLFW_PRESS) { g_cam.focus = std::fmax(0.5, g_cam.focus - 5.0 * dt); g_dirty = true; }

		if (g_reload)
		{
			g_reload = false;
			GLuint p = BuildProgram(SHADER_PATH);
			if (p) { glDeleteProgram(prog); prog = p; g_dirty = true; std::cout << "Shader reloaded\n"; }
		}
		if (g_dirty) { samples = 0; g_dirty = false; }

		// --- dispatch
		CamUniforms cu = ComputeCamera(g_cam);
		glUseProgram(prog);
		auto U = [&](const char* n) { return glGetUniformLocation(prog, n); };
		glUniform2i(U("uRes"), WIDTH, HEIGHT);
		glUniform1i(U("uSamples"), samples);
		glUniform1i(U("uSpp"), g_spp);
		glUniform1i(U("uMaxDepth"), g_maxDepth);
		glUniform1ui(U("uSeed"), seed++);
		SetVec3(U("uOrigin"), cu.origin);
		SetVec3(U("uPixel00"), cu.pixel00);
		SetVec3(U("uDu"), cu.du);
		SetVec3(U("uDv"), cu.dv);
		SetVec3(U("uDefU"), cu.defU);
		SetVec3(U("uDefV"), cu.defV);
		glUniform1i(U("uDefocus"), g_cam.defocusAngle > 0.0 ? 1 : 0);
		glUniform1i(U("uHasPlane"), scene.hasPlane);
		glUniform3f(U("uPlanePoint"), scene.planePoint[0], scene.planePoint[1], scene.planePoint[2]);
		glUniform3f(U("uPlaneNormal"), scene.planeNormal[0], scene.planeNormal[1], scene.planeNormal[2]);
		glUniform1i(U("uPlaneMat"), scene.planeMat);

		glDispatchCompute((WIDTH + 7) / 8, (HEIGHT + 7) / 8, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT | GL_FRAMEBUFFER_BARRIER_BIT);
		samples += g_spp;

		// --- present
		int fbw, fbh;
		glfwGetFramebufferSize(win, &fbw, &fbh);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBlitFramebuffer(0, 0, WIDTH, HEIGHT, 0, 0, fbw, fbh, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glfwSwapBuffers(win);

		if (g_screenshot) { g_screenshot = false; SaveScreenshot(outTex); }

		// --- FPS in the title bar
		fpsFrames++;
		if (now - fpsTimer >= 0.5)
		{
			double fps = fpsFrames / (now - fpsTimer);
			char title[160];
			std::snprintf(title, sizeof(title), "GPU Ray Tracer | %.1f FPS (%.2f ms) | %d spp/frame | %d spp accumulated",
				fps, 1000.0 / fps, g_spp, samples);
			glfwSetWindowTitle(win, title);
			fpsTimer = now;
			fpsFrames = 0;
		}
	}

	glfwDestroyWindow(win);
	glfwTerminate();
	return 0;
}
