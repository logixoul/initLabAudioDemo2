compile_flags.txt exists just for the clangd vscode extension to use.

## Setup on Ubuntu

```sh
sudo apt update
sudo apt install build-essential clang git libx11-dev libxi-dev libxcursor-dev libgl1-mesa-dev 
```

## Setup on Fedora

```sh
sudo dnf install clang gcc-c++ make git libX11-devel libXi-devel libXcursor-devel mesa-libGL-devel
```

## Setup on Arch

```sh
sudo pacman -S --needed base-devel clang git libx11 libxi libxcursor mesa
```

## Setup on macOS

```sh
xcode-select --install
```

This installs Apple's Command Line Tools, which include `clang++`, `make` and `git`. The system frameworks the app links against (Cocoa, OpenGL, CoreAudio, …) come with macOS.

## Building and running

```sh
make
./build/synth
```
