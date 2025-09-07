# LearningOpenGL
A repository to better understand and explain OpenGL.
This project uses **GLFW** for windowing and **GLAD** for OpenGL function loading.

## Branches
Branches are used to demonstrate specific OpenGL concepts, building step by step from the basics to more advanced topics.

- **Branch 0.0** - Demonstrates how to set up and build a minimal OpenGL project using **CMake**.
- **Branch 0.1** - Demonstrates the bare minimum for a **GLFW** and **GLAD** window.
- **Branch 0.2** - *W.I.P*
- **Branch 0.3** - *W.I.P*
- **Branch 0.4** - *W.I.P*
- **Branch 0.5** - *W.I.P*
- **Branch 0.6** - *W.I.P*
- **Branch 0.7** - *W.I.P*
- **Branch 0.8** - *W.I.P*
- **Branch 0.9** - *W.I.P*
- **Branch 1.0** - Triangle *W.I.P*

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
