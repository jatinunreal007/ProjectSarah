# ProjectSarah

A physically-based ray tracer built from scratch in C++, following along with and extending *Ray Tracing in One Weekend* by Peter Shirley.

<img width="3840" height="2021" alt="ProjectSarah render" src="https://github.com/user-attachments/assets/aad89c50-2d0b-4cf4-9ade-bf08a350e46a" />

*Render as of 23-08-2026*

## Features

- **Primitives** — spheres (including moving spheres, for motion blur) and infinite planes
- **Materials**
  - Lambertian diffuse reflection
  - Metal reflection with adjustable fuzz/roughness
  - Dielectric (glass) refraction, using Snell's law and Schlick's approximation for reflectance
- **Camera**
  - Configurable field of view and image resolution
  - Positionable via look-from / look-at / view-up vectors
  - Defocus blur (depth of field) via a lens-radius model
  - Motion blur via time-sampled rays
- **Acceleration** — a Bounding Volume Hierarchy (BVH) built over axis-aligned bounding boxes (AABB), for faster ray-scene intersection on larger scenes
- **Antialiasing** — multiple samples per pixel with random sub-pixel jitter
- **Lighting** — a basic directional light model (still in progress)
- **Output** — renders directly to a `.ppm` image, and reports total render time on completion

## Build

This is a Windows / Visual Studio project (MSVC, C++20).

1. Clone the repo
2. Open `ProjectSarah.slnx` in Visual Studio
3. Build and run — the rendered image is written to `render.ppm` in the working directory

## Project structure

| File | Description |
|---|---|
| `renderer.cpp` | Entry point — builds the scene and starts the render |
| `Camera.h` | Camera model, viewport setup, ray generation, main render loop |
| `Hittables.h` | `Hittable` interface, `Sphere`, `Plane` |
| `HittablesList.h` | Container for all objects in a scene |
| `Materials.h` | `lambertian`, `metal`, `Dielectric` materials |
| `Bvh.h` | BVH acceleration structure |
| `Aabb.h` | Axis-aligned bounding box |
| `Lightings.h` | Directional light |
| `Ray.h` | Ray class |
| `Vectors.h` | `vec3` math |
| `Algebra.h` | Supporting math (e.g. intervals) |
| `Color.h` | Pixel color accumulation and shading |
| `Utilities.h` | Random number helpers and constants |

## Reference

Built while following *[Ray Tracing in One Weekend](https://raytracing.github.io/)* by Peter Shirley, with extra features layered on top — planes, directional lighting, BVH, and motion blur.
