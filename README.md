# Cpp Vulkan Template

Template for development with the Vulkan API using C++ as the programming language.

## Minimum requirements

| Software        | Platform        |
| --------------- | --------------- |
| Python 3.11+    | Windows / Linux |
| CMake 3.26+     | Windows / Linux |
| GCC 11+         | Windows / Linux |
| Vulkan SDK 1.4+ | Windows / Linux |

## Clone the repositories

```bash
py Scripts/bootstrap.py
```

## Build project (Windows)

```bash
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
```

## Run project

```bash
cmake --build build
```