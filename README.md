# 3Sisters-Legacy-Game-Template
A game template for an easier time making games with the [3Sisters](https://github.com/Rihay66/3Sisters-Engine.git) Framework, Legacy version.

### What's this for?
This template is mostly to be utilized to get started on making a game utilizing the [3Sisters](https://github.com/Rihay66/3Sisters-Engine.git) Framework without dealing with headaches and figuring out how to construct the project which saves time.

### What's in this?
Inside this template include the following:
* Project folder structure
* CMake script 
* blank code for a blank application
* Additional game related utilities like Swept AABB Physics, Animation functions, and more!

### Adding more files to compile 
When adding more source files, look inside the ```CMakeLists.txt``` where it reads as follows:
```Cmake
# Include all source files
file(GLOB SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/src/main.cpp
    ${CMAKE_SOURCE_DIR}/src/window_app.cpp
    # Add more source files here
)
```

## Requirements
To be able to compile your projects you must have the following installed, due note it may differ between different OS.
* For Windows
    - Visual Studio 2022 msvc compiler
    - CMake   
[!CAUTION]
It is not recomended to use MSYS or mingw2
* For Linux
    - CMake
    - Ninja
    - Clang


## How to compile
For compilation, a build script is provided for both Windows and Linux. You must run 

For Linux: 
```
./build.sh
``` 
or For Windows:
```
./build.bat
```
**If you can use IDE CMake tools**

Traditionally, one would use an IDE with fancy interfaces so what is required is following:
* Ensure CMake outputs to a folder names ```build``` as this settings can take the form as a ```.json``` cmake settings file
* Ensure "Resource" folders are inside along with your executable application ```.exe``` or ```.out``` such as:
    - **Shaders**
    - **Textures**
    - **Sounds**
    - **Fonts**
