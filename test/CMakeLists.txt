cmake_minimum_required(VERSION 3.27...4.2.0)

set(TESTED_PROJECT_NAME CppStaticLib)

project(${TESTED_PROJECT_NAME}Tests LANGUAGES CXX)

# ---- Options ----

option(${TESTED_PROJECT_NAME}_ENABLE_COVERAGE "Enable test coverage" OFF)
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

# ---- Create binary ----

file(GLOB sources CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_executable(${PROJECT_NAME} ${sources})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 23)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../include")

target_link_libraries(${PROJECT_NAME} PRIVATE Boost::ut ${TESTED_PROJECT_NAME})

target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR})

# enable compiler warnings
if(NOT TEST_INSTALLED_VERSION)
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wpedantic -Wextra -Werror)
  elseif(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
  endif()
endif()

# ---- Add Tests ----

enable_testing()
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME} -s)

# ---- code coverage ----

if(${TESTED_PROJECT_NAME}_ENABLE_COVERAGE)
  message(STATUS "${TESTED_PROJECT_NAME}_ENABLE_COVERAGE ON, compiler: ${CMAKE_CXX_COMPILER_ID}")
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(STATUS "Add coverage options for ${CMAKE_CXX_COMPILER_ID}")
    target_compile_options(${PROJECT_NAME} INTERFACE --coverage -O0 -g)
    target_link_libraries(${PROJECT_NAME} INTERFACE --coverage)
  endif()
endif()
