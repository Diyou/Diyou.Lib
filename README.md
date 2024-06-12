# Diyou.Lib

## Requirements

- cmake > 3.28
- clang > 17

## Building

### On command-line:

```shell
#list available presets with
#> cmake --list-presets
cmake --preset=Debug && cmake --preset=Debug --build
```

### In vscode with [cmake-tools](https://github.com/microsoft/vscode-cmake-tools)

Select a Configuration and Build Preset from the Status bar\
Or view Command Palette [CTRL+Shift+P]

- CMake: Select Configure Preset
- CMake: Configure
- CMake: Build [F7]
