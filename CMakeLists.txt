cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0091 NEW)

set(CMAKE_TOOLCHAIN_FILE ./conan_toolchain.cmake)

project(pulsar-engine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ./build/$<0:>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ./build/$<0:>)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ./build/$<0:>)

if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()


find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

FILE(GLOB_RECURSE ENGINE_SRC ./src/*.cpp)

add_library(pulsar-engine ${ENGINE_SRC})
target_include_directories(pulsar-engine PUBLIC ./src)
target_link_libraries(pulsar-engine PUBLIC Vulkan::Vulkan glfw)

enable_testing()

if (WIN32)
    set(NO_CONSOLE WIN32)
endif()

FILE(GLOB_RECURSE TEST_SRC ./test/*.cpp)

add_compile_options(NO_CONSOLE)
add_executable(pulsar-engine-test ${TEST_SRC})
target_include_directories(pulsar-engine-test PRIVATE ./test PRIVATE ./src)
target_link_libraries(pulsar-engine-test PRIVATE pulsar-engine Vulkan::Vulkan glfw)

#TODO add some tests