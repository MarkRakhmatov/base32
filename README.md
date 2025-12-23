# base32

[![ci](https://github.com/MarkRakhmatov/base32/actions/workflows/ci.yml/badge.svg)](https://github.com/MarkRakhmatov/base32/actions/workflows/ci.yml)

## About base32
C++20 Base32 encoding/decoding library

### By default:
* Warnings as errors
* clang-tidy and cppcheck static analysis
* [CPM](https://github.com/cpm-cmake/CPM.cmake) for dependencies

### It includes

* unit testing using [UT/μt](https://github.com/boost-ext/ut)
* fuzz testing using [fuzztest](https://github.com/google/fuzztest)
* single GitHub action workflow

### It requires

* cmake
* a compiler

### Platforms and compilers support matrix:

| | Windows | Ubuntu     |
|-------|-----|----------|
| MSVC |:heavy_check_mark: | |
| clang | | :heavy_check_mark: |
| gcc | | :heavy_check_mark: |

### Usage
To use base32 library in cmake project:

```
CPMAddPackage("gh:MarkRakhmatov/base32@v0.1.0")
```

Link base32 library to your target
```
target_link_libraries(your_target PRIVATE base32)
```

