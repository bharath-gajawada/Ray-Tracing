# Ray Tracing Renderer

This repository contains a high-performance C++ ray tracer implementing advanced rendering techniques. The renderer is designed to produce high-quality 3D images using Monte Carlo integration and supports various lighting and shading models.

## Features

- **Acceleration Structures**: Implements Bounding Box Volume Hierarchy (BVH) and Axis-Aligned Bounding Boxes (AABB) for efficient ray-scene intersection.
- **Lighting Models**: Supports Point Lights, Directional Lights, and Area Lights with soft shadows.
- **Material System**: Texture mapping and BSDF evaluation.
- **Sampling**: Configurable Monte Carlo path tracing with adjustable samples per pixel (SPP).
- **Input/Output**: JSON-based scene configuration and image output support.

## Building

### Prerequisites

- A C++14 compatible compiler (GCC, Clang, or MSVC).
- CMake (Version 3.18 or higher).

### Build Instructions

1.  **Clone the repository** (ensure recursive clone if submodules are present, though dependencies appear to be included):
    ```bash
    git clone https://github.com/bharath-gajawada/Ray-Tracing.git
    cd Ray-Tracing
    ```

2.  **Create a build directory**:
    ```bash
    cd render
    mkdir build
    cd build
    ```

3.  **Configure and Compile**:
    ```bash
    cmake ..
    make -j8
    ```
    *Note for Windows users*: After running `cmake ..`, open the generated Visual Studio solution file (`.sln`) and build the `Release` configuration, or run:
    ```bash
    cmake --build . --config Release
    ```

## Usage

The renderer is executed from the command line.

```bash
./render <scene_config> <out_path> <num_samples> <sampling_strategy> <area_light_samples>
```

### Arguments

| Argument | Description |
|----------|-------------|
| `<scene_config>` | Path to the JSON scene configuration file. |
| `<out_path>` | File path for the output image (e.g., `output.png`). |
| `<num_samples>` | Number of samples per pixel (SPP). Higher values reduce noise. |
| `<sampling_strategy>` | Integer identifier for the sampling method (see below). |
| `<area_light_samples>` | Number of shadow rays to sample for area lights. |

### Sampling Strategies

The renderer supports three different sampling strategies for Monte Carlo integration (`<sampling_strategy>`):

- **`0` - Uniform Hemisphere Sampling**:
  Samples incident directions uniformly over the hemisphere centered at the surface normal. This is a basic Monte Carlo approach but converges slowly, especially for small light sources.

- **`1` - Cosine-Weighted Hemisphere Sampling**:
  Samples directions with a probability proportional to the cosine of the angle with the surface normal. This importance sampling technique reduces variance for diffuse surfaces by prioritizing directions that contribute more to the reflection.

- **`2` - Area Light Sampling (Next Event Estimation)**:
  Directly samples points on the area light sources rather than random directions. This is significantly more efficient (lower noise for the same SPP) because it ensures that rays are directed towards light sources, which is crucial for scenes with small or distant lights.

### Example

```bash
./render ../scenes/cornell_box.json output.png 128 2 4
```
*Renders the Cornell Box scene with 128 samples per pixel using Area Light Sampling (Strategy 2) and 4 light samples.*

## Project Structure

- `render/`: Contains the core C++ source files.
- `render/headers/`: Contains header files defining the renderer architecture.
- `render/extern/`: Contains third-party dependencies.
- `render/CMakeLists.txt`: CMake build configuration.
