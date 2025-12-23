# %%myproject%%

[![ci](https://github.com/%%myorg%%/%%myproject%%/actions/workflows/ci.yml/badge.svg)](https://github.com/%%myorg%%/%%myproject%%/actions/workflows/ci.yml)

## About %%myproject%%
%%description%%

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
To use %%myproject%% library in cmake project:

```
CPMAddPackage("gh:%%myorg%%/%%myproject%%@v0.1.0")
```

Link %%myproject%% library to your target
```
target_link_libraries(your_target PRIVATE %%myproject%%)
```

