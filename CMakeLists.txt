cmake_minimum_required(VERSION 3.10)

project(PerlinNoise)

add_subdirectory(deps/RenderSurface)

include_directories(${CMAKE_SOURCE_DIR}/include)
file(GLOB SOURCES "src/*.cpp")

add_executable(PerlinNoise ${SOURCES})

target_link_libraries(PerlinNoise PRIVATE RenderSurface)