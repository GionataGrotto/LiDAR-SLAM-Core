# SLAM – LiDAR odometry and 3D reconstruction

A C++17 application that builds a 3D map from a sequence of LiDAR scans
(`.pcd` files). Each new scan is aligned to the previous one with an **ICP**
(Iterative Closest Point) algorithm written from scratch; the estimated sensor
pose is then used to accumulate the points into a **global map**, rendered in
real time with OpenGL.

## Features

- **Point-to-point ICP odometry**: nearest-neighbor correspondences, outlier
  rejection by distance threshold, and rotation/translation estimation via SVD
  (Eigen).
- Custom 3D **KD-tree** to speed up nearest-neighbor search.
- **Asynchronous loading** of frames: binary PCD files are parsed in a separate
  thread, without blocking the renderer.
- **OpenGL 3.3 viewer** with a reference grid, a point-cloud map and a free
  FPS-style camera.

## Project structure

```
include/   headers (Camera, KDTree, Odometry, Scene, Shader, Utils, Visualizer)
src/       implementations and main
shaders/   GLSL vertex and fragment shaders
```

| Module       | Role                                                        |
|--------------|-------------------------------------------------------------|
| `Odometry`   | ICP: rigid transform estimation between two scans           |
| `KDTree`     | Spatial structure for closest-point queries                 |
| `Scene`      | Asynchronous frame loading                                  |
| `Utils`      | Binary PCD parser, keyboard input                           |
| `Visualizer` | Rendering of the grid and the global map                    |
| `Camera`     | Free camera driven by keyboard and mouse                    |

## Dependencies

| Library | Purpose                         |
|---------|---------------------------------|
| GLFW    | window and input                |
| GLAD    | OpenGL function loading         |
| GLM     | graphics math                   |
| Eigen   | linear algebra (SVD)            |

Dependencies are managed with [vcpkg](https://github.com/microsoft/vcpkg).

## Building

Requirements: CMake, a C++17 compiler (e.g. Visual Studio 2022 on Windows) and
vcpkg.

```bash
vcpkg install glfw3 glm glad eigen3
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=<path-to-vcpkg>/scripts/buildsystems/vcpkg.cmake
cmake --build build --config Release
```

The shaders are copied into the build folder at configure time. Run the
program from the `build` folder so that it can find `shaders/`.

## Dataset

The program reads a folder of **binary** `.pcd` files (XYZ fields as 32-bit
floats), sorted by name. It has been tried with the Anovox sample data.

The folder path is currently hard-coded in `src/main.cpp` (the `pcdPath`
variable): change it to the location of your dataset before building.

## Controls

| Key             | Action                              |
|-----------------|-------------------------------------|
| `W` `A` `S` `D` | move the camera                     |
| `E` / `Q`       | move up / down                      |
| `TAB`           | toggle mouse look                   |
| `SPACE`         | pause / resume the reconstruction   |
| `ESC`           | quit                                |

When the sequence ends the reconstruction pauses, and you can freely explore
the final map.

## How it works

1. Frame *t* is loaded and compared with frame *t−1*.
2. For each subsampled point, the nearest neighbor is looked up in the KD-tree;
   pairs that are too far apart are discarded.
3. The centroids and the cross-covariance matrix are computed; its SVD gives the
   optimal rotation, from which the translation follows.
4. The step is applied and the process repeats for a fixed number of iterations.
5. The relative transform updates the sensor pose, which carries the points of
   the current frame into the global reference frame.

## Limitations and future work

- Subsampling is fixed and ICP has no convergence criterion.
- No loop closure or global optimization (pose graph): drift accumulates along
  the trajectory.
- The dataset path should be made configurable (command-line argument).
- Support for PCD files with extra fields (intensity, color) and the ASCII
  format.

## License

To be defined.
