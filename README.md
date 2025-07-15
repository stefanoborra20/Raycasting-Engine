# Raycasting Engine

A minimal [raycasting](https://en.wikipedia.org/wiki/Ray_casting) engine made in **C** using [SDL2](https://www.libsdl.org).
![Image](https://github.com/user-attachments/assets/ee8c29bb-84c6-455c-99cf-c6782a16bf54)

## 🚀 Build & Run
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
### ⚙️ Optional Flags
```console
./rc -rn 10
```
- -rn <number> → Set the number of rays

## ✨ Features

- Real-time 2D/3D rendering switch
- Interactive map editor
- Resizable window
- Wall textures

## 🎮 Controls
## 🎮 Controls

| Mode         | Action                  | Key / Mouse Input     | Description                          |
|--------------|-------------------------|-----------------------|--------------------------------------|
| General      | Switch view             | `v`                   | Toggle between 2D and 3D             |
|              | Open map editor         | `m`                   | Launch the editor                    |
|              | Toggle outlines         | `p`                   | Show/hide square outlines            |
|              | Quit engine             | `q`                   | Exit the application                 |
| 2D Mode      | Move                    | `W / A / S / D`       | Navigate in 2D space                 |
|              | View direction          | `Mouse`               | Move cursor to change view           |
|              | Slow walk               | `Shift`               | Reduced movement speed               |
| 3D Mode      | Move up/down            | `W / S`               | Vertical movement                    |
|              | Turn view               | `A / D`               | Rotate left/right                    |
|              | Toggle textures         | `t`                   | Enable / disable textures            |
|              | Slow walk               | `Shift`               | Reduced movement speed               |
| Map Editor   | Add wall                | 🖱️ **Left Click**      | Place a wall tile                    |
|              | Remove wall             | 🖱️ **Right Click**     | Delete a wall tile                   |
