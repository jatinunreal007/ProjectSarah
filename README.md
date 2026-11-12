# ProjectSarah

A physically-based ray tracer built from scratch in C++, following along with and extending *Ray Tracing in One Weekend* by Peter Shirley. It has two renderers that share the same scene ideas:

- **CPU renderer** (`renderer.cpp`): high-quality offline renders written to a `.ppm` image.
- **GPU renderer** (`GpuRenderer.cpp` + `raytrace.comp`): a real-time OpenGL compute-shader path tracer with a live FPS counter and a fly-around camera.

<img width="4096" height="1713" alt="render" src="https://github.com/user-attachments/assets/f08cb5a5-e84b-44da-94eb-c1042ee14c94" />

*Render as of 24-08-2026*

## Features

- **Primitives**: spheres (including moving spheres, for motion blur) and infinite planes
- **Materials**
  - Lambertian diffuse reflection
  - Metal reflection with adjustable fuzz/roughness
  - Dielectric (glass) refraction, using Snell's law and Schlick's approximation for reflectance
- **Textures**: solid colour and 3D checker pattern
- **Camera**
  - Configurable field of view and image resolution
  - Positionable via look-from / look-at / view-up vectors
  - Defocus blur (depth of field) via a lens-radius model
  - Motion blur via time-sampled rays
- **Acceleration**: a Bounding Volume Hierarchy (BVH) built over axis-aligned bounding boxes (AABB), for faster ray-scene intersection on larger scenes. The GPU renderer uses a flattened version of the same idea.
- **Antialiasing**: multiple samples per pixel with random sub-pixel jitter
- **Lighting**: a basic directional light model (still in progress on the CPU side, not yet ported to the GPU)
- **Output**
  - CPU: renders directly to a `.ppm` image and reports total render time
  - GPU: renders to a window in real time, shows FPS and accumulated samples in the title bar, and can save a screenshot to `render_gpu.ppm`

## Build

This is a Windows / Visual Studio project (MSVC, C++20, x64).

### CPU renderer

1. Clone the repo
2. Open `ProjectSarah.slnx` in Visual Studio
3. Make sure `renderer.cpp` is included in the build and `GpuRenderer.cpp` is excluded (both define `main()`)
4. Build and run. The rendered image is written to `render.ppm` in the working directory

### GPU renderer

Requires a GPU and driver with **OpenGL 4.3** support. Libraries needed: **GLFW** and **GLAD**, installed through vcpkg:

```powershell
git clone https://github.com/microsoft/vcpkg C:\vcpkg
C:\vcpkg\bootstrap-vcpkg.bat
C:\vcpkg\vcpkg integrate install
C:\vcpkg\vcpkg install glfw3:x64-windows glad:x64-windows
```

1. Open the project in Visual Studio and select the **x64** configuration
2. Set C/C++ → Language → C++ Language Standard to **ISO C++20**
3. Exclude `renderer.cpp` from the build and include `GpuRenderer.cpp` (both define `main()`)
4. Run with Ctrl+F5 from Visual Studio so `raytrace.comp` is found in the working directory. To run the `.exe` on its own, copy `raytrace.comp` next to it or define `SHADER_PATH` as the full path

### GPU controls

| Key | Action |
|---|---|
| Tab | Capture / release the mouse |
| W A S D | Move |
| Space / Left Ctrl | Move up / down |
| Shift | Move faster |
| Mouse wheel | Zoom (changes field of view) |
| R / F | Increase / decrease focus distance |
| 1 2 3 4 | Samples per pixel per frame: 1 / 2 / 4 / 8 |
| F5 | Hot-reload `raytrace.comp` without restarting |
| P | Save `render_gpu.ppm` |
| Esc | Quit |

The GPU renderer takes one sample per pixel per frame and averages frames while the camera is still. The image looks noisy while moving and converges to a clean render within a second or two of stopping.

## Project structure

### Shared / CPU renderer

| File | Description |
|---|---|
| `renderer.cpp` | CPU entry point: builds the scene and starts the render |
| `Camera.h` | Camera model, viewport setup, ray generation, main render loop |
| `Hittables.h` | `Hittable` interface, `Sphere`, `Plane` |
| `HittablesList.h` | Container for all objects in a scene |
| `Materials.h` | `lambertian`, `metal`, `Dielectric` materials |
| `Textures.h` | `SolidColor` and `CheckerTexture` |
| `Bvh.h` | BVH acceleration structure |
| `Aabb.h` | Axis-aligned bounding box |
| `Lightings.h` | Directional light |
| `Ray.h` | Ray class |
| `Vectors.h` | `vec3` / `vec4` math |
| `Algebra.h` | Supporting math (2x2 matrix determinant) |
| `Color.h` | Pixel colour output and gamma correction |
| `Utilities.h` | Random number helpers, constants and the `Interval` class |

### GPU renderer

| File | Description |
|---|---|
| `GpuRenderer.cpp` | GPU entry point: window, input, camera uniforms, render loop, FPS, scene upload, demo scene |
| `raytrace.comp` | GLSL compute shader: BVH traversal, intersection, materials, path tracing loop |
| `GpuScene.h` | Plain-data scene description (materials, spheres, plane) and the flattened BVH builder |
| `CMakeLists.txt` | Optional CMake build for the GPU renderer (not needed for the Visual Studio project) |

## How the GPU renderer maps to the CPU one

| CPU | GPU |
|---|---|
| `Hittable` / `Sphere` / `Plane` classes | Flat arrays of plain structs in shader storage buffers |
| `BvhNode` with `shared_ptr` children | Flattened BVH array, traversed with an explicit stack |
| Recursive `RayColor()` | A loop in `trace()` that carries a `throughput` colour |
| `lambertian` / `metal` / `Dielectric` classes | A material `type` field and a branch in the shader |
| `std::mt19937` | PCG hash random number generator in the shader |
| Writing a `.ppm` | Accumulation texture blitted to the window |

## Adding a new feature to the GPU renderer

Every feature needs the same four steps, because the GPU cannot use C++ classes directly:

1. **Describe it as plain data** in `GpuScene.h` (a struct of floats/ints plus an `Add...` or `Set...` function).
2. **Upload it** in `GpuRenderer.cpp`, either as uniforms (a few values) or as a shader storage buffer (arrays).
3. **Declare it in `raytrace.comp`** with a matching `uniform` or buffer struct.
4. **Use it** in the shader (`hitScene()` for geometry, `trace()` for shading and lighting).

Layout rule for buffer structs: use `float[4]` / `int[4]` blocks so the C++ struct and the GLSL `std430` struct have identical size, and keep the `static_assert(sizeof(...))` checks. Shader-only changes can be tested instantly with F5.

## Reference

Built while following *[Ray Tracing in One Weekend](https://raytracing.github.io/)* by Peter Shirley, with extra features layered on top: planes, directional lighting, BVH, textures, motion blur, and a real-time GPU path tracer.
