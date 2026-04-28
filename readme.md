# mt_lib

A minimalisctic implementation of threadsafe data structures, algorithms and utils.
CMake-based C++ library project that builds on **Linux**, and can be consumed via `find_package()` after installation.

## Initial requirements

- CMake 3.22+
- Make
- A C++17 compiler:
  - Linux: GCC or Clang

## Build

Out-of-source builds are recommended:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

### Build a shared library

```bash
cmake -S . -B build -DBUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

## Install

### Linux

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="$HOME/.local"
cmake --build build
cmake --install build
```

## Use from another CMake project

After installing, you can consume the library with:

```cmake
find_package(mt_lib CONFIG REQUIRED)
target_link_libraries(your_app PRIVATE mt_lib::mt_lib)
```

If installed to a non-standard prefix, point CMake at it:

- Linux:

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH="$HOME/.local"
```

## Usage examples

## TODO:

- Make it build on macOS and Windows too
- add test option
