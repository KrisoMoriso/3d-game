# 3D Voxel Game

A voxel game based on Minecraft.

## Prerequisites

- **CMake** (version 3.31 or higher)
- **C++ compiler** supporting C++20 (MSVC, GCC, or Clang)
- **Git**

## General Build Steps (All Platforms)

```bash
# Clone the repository
git clone https://github.com/KrisoMoriso/3d-game.git
cd 3d-game

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# Build
cmake --build . --config Release
```

---

## Visual Studio (Windows)

1. Install CMake and Visual Studio (with C++ workload)
2. In Visual Studio, go to **File → Open → CMake**
3. Navigate to your repository's `CMakeLists.txt`
4. Visual Studio will auto-configure the project
5. Press **Ctrl+Shift+B** to build or use **Build Menu → Build All**
6. Run via **Debug → Start Debugging** or **Ctrl+F5**

---

## CLion (JetBrains)

1. Install CMake and a C++ compiler
2. Open CLion → **File → Open** and select the project folder
3. CLion auto-detects the `CMakeLists.txt`
4. Click **Build → Build Project** or press **Ctrl+F9**
5. Run via **Run → Run** or press **Shift+F10**

---

## VS Code (Cross-Platform)

1. Install the **CMake Tools** and **C/C++** extensions
2. Open the project folder
3. Select a kit when prompted (your compiler)
4. Run the **CMake: Configure** command
5. Run **CMake: Build** to compile
6. Press **Ctrl+F5** to run (or use the Run icon)

---

## Command Line (All Platforms)

```bash
cd build
cmake --build . --config Release
./3d_game  # Linux/Mac
3d_game.exe  # Windows
```

---

## Notes

- Raylib v5.5 will auto-download if not found (via CMake's FetchContent)
- Ensure you're in the `build` folder when building
- Release builds are faster than Debug for games