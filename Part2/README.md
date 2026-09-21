# Part 2: Haversine Solver

A C++ project for following along with Part 2 of Casey Muratori's **Computer Enhance** course: generating input JSON, parsing it, and optimizing a program that calculates Haversine distances between pairs of geographic coordinates.

The Haversine formula computes the great-circle distance between two points on a sphere using their latitude and longitude. This project builds toward a complete pipeline from generated test data to a measured, optimized distance calculation.

## Progress

- **JSON generation:** implemented; generates random coordinate pairs and writes them to a file.
- **JSON parsing:** planned.
- **Haversine calculation:** planned; the `-solve` command currently only prints a placeholder message.
- **Performance measurement and optimization:** planned, after the baseline solver is working.

## Build

Requirements: CMake 4.2 or newer and a C++ compiler. The current generator uses a variable-length stack array, which requires compiler extension support (such as Clang or GCC).

From the `Part2` directory:

```sh
cmake -S . -B build
cmake --build build
```

## Generate input JSON

```sh
./build/haversine -generate 100 -o sample_data.json
```

This writes 100 coordinate pairs to `sample_data.json`, replacing the file if it already exists. Use a positive integer for the sample size and always provide `-o <output file>`; the default output path is not currently handled safely.

Each run uses a seed from `std::random_device`. A user-supplied seed (`-s`) is not yet supported.

### JSON format

The input contains a top-level `pairs` array:

```json
{
  "pairs": [
    { "x0": 37, "y0": -122, "x1": 40, "y1": -74 }
  ]
}
```

Each entry describes two points, `(x0, y0)` and `(x1, y1)`. In the **current generator**, `x0` and `x1` are integer latitudes in `[-90, 90]`, and `y0` and `y1` are integer longitudes in `[-180, 180]`, all in degrees. The parser and solver will need to follow the same coordinate convention.

## Solve input JSON

The intended command is:

```sh
./build/haversine -solve sample_data.json
```

Currently, this prints `Solve the JSON.` without reading the file or calculating distances.

## Project layout

| File | Purpose |
| --- | --- |
| `CMakeLists.txt` | Builds the `haversine` executable. |
| `src/Main.cpp` | Command-line handling and mode selection. |
| `src/Unity.cpp` | Includes implementation files into a single translation unit. |
| `src/JsonGenerator.h` | Coordinate-pair structure and generator declaration. |
| `src/JsonGenerator.cpp` | Random coordinate generation and JSON output. |
| `sample_data.json` | Example generated input. |

## Next steps

1. Improve argument validation and implement output-file defaults and seed selection.
2. Parse the generated JSON into coordinate pairs.
3. Implement and verify a baseline Haversine calculation.
4. Measure parsing and calculation performance, identify bottlenecks, and compare optimizations against the baseline.

The generator currently stores all pairs on the stack before writing them, so large sample sizes may exceed the available stack space.
