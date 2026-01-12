cmake_minimum_required(VERSION 3.27...4.2.0)

# ---- Project ----

# strongly encouraged to enable this globally to avoid conflicts between -Wpedantic being enabled
# and -std=c++23 and -std=gnu++23 for example when compiling with PCH enabled
set(CMAKE_CXX_EXTENSIONS OFF)

project(
  base32
  VERSION 0.1.1
  LANGUAGES CXX
)

option(${PROJECT_NAME}_BUILD_TESTS "Build test" OFF)
option(${PROJECT_NAME}_ENABLE_CODE_ANALYSIS "Run static code analysis" OFF)
option(${PROJECT_NAME}_ENABLE_COVERAGE "Code coverage" OFF)

option(${PROJECT_NAME}_BUILD_FUZZ_TESTS "Build fuzz tests" OFF)

# ---- Include guards ----

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(
    FATAL_ERROR
      "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
  )
endif()

# ---- Add dependencies via CPM ----
# see https://github.com/TheLartians/CPM.cmake for more info

include(cmake/CPM.cmake)

# ---- Add source files ----

# Note: globbing sources is considered bad practice as CMake's generators may not detect new files
# automatically. Keep that in mind when changing files, or explicitly mention them here.
file(GLOB_RECURSE headers CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")
file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# ---- Create static library ----

add_library(${PROJECT_NAME} STATIC ${headers} ${sources})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 23)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

# being a cross-platform target, we enforce standards conformance on MSVC
target_compile_options(${PROJECT_NAME} PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/permissive->")

# enable compiler warnings
if(NOT TEST_INSTALLED_VERSION)
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wpedantic -Wextra -Werror)
  elseif(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
  endif()
endif()

# Link dependencies
target_link_libraries(${PROJECT_NAME} PRIVATE)

target_include_directories(
  ${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                         $<INSTALL_INTERFACE:include/${PROJECT_NAME}-${PROJECT_VERSION}>
)

# ---- Static analisys ----

if(${PROJECT_NAME}_ENABLE_CODE_ANALYSIS)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    message(STATUS "Clang compiler detected. Integrating clang-tidy")
    set_target_properties(${PROJECT_NAME} PROPERTIES CXX_CLANG_TIDY "clang-tidy")
  else()
    message(STATUS "Integrating Cppcheck")
    find_program(CPPCHECK_EXECUTABLE NAMES cppcheck REQUIRED)
    set_target_properties(
      ${PROJECT_NAME}
      PROPERTIES
        CXX_CPPCHECK ${CPPCHECK_EXECUTABLE};
        --std=c++${CMAKE_CXX_STANDARD};--enable=all;--error-exitcode=10;--suppressions-list=${PROJECT_SOURCE_DIR}/cppcheck.supp;
    )
  endif()
endif()

# ---- code coverage ----
if(${PROJECT_NAME}_ENABLE_COVERAGE)
  message(STATUS "${PROJECT_NAME}_ENABLE_COVERAGE ON, compiler: ${CMAKE_CXX_COMPILER_ID}")
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(STATUS "Add coverage options for ${CMAKE_CXX_COMPILER_ID}")
    target_compile_options(${PROJECT_NAME} PUBLIC --coverage -O0 -g)
    target_link_libraries(${PROJECT_NAME} PUBLIC --coverage)
  endif()
endif()

# Don't even look at tests if we're not top level
if(NOT PROJECT_IS_TOP_LEVEL)
  return()
endif()

# ---- Tests ----

if(${PROJECT_NAME}_BUILD_TESTS)
  add_subdirectory(test)
endif()

# ---- FuzzTests ----

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND ${PROJECT_NAME}_BUILD_FUZZ_TESTS)
  target_compile_options(
    ${PROJECT_NAME}
    PUBLIC -g
           -DFUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION
           -UNDEBUG
           -fsanitize-coverage=inline-8bit-counters
           -fsanitize-coverage=trace-cmp
           -fsanitize=address
           -DADDRESS_SANITIZER
  )
  target_link_libraries(${PROJECT_NAME} PUBLIC -fsanitize=address)
  add_subdirectory(fuzz_test)
endif()

# ---- Create an installable target ----
# this allows users to install and find the library via `find_package()`.

# the location where the project's version header will be placed should match the project's regular
# header paths
string(TOLOWER ${PROJECT_NAME}/version.h VERSION_HEADER_LOCATION)

add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
install(
  TARGETS ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES
  DESTINATION include
)
install(
  EXPORT ${PROJECT_NAME}
  FILE ${PROJECT_NAME}Config.cmake
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION lib/cmake/${PROJECT_NAME}
)

# ---- Package project ----
# Experience shows that explicit package naming can help make it easier to sort out potential ABI
# related issues before they start, while helping you track a build to a specific GIT SHA
execute_process(
  COMMAND git rev-parse --short HEAD
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_SHORT_SHA
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(CPACK_PACKAGE_FILE_NAME
    "${CMAKE_PROJECT_NAME}-${CMAKE_PROJECT_VERSION}-${GIT_SHORT_SHA}-${CMAKE_SYSTEM_NAME}-${CMAKE_BUILD_TYPE}-${CMAKE_CXX_COMPILER_ID}-${CMAKE_CXX_COMPILER_VERSION}"
)

include(CPack)
