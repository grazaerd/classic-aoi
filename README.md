# DNCLASSIC
## Official clients only.
> - Tested on SEA. 
> - No private server support.

## Disclaimer
**Use at your own risk, especially getting banned or losing your raids.** I haven't fully tested this on dungeon, raids, etc., only tested in town.

## List of features
- Fix misaligned particles.
- Camera Shake
- Camera Blur (Radial Blur in shaders)
- FOV
- Camera Zoom

## How to use it
```
└── Dragon Nest Folder
    ├── ddraw.dll
    └── dragonnest_reborn.exe
```
## Building

### Requirements
- [ninja](https://github.com/ninja-build/ninja)
- [CMake](https://cmake.org/download/)
- [llvm-mingw](https://github.com/mstorsjo/llvm-mingw/releases) (mingw clang)

Make sure `ninja`, `cmake`, `clang`, `clang++` and `lld` are available on your `PATH`.

### Steps

```sh
cmake -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_C_COMPILER=clang \
  -DCMAKE_CXX_COMPILER=clang++ \
  -DCMAKE_EXE_LINKER_FLAGS="-fuse-ld=lld" \
  -DCMAKE_SHARED_LINKER_FLAGS="-fuse-ld=lld"

ninja
```

## Issue
If you found an issue while using this, please create an issue on [Issues Section](https://github.com/grazaerd/classic-aoi/issues), and make sure to test it thoroughly, without and with the mod. If the issue is hard to notice, attach a video, otherwise, attach a picture.
