cmake_minimum_required(VERSION 3.27...4.2.0)

set(TESTED_PROJECT_NAME CppStaticLib)

project(${TESTED_PROJECT_NAME}FuzzTests LANGUAGES CXX)

# ---- Options ----

option(TEST_INSTALLED_VERSION "Test the version found by find_package" OFF)

# ---- Dependencies ----

include(../cmake/CPM.cmake)

if(TEST_INSTALLED_VERSION)
  find_package(${TESTED_PROJECT_NAME} CONFIG REQUIRED) # for intro, project_options, ...

  if(NOT TARGET ${TESTED_PROJECT_NAME}_options)
    message(FATAL_ERROR "Required config package not found!")
    return() # be strictly paranoid for Template Janitor github action! CK
  endif()
endif()

CPMAddPackage("gh:boost-ext/ut@2.3.1")

CPMAddPackage(
    URI "gh:google/fuzztest#2025-08-05"
    OPTIONS "FUZZTEST_FUZZING_MODE ON"
)

# ---- Create binary ----

file(GLOB sources CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_executable(${PROJECT_NAME} ${sources})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 23)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../include")

target_link_libraries(
  ${PROJECT_NAME}
    PRIVATE
    ${TESTED_PROJECT_NAME})

target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR})

link_fuzztest(${PROJECT_NAME})
gtest_discover_tests(${PROJECT_NAME})

# enable compiler warnings
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wpedantic -Wextra -Werror)

# ---- Add Tests ----

enable_testing()
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME} --fuzz_for=15s)
