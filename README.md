# ProjectSarah

A physically-based ray tracer built from scratch in C++, following along with and extending *Ray Tracing in One Weekend* by Peter Shirley.

<img width="4096" height="2155" alt="render_full" src="https://github.com/user-attachments/assets/9d56e76c-3072-4341-9f02-023f90fd9a11" />

*Render as of 02-10-2026*

## Features

- **Primitives** — spheres (including moving spheres, for motion blur) and infinite planes
- **Materials**
  - Lambertian diffuse reflection, driven by any texture (a plain color is wrapped in a solid-color texture)
  - Metal reflection with adjustable fuzz/roughness
  - Dielectric (glass) refraction, using Snell's law and Schlick's approximation for reflectance
- **Textures**
  - Abstract `texture` interface, so materials don't care where their color comes from
  - Solid color texture
  - Procedural 3D checker texture with adjustable scale
  - Checker squares can hold *any* texture, not just colors, so textures can be nested (e.g. a checker of checkers)
- **Camera**
  - Configurable field of view and image resolution
  - Positionable via look-from / look-at / view-up vectors
  - Defocus blur (depth of field) via a lens-radius model
  - Motion blur via time-sampled rays
- **Acceleration** — a Bounding Volume Hierarchy (BVH) built over axis-aligned bounding boxes (AABB), for faster ray-scene intersection on larger scenes
- **Antialiasing** — multiple samples per pixel with random sub-pixel jitter
- **Gamma correction** — gamma-2 correction and clamping applied when writing pixels
- **Lighting** — a basic directional light model (still in progress)
- **Output** — renders directly to a `.ppm` image, and reports total render time on completion

## Build

This is a Windows / Visual Studio project (MSVC, C++20).

1. Clone the repo
2. Open `ProjectSarah.slnx` in Visual Studio
3. Switch the configuration to **Release** (Debug is several times slower for rendering)
4. Build and run — the rendered image is written to `render.ppm` in the working directory

`.ppm` files are large and not widely supported by viewers. Convert the output to PNG or JPG, for example with ImageMagick:

```
magick render.ppm render.png
```

## Project structure

| File | Description |
|---|---|
| `renderer.cpp` | Entry point — builds the scene and starts the render |
| `Camera.h` | Camera model, viewport setup, ray generation, main render loop |
| `Hittables.h` | `Hittable` interface, `Sphere`, `Plane` |
| `HittablesList.h` | Container for all objects in a scene |
| `Materials.h` | `lambertian`, `metal`, `Dielectric` materials |
| `Textures.h` | `texture` interface, `SolidColor`, `CheckerTexture` |
| `Bvh.h` | BVH acceleration structure |
| `Aabb.h` | Axis-aligned bounding box |
| `Lightings.h` | Directional light |
| `Ray.h` | Ray class |
| `Vectors.h` | `vec3` math |
| `Algebra.h` | Supporting math (2×2 matrix determinant) |
| `Color.h` | `Color` class and gamma-corrected pixel output |
| `Utilities.h` | Random number helpers, constants, and the `Interval` class |

## Usage example

Textures plug into materials, so a checkered ground plane looks like this:

```cpp
// Two-color checker, 0.5 units per square
auto checker = std::make_shared<CheckerTexture>(0.5, Color(0.05, 0.05, 0.8), Color(0.9, 0.9, 0.9));

// Use it as the albedo of a diffuse material
auto ground = std::make_shared<lambertian>(checker);

Plane floor(vec3(0.0, -1.0, -2.0), vec3(0.0, 1.0, 0.0), ground);
```

## Roadmap

- UV coordinates for spheres and planes (the texture interface already takes `u` and `v`; the current checker is based on 3D position)
- Image textures
- Multithreaded rendering
- Improved lighting

## Reference

Built while following *[Ray Tracing in One Weekend](https://raytracing.github.io/)* by Peter Shirley, with extra features layered on top — planes, directional lighting, BVH, motion blur, and textures.
