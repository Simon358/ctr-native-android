# CTR Native Android Port

This project has been ported to Android using SDL3.

## Building

1.  **Clone with submodules**: 
    `git clone --recursive https://github.com/Simon358/ctr-native-android`
2.  **Open in Android Studio**: Open the `android` folder.
3.  **Install NDK**: Go to `Settings > SDK Tools` and install `NDK (Side by side)` and `CMake`.
4.  **Sync Gradle**: Click the "Elephant" icon.
5.  **Build & Run**: The project is configured for 32-bit (`armeabi-v7a`) to maintain compatibility with the game's memory model.

## Assets Setup (Modern Android)

The game requires retail NTSC-U assets to run. On modern Android (11+), you have two options:

### Option 1: The Easy Way (Folder Picker)
1.  Place your game files (e.g., `BIGFILE.BIG` or `ctr-u.bin`) in a folder you can easily access, like `Documents/CTR/assets/`.
2.  Launch the app. It will ask for **"All Files Access"** permission. Enable this so the game can read your selected folder.
3.  In the "Missing Assets" dialog, click **"Select Folder"**.
4.  Navigate to and select the folder where your assets are stored.
5.  **Restart the app**.

### Option 2: The Manual Way (PC/Root)
Place your `ctr-u.bin` or extracted assets folder into the app's private data directory:
`/storage/emulated/0/Android/data/com.ctrnative/files/assets/`

## Troubleshooting

-   **Black Screen/Crash**: Ensure you have the correct NTSC-U retail assets. Check **Logcat** in Android Studio using the tag `CTR-Native`.
-   **OpenGL Errors**: This port requires **OpenGL ES 3.0**. Ensure your device supports it. If using an emulator, set Graphics to "Hardware".
-   **Permissions**: If the folder picker doesn't work, ensure "All Files Access" is enabled for "CTR Native" in your phone's System Settings.

## Controls

- Supports standard Android gamepads (Backbone One, Xbox, DualShock) via SDL3.
- Touch controls are currently not implemented.
