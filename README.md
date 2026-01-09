# CppStaticLib

[![ci](https://github.com/MarkRakhmatov/CppStaticLibTemplate/actions/workflows/ci.yml/badge.svg)](https://github.com/MarkRakhmatov/CppStaticLibTemplate/actions/workflows/ci.yml)

## About
C++ 23 static library template

### By default:
* Warnings as errors
* clang-tidy and cppcheck static analysis
* [CPM](https://github.com/cpm-cmake/CPM.cmake) for dependencies

### It includes

* unit testing using [UT/μt](https://github.com/boost-ext/ut)
* fuzz testing using [fuzztest](https://github.com/google/fuzztest)
* tooling to verify consistent commits format inspired by [conventional commits](https://www.conventionalcommits.org/en/v1.0.0/)
* autocreation of [version](https://semver.org/) tags in main branch based on commits

#### Commits format

1. API breaking change (will increment major version)
For example: removal of public interface parts after introducing new version of API
Please, provide short description with reasons and migration hints if needed
```
api: remove deprecated v1 API
api(parser): hide parser API from public interface
```

2. New feature or backward compatible API change (will increment minor version and reset patch to 0)
```
feat: introduce parser v2 API
feat(parser): extend parser v2 API public interface
```

3. Fix of bug in existing logic
```
fix: bug with list parsing in v2 API
refactor(parser): reduce heap allocations in parser v2 API
```

### It requires

* cmake
* a compiler

### Platforms and compilers support matrix:

| | Windows | Ubuntu     |
|-------|-----|----------|
| MSVC |:heavy_check_mark: | |
| clang | | :heavy_check_mark: |
| gcc | | :heavy_check_mark: |
