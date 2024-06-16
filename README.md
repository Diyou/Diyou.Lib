# Diyou.Lib

## Requirements

- cmake > 3.28
- clang > 17

## Usage

As a general entry-point See: [Template](Examples/Template.c++)

## Building

### On command-line:

```shell
#list available presets with
#> cmake --list-presets
cmake --preset=Debug::Emscripten && cmake --build --preset=Debug::Emscripten
```

#### Available Presets

- Debug::Unix (clang + Ninja)
- Release::Unix (clang + Ninja)
- Debug::Windows (MSVC)
- Release::Windows (MSVC)
- Debug::Emscripten (clang + Ninja)
- Release::Emscripten (clang + Ninja)

### In vscode with [cmake-tools](https://github.com/microsoft/vscode-cmake-tools)

Select a Configuration and Build Preset from the Status bar\
Or view Command Palette [CTRL+Shift+P]

- CMake: Select Configure Preset
- CMake: Configure
- CMake: Build [F7]

## Testing Web-Target [emscripten]

### On command-line

```shell
.cache/emsdk/upstream/emscripten/emrun --hostname=localhost --no_browser build/Debug-Emscripten/Examples
```

### In vscode

- Lauch emrun with [ctrl+shift+b]
- Open Browser @ http://localhost:6931
