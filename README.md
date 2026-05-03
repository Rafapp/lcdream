# LCDream
A shader framework that recreates the look of filming sub-pixel-rendered retro device LCD screens with a digital camera

## Dependencies

- CMake 3.16 or newer
- Ninja
- A C++20-capable compiler

Make sure `cmake`, `ninja`, and your compiler are available from your terminal's `PATH`.

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
