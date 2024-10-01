# minimal-2d-platformer-sdl2

## Gameplay
![](screenshots/2d_platformer.webp)

## About
A simple 2D palformer game written in C++ using SDL2.

## Dependencies
* gcc (GNU GCC Compiler)
	* build-essential (Debian and Ubuntu distros and its derivatives)
* libsdl2-dev
* libsdl2-mixer-dev
* libsld2-image-dev
* cmake

## Manual Install (Ubuntu 22.04.2 LTS)
[manual_install_sdl2](./manual_install_sdl2.md)

## Building the project
Go the root of my repository
```
cd minimal-2d-platformer-sdl2
```

Set up the build directory
```
cmake -B build
```

Go to build directory
```
cd build
```

Compile the project
```
cmake --build .
```
