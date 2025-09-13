# LearningOpenGL
A repository to better understand and explain OpenGL.
This project uses **GLFW** for windowing and **GLAD** for OpenGL function loading.

## Branches and Tags
Branches are used to demonstrate **specific OpenGL concepts** as major steps (0.1, 0.2).   
Tags mark **smaller improvements** or **refactors** inside those branches (0.1.1, 0.1.2).   

- [Branch 0.0](https://github.com/Masonlet/LearningOpenGL/blob/Branch_0.0/CMakeLists.txt) — Demonstrates how to set up and build a minimal OpenGL project using **CMake**.
- [Branch 0.1](https://github.com/Masonlet/LearningOpenGL/blob/Branch_0.1/src/main.cpp) — Bare minimum GLFW + GLAD window
  - [v0.1.0](https://github.com/Masonlet/LearningOpenGL/blob/v0.1.0/src/main.cpp) — Initial OpenGL initializing with explanatory comments
  - [v0.1.1](https://github.com/Masonlet/LearningOpenGL/blob/v0.1.1/src/main.cpp) — Separates OpenGL initialization into functions
- [Branch 0.2](https://github.com/Masonlet/LearningOpenGL/) — *W.I.P*
- [Branch 0.3](https://github.com/Masonlet/LearningOpenGL/) — *W.I.P*
- [Branch 0.4](https://github.com/Masonlet/LearningOpenGL/) — *W.I.P*
- [Branch 0.5](https://github.com/Masonlet/LearningOpenGL/) — *W.I.P*
- [Branch 0.6](https://github.com/Masonlet/LearningOpenGL/) — *W.I.P*
- [Branch 0.7](https://github.com/Masonlet/LearningOpenGL/) — *W.I.P*
- [Branch 0.8](https://github.com/Masonlet/LearningOpenGL/) — *W.I.P*
- [Branch 0.9](https://github.com/Masonlet/LearningOpenGL/) — *W.I.P*
- [Branch 1.0](https://github.com/Masonlet/LearningOpenGL/) — Triangle *W.I.P*

## Building the Project
This project uses **CMake**. Follow these steps to build:

### 1. Clone the Repository
```bash
git clone https://github.com/Masonlet/LearningOpenGL.git
cd LearningOpenGL
```

### 2. Create a Build Directory and Generate Build Files
```bash
mkdir build
cd build 
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
```
`-DCMAKE_EXPORT_COMPILE_COMMANDS=ON` flag generates a `compile_commands.json` file  
Can be safely omitted on Windows if you're using Visual Studio

### 3. Build the Project
- **Linux**:
  ```bash
  make
  ```

- **Windows**:
  ```bash
  cmake --build .
  ```
  Or open the generated `.sln` file in Visual Studio and build the solution.
