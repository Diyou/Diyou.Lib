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

## Testing Web-Target [emscripten]

### On command-line

```shell
.cache/emsdk/source/upstream/emscripten/emrun --hostname=localhost --no_browser build/Debug.html/Examples
```

### In vscode

- Lauch emrun with [ctrl+shift+b]
- Open Browser @ http://localhost:6931

## Contributing

I'd appreciate any feedback and questions regarding the usability of the library.
