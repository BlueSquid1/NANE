# NANE - Not Another Nes Emulator
## Details
This is a Nintendo Entertainment System (NES) Emulator for the nintendo switch based of [Another NES Emulator (ANESE)](https://github.com/daniel5151/ANESE).

I have only tested this on the SX OS Pro Custom Firmware (CFW) however it should be relatively easy to get running on other CFWs such as the open source CFW [Atmosphere](https://github.com/Atmosphere-NX/Atmosphere).

## Installing
Note: In order to run custom .nro files on a Nintendo Switch you first need to install a CFW. I recommend following the [Switch Homebrew Guide](https://switch.homebrew.guide/gettingstarted/beforestarting).

Copy the NANE.nro to the root directory of your Switch's SD card. Boot into your Nintendo Switch's CFW and run the .nro file with any homebrew launcher.

## How to build from source:
### Install CMake
For MacOS:
```
brew install cmake
```

### Install DevKitPro
For MacOS:
Run devkitpro-pacman.pkg from [devkitpro-pacman](https://github.com/devkitPro/pacman/releases/tag/devkitpro-pacman-1.0.1)
```
dkp-pacman -S switch-dev  -r /System/Volumes/Data
```
log out and back in to refresh evironment variables

### Install SDL2
For MacOS:
```
brew install sdl2
brew install sdl2_ttf
```

### install Catch2 (for unit tests only)
For MacOS:
```
brew install catch2
```

### build source
```
sh ./deploy.sh
```