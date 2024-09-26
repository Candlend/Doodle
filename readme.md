<p align="center">
  <img src="assets/splash.png" width="100%" alt="project-logo">

<h1>Doodle Engine (WIP)</h1>

<em>A lightweight and easy-to-use 3D game engine. </em>

[![License](https://img.shields.io/github/license/Candlend/Doodle?style=for-the-badge&logo=opensourceinitiative&logoColor=white)](https://github.com/Candlend/Doodle/blob/main/LICENSE)
[![Last Commit](https://img.shields.io/github/last-commit/Candlend/Doodle?style=for-the-badge&logo=git&logoColor=white)](https://github.com/Candlend/Doodle/commits/main)
[![Top Language](https://img.shields.io/github/languages/top/Candlend/Doodle?style=for-the-badge)](https://github.com/Candlend/Doodle)
[![Language Count](https://img.shields.io/github/languages/count/Candlend/Doodle?style=for-the-badge)](https://github.com/Candlend/Doodle)

<em>Developed with the software and tools below.</em>

![](https://img.shields.io/badge/C++-00599C.svg?style=for-the-badge&logo=cplusplus&logoColor=white)
![](https://img.shields.io/badge/OpenGL-5586A4.svg?style=for-the-badge&logo=opengl&logoColor=white)
![](https://img.shields.io/badge/Python-3776AB.svg?style=for-the-badge&logo=python&logoColor=white)

</p>

---

## 📍 Overview

Doodle Engine (WIP) is a lightweight 3D game engine crafted in C++ and OpenGL, designed to allow developers to create captivating 3D scenes and games as easily and freely as doodling. The engine is divided into two primary components:

- **Doodle Engine**: This core component manages rendering, input handling, resource management, and more, enabling developers to focus on creativity without getting bogged down by technical complexities.

- **Doodle Editor**: A user-friendly editor that facilitates scene and resource management, complete with preview and debugging capabilities. This tool empowers creators to visualize their ideas in real-time.

To enhance flexibility and ease of development, Python is employed as the scripting language, leveraging its robust ecosystem and simplicity.
Doodle Engine embodies the spirit of creativity and exploration, allowing developers to experiment freely and bring their imaginative concepts to life.

---

![Screenshot](assets/screenshot.png)

## 📁 Repository Structure

```plaintext
Doodle/
├── .clang-format
├── .clang-tidy
├── .clangd
├── .gitattributes
├── .gitignore
├── Doodle/
│   ├── pybind11/
│   │   ├── Bindings.cpp
│   │   ├── ...
│   ├── src/
│   │   ├── Application/
│   │   │   ├── Application.cpp
│   │   │   ├── ...
│   │   ├── ...
│   └── ...
├── DoodleEditor/
│   ├── src/
│   │   ├── main.cpp
│   │   ├── ...
│   └── ...
└── assets/
    ├── fonts/
    ├── icons/
    ├── models/
    ├── shaders/
    ├── textures/
    └── ...
```

---


## 🚀 Getting Started

### Requirements

Ensure you have the following dependencies installed:

- **C++ Compiler** (e.g., MSVC for Windows)
- **Xmake** (C++ build system and package manager)

### Installation

1. Clone the repository:

```sh
git clone https://github.com/Candlend/Doodle
cd Doodle
```

2. Install Xmake if you haven't already. You can find installation instructions [here](https://xmake.io/#/).

3. Build the project:

```sh
xmake
```

### Running the Engine

After building, you can run the engine with:

```sh
xmake run
```
---

## 🔍 Project Roadmap
- [X] Component-Based Architecture
- [X] Entity Conponent System
- [X] Implement basic rendering pipeline
- [X] OpenGL backend
- [X] ImGui-based Interface System
- [ ] Python scripting (WIP)
- [ ] Animation System
- [ ] Advanced rendering techniques
- [ ] Cross-platform support

---

## 📜 License

This project is licensed under the Apache License v2.0 - see the [LICENSE](LICENSE) file for details.

---

## 🏆 Acknowledgments

Special thanks to the following projects for their invaluable references and inspiration for Doodle Engine:

- [TheCherno/Hazel](https://github.com/TheCherno/Hazel)
- [openframeworks/openFrameworks](https://github.com/openframeworks/openFrameworks)
- [BoomingTech/Piccolo](https://github.com/BoomingTech/Piccolo)
- [cadenji/foolrenderer](https://github.com/cadenji/foolrenderer)

### 3rd Parties Libraries

- **fmt**: A fast and safe formatting library.
- **Assimp**: A library for importing various 3D model formats.
- **Boost**: A collection of libraries that provide support for tasks and structures such as smart pointers, threading, and file systems.
- **stb**: A collection of single-header libraries for image loading and other functionalities.
- **spdlog**: A fast C++ logging library.
- **nlohmann_json**: An easy-to-use C++ JSON library.
- **GLFW**: A library for creating windows and handling input.
- **Glad**: An OpenGL loader library that simplifies the use of OpenGL functions.
- **GLM**: A mathematics library designed for graphics programming.
- **EnTT**: A high-performance entity-component-system (ECS) library.
- **ImGui**: An immediate mode graphical user interface library.
- **ImNodes**: A library for creating node-based graphical interfaces.
- **ImGuizmo**: A tool for providing transformation controls within ImGui.
- **pybind11**: A library for binding C++ code to Python.
