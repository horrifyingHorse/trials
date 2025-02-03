cmake_minimum_required(VERSION 3.25)

project(
  ProjectRayLib
  LANGUAGES CXX
  VERSION 0.6.9)

set(CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/raylib/")
include_directories(raylib/include include/)
link_directories(raylib/)

file(GLOB SOURCES src/*.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})

find_package(raylib REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE raylib)
