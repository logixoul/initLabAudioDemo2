# Описание на репото

В това репо е "заготовката" в/у която градим в уъркшопа.
В нея са включени библиотеките miniaudio, imgui и sokol, и са wrap-нати удобно за целите на уъркшопа, така че реално application-specific кода е концентриран в един единствен хедър файл - WorkshopApp.h, за да се фокусираме в/у важното в живота - писането на музикален код!

Бранчовете са два - `main` (bare-bones версията, от където тръгва уъркшопа - само една синусоида и един слайдър) и `fully-developed` (това, което в крайна сметка ни е target-а - това, което ще изградим заедно по продължителността на уъркшопа)

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

## Misc notes

compile_flags.txt exists just for the clangd vscode extension to use.
