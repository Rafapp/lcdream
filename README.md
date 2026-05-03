# LCDream
A shader framework that recreates the look of filming sub-pixel-rendered retro device LCD screens with a digital camera

## Dependencies

- CMake 3.16 or newer
- Ninja
- A C++20-capable compiler
- Git
- Python 3, used by the glad generator during CMake configure/build

Make sure `cmake`, `ninja`, `git`, Python, and your compiler are available from your terminal's `PATH`.

## Vendor Dependencies

Third-party code lives under `vendor` as git submodules:

- GLFW: window and input handling
- GLM: header-only math library
- glad: OpenGL loader generation

Add the submodules once from the repository root:

```sh
git submodule add https://github.com/glfw/glfw.git vendor/glfw
git submodule add https://github.com/g-truc/glm.git vendor/glm
git submodule add -b glad2 https://github.com/Dav1dde/glad.git vendor/glad
git submodule update --init --recursive
```

After they are added, `build.bat` automatically runs:

```sh
git submodule update --init --recursive --jobs 3 -- vendor/glfw vendor/glm vendor/glad
```

## Building

On Windows, use the included build script from the repository root:

```bat
build.bat --release
```

For a debug build:

```bat
build.bat --debug
```

Running `build.bat` with no flag defaults to a release build. The executable is written to:

```text
build\<Debug|Release>\lcdream.exe
```

The script configures CMake the first time a build folder is created. Later builds reuse the existing CMake cache and go straight to the build step for faster edit-build-run loops.

To force CMake to regenerate the build files:

```bat
build.bat --debug --reconfigure
```

You can also build directly with CMake:

```sh
cmake -S . -B build/Release -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build/Release --parallel
```
