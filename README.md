# CTR Native

A native PC and Android port of Crash Team Racing (PS1, 1999), built on top of the [CTR-ModSDK](https://github.com/CTR-tools/CTR-ModSDK) decompilation project.

## Philosophy

- **No byte budget.** Game source lives in `game/` as our own copies. Edit freely.
- **No PSX toolchain.** Targets Windows, Linux, and Android with SDL3. No MIPS compiler needed.
- **Clean platform layer.** `main.c` owns process startup; host details stay in `platform/native_*`.
- **Fully static build.** Linked statically with SDL3 for a single-file executable or APK.

## Prerequisites

### Windows
- Visual Studio 2022 (MSVC x86) or MSYS2 (MinGW i686).
- CMake 3.20+.

### Linux (Debian/Ubuntu)
- `sudo apt install gcc-multilib libx11-dev libgl1-mesa-dev libasound2-dev libudev-dev libdbus-1-dev`.

### Android
- **Android Studio**.
- **Android NDK** (Side-by-side) and **CMake** (installed via SDK Manager > SDK Tools).
- A physical device or emulator supporting **OpenGL ES 3.0**.

## Building

### PC (Windows/Linux)
Run `build.bat` (MinGW), `build-msvc.bat` (MSVC), or `build.sh` (Linux).

### Android
1. Open the `android/` folder in Android Studio.
2. Sync Gradle and click **Run**.
3. The build targets `armeabi-v7a` (32-bit) for memory model compatibility.

*Note: Always clone with `--recursive` or run `git submodule update --init --recursive` to fetch SDL3.*

## Assets Setup

The game requires retail NTSC-U assets (disc image `ctr-u.bin` or extracted files like `BIGFILE.BIG`).

### PC
Place an `assets/` folder containing your files next to the executable.

### Android
1. **Easy Way**: Place assets in `Documents/CTR/assets/`. Launch the app, grant **"All Files Access"**, and use the **"Select Folder"** button in the startup dialog.
2. **Manual Way**: Place assets in `/storage/emulated/0/Android/data/com.ctrnative/files/assets/`.

## Controls
- **PC**: Keyboard and Gamepads supported via SDL3.
- **Android**: Gamepads supported. Touch controls are planned.

## Troubleshooting
- **Logcat**: Filter for the tag `CTR-Native` to see engine initialization logs.
- **GLES**: Ensure your Android device supports **OpenGL ES 3.0**.

## Credits
- [CTR-ModSDK](https://github.com/CTR-tools/CTR-ModSDK) — the decompilation foundation.
- [PsyCross](https://github.com/OpenDriver2/PsyCross) — original PS1 compatibility code base.
- [SDL3](https://github.com/libsdl-org/SDL) — cross-platform multimedia.
- Crash Team Racing is a trademark of Sony Computer Entertainment / Naughty Dog.
