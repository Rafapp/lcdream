# LCDream
A shader framework that recreates the look of filming sub-pixel-rendered retro device LCD screens with a digital camera

## Dependencies

- CMake 3.16 or newer
- Ninja
- A C++20-capable compiler
- Git
- Python 3, used by the glad generator during CMake configure/build
- Python module `jinja2`, required by glad code generation

`build.ninja` is generated automatically by CMake when configuring with the Ninja generator.

Make sure `cmake`, `ninja`, `git`, Python, and your compiler are available from your terminal's `PATH`.
If `jinja2` is missing, install it with:

```sh
python -m pip install jinja2
```

## Vendor Dependencies

Third-party code lives under `vendor` as git submodules:

- GLFW: window and input handling
- GLM: header-only math library
- glad: OpenGL loader generation
- ImGui: immediate-mode UI

`build.bat` initializes and updates these automatically on first run — no manual step needed after cloning.

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
