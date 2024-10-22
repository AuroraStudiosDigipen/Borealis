# Borealis Game Engine

## Overview

Borealis Game Engine is a game development framework written in C++. The engine is built using Microsoft Visual Studio (MSVC) and managed using Premake for easy project configuration.

## Features

- **C# Scripting**: Create game logic using C#, allowing for flexible and dynamic gameplay programming.
- **Project Management**: Tools to organize and manage game projects efficiently.
- **In-built AI Systems**: Predefined systems to handle common AI behaviors, making it easier to implement intelligent characters.
- **Profilers**: Tools to analyze and optimize performance, helping developers identify bottlenecks in their code.
- **Physics Engine**: Integrated physics support for realistic simulations and interactions in your game environment.

## Getting Started

### Prerequisites

To build and run the engine, you'll need:

- [Microsoft Visual Studio](https://visualstudio.microsoft.com/) (2022 or later)
- C++20 compatible compiler
- .NET Framework 4.7.2

## Build Configurations

This project supports three build configurations: **Debug**, **Release**, and **Distribution**. Each configuration serves a different purpose during development and deployment.

### 1. Debug Configuration

- **Purpose**: Intended for development and debugging.
- **Features**:
  - Includes debugging symbols for tracing through code.
  - Optimizations are turned off for easier debugging.
  - Additional checks (e.g., assertions) are enabled.
- **Usage**: Use this configuration while developing new features or fixing bugs. It allows for stepping through code in a debugger and inspecting variables and states at runtime.

### 2. Release Configuration

- **Purpose**: Optimized for performance, intended for final builds.
- **Features**:
  - Compiler optimizations are enabled for faster execution and reduced memory usage.
  - Debugging symbols may be stripped or reduced, resulting in smaller binaries.
  - Assertions and other checks may be disabled to improve performance.
- **Usage**: Use this configuration when you are ready to test the engine or final game build. This is the version you will distribute to users for a production environment.

### 3. Distribution Configuration

- **Purpose**: Specially tailored for packaging and distribution.
- **Features**:
  - Similar optimizations to Release, but may include additional settings for compatibility.
  - Ensures that the final product meets distribution requirements (e.g., specific compiler flags).
- **Usage**: Use this configuration when preparing the final version of your game or engine for deployment. It’s particularly useful for creating installers or packages for end users.

## Project Structure

This project consists of several components, each serving a distinct purpose in the development of games using the Borealis game engine. Below is a brief overview of each project:

### Projects

- **Borealis**: 
  - A static library that serves as the core of the game engine, providing essential functionalities and driving the program.

- **BorealisEditor**: 
  - A Windows application that functions as a level editor, allowing developers to create and manage game levels using the features provided by the game engine.

- **BorealisRuntime**: 
  - A Windows program that generates a runtime executable. This executable is used by the level editor to build the actual software for the game.

- **BorealisAssetCompiler**: 
  - An executable that compiles and processes assets. It is invoked by the editor when importing various assets into the game engine.

- **BorealisScriptCore**: 
  - A C# project that provides a scripting framework interface, allowing developers to write scripts that interact with the game engine.

## External Libraries

This project utilizes several external libraries to enhance its functionality and provide various features. Below is a list of the external libraries used in the Borealis game engine:

- **EnTT**: A fast and reliable entity-component system (ECS) library for managing game entities and their components.
- **FMOD**: A powerful audio library used for sound and music playback in games.
- **GLAD**: An OpenGL function loader that simplifies the process of using OpenGL in your projects.
- **GLFW**: A library for creating windows and handling input in OpenGL applications.
- **gli**: A header-only library for handling OpenGL texture formats.
- **glm**: A mathematics library for graphics software, providing support for vectors and matrices.
- **Imgui**: A bloat-free graphical user interface library for C++.
- **ImGuizmo**: A simple manipulation widget for ImGui, useful for 3D transformations.
- **JoltPhysics**: A physics engine for real-time simulations.
- **mono**: A cross-platform, open-source implementation of the .NET framework, used for scripting support.
- **RTTR**: A library for runtime type reflection in C++.
- **spdlog**: A very fast C++ logging library.
- **stb_image**: A single-file library for loading images in various formats.
- **tracy**: A real-time, multi-threaded CPU profiler.
- **yaml-cpp**: A YAML parser and emitter for C++.
- **assimp**: A library for importing and processing 3D model formats.
- **imgui-node-editor**: An extension for ImGui that provides a node-based editor interface.
- **ispc**: A compiler for a variant of C that allows for high-performance parallel processing.
- **msdf**: A library for generating signed distance fields for fonts.

## Building the Project

To build the project using the specified configurations, follow these steps:

1. Run the `generateProject.bat` file to generate the Visual Studio solution.
2. Open the generated solution in Microsoft Visual Studio.
3. Select your desired build configuration from the dropdown menu in the toolbar (Debug, Release, or Distribution).
4. Build the solution by selecting `Build` -> `Build Solution` or by pressing `Ctrl + Shift + B`.


