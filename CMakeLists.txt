cmake_minimum_required(VERSION 3.22 VERSION 0.1.0)

project(2DPlatformer)

set(HEADER_FILES src/pch/minimal-2d-platformer-sdl2-pch.hpp)

file(GLOB_RECURSE SOURCE_FILES "src/*.cpp" "src/*.hpp")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(STANDARD_CXX_VERSION_FLAG "-std=c++11")
set(OPTIMIZE_FLAG "-O2")
set(WARNING_FLAGS "-Werror -Wpedantic -Wall -Wno-zero-as-null-pointer-constant -Wno-unsafe-buffer-usage -Wno-c++98-compat -Wno-missing-prototypes -Wno-unused-parameter")

# Set Directories for SDL2
set(SDL2_DIR "C:\\vclib\\SDL2-2.30.5\\cmake")
set(SDL2_INCLUDE_DIRS "C:\\vclib\\SDL2-2.30.5\\include")
set(SDL2_LIB_DIR "C:\\vclib\\SDL2-2.30.5\\lib\\x64")

# Set Directories for SDL2_image
set(SDL2_image_DIR "C:\\vclib\\SDL2_image-2.8.2\\cmake")
set(SDL2_IMAGE_INCLUDE_DIR "C:\\vclib\\SDL2_image-2.8.2\\include")

# Set the library path for the SDL2_image.lib file
set(SDL2_IMAGE_LIB "C:\\vclib\\SDL2_image-2.8.2\\lib\\x64\\SDL2_image.lib")

# Set Directories for SDL2_mixer
set(SDL2_mixer_DIR "C:\\vclib\\SDL2_mixer-2.8.0\\cmake")
set(SDL2_MIXER_INCLUDE_DIR "C:\\vclib\\SDL2_mixer-2.8.0\\include")

# Set the library path for the SDL2_mixer.lib file
set(SDL2_MIXER_LIB "C:\\vclib\\SDL2_mixer-2.8.0\\lib\\x64\\SDL2_mixer.lib")

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIR} ${SDL2_MIXER_INCLUDE_DIR})

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
file(COPY lint_codebase.sh DESTINATION ${CMAKE_BINARY_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OPTIMIZE_FLAG} ${WARNING_FLAGS}")

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC include)

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIB} ${SDL2_MIXER_LIB})

target_precompile_headers(${PROJECT_NAME} PRIVATE ${HEADER_FILES})
