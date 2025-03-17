# Raycasting Engine

A minimal raycasting engine made in C and [SDL2](https://www.libsdl.org).

## Build/Run/Debug/Flags
```console
make
./rc
```
or
```console
make debug
./rc
```
for debug mode.
### Flags
Flags are optionals
```console
./rc -rn 10
```
- rn %d (rays number)
## Features

- 2D map view
- 3D map view
- Resizable window
- Map editor

## Usage/Examples

- [v] Switch 2D/3D mode
- [m] Map editor
- [p] Enable/disable squares outlines
- [q] Quit engine

### 2D mode
- [WASD] Movement
- [Mouse] View direction
- [Shift] Slow walk

### 3D mode
- [w] Up
- [s] Down
- [a/d] Turn view left/right 
- [Shift] Slow walk

### Map editor
- [Left Click]  Put wall
- [Right Click] Remove wall
