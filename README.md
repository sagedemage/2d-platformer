# minimal-2d-platformer-sdl2

## Gameplay
![](screenshots/2d_platformer.webp)

## About
A simple 2D palformer game written in C using SDL2. This game is only supported for Linux. This game is in alpha software. There are going to be features missing.

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

## Setup Editor for VSCode
Go to `~/.config/Code/User/settings.json`
Add this line if it does not exist
```
"C_Cpp.clang_format_fallbackStyle": "{ BaseOnStyle: Google, IndentWidth: 4, TabWidth: 4 }"
```

## Assets (Music, Art and Soundeffects)
<a rel="license" href="http://creativecommons.org/licenses/by/4.0/"><img alt="Creative Commons License" style="border-width:0" src="https://i.creativecommons.org/l/by/4.0/88x31.png" /></a><br />I created the music, art and soundeffects for this game. The assets are licensed under a <a rel="license" href="http://creativecommons.org/licenses/by/4.0/">Creative Commons Attribution 4.0 International License</a>.
