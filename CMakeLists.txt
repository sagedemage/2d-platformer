cmake_minimum_required(VERSION 3.22 VERSION 0.1.0)

project(2DPlatformer)

set(HEADER_FILES src/pch/minimal-2d-platformer-sdl2-pch.hpp)

file(GLOB_RECURSE SOURCE_FILES "src/*.cpp" "src/*.hpp")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(STANDARD_CXX_VERSION_FLAG "-std=c++11")
set(OPTIMIZE_FLAG "-O3")
set(WARNING_FLAGS "-Werror -Wpedantic -Wall -Wextra")

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
file(COPY lint_codebase.sh DESTINATION ${CMAKE_BINARY_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${STANDARD_CXX_VERSION_FLAG} ${OPTIMIZE_FLAG} ${WARNING_FLAGS}")

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC include)

target_link_libraries(${PROJECT_NAME} -lSDL2 -lSDL2_mixer -lSDL2_image)

target_precompile_headers(${PROJECT_NAME} PRIVATE ${HEADER_FILES})
