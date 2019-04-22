![Lina](Docs/Images/linalogofull_medium.png)

## Lina Engine  
[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0) 
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/12c68c16c719427786597403aee43eb4)](https://app.codacy.com/app/inanevin/LinaEngine?utm_source=github.com&utm_medium=referral&utm_content=inanevin/LinaEngine&utm_campaign=Badge_Grade_Dashboard)
[![contributions welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg?style=flat)](https://github.com/inanevin/LinaEngine/issues) 
[![HitCount](http://hits.dwyl.io/inanevin/LinaEngine.svg)](http://hits.dwyl.io/inanevin/LinaEngine)

Lina Engine is an open-source game engine, that is dedicated to bring alternative and unique solutions to solve existing problems in the architecture of many game engines. It offers an architecture that has the purpose to create lightweight systems and focus on the robustness of core structure rather than stacking thousands of features for market competence. 

Lina Engine is a research project, and it is aimed to be the basis of development for more open-source game engine projects. It is our hope that the techniques and architecture used in this game engine can play the initiative role for more projects to develop open source game engines.

## Announcements / Notes

- As a build system, this project uses CMake 3.6. Please make sure you have a working version of CMake, prerably 3.6 or above installed on your machine.
- This repository currently uses git submodules for dependencies and builds them along with the engine.

## Lina Engine Tools

- [Lina Engine Build Launcher](https://github.com/inanevin/Lina-Engine-Build-Launcher)

## External Dependencies

-  [Glad](https://github.com/Dav1dde/glad)
-  [GLFW](https://github.com/glfw/glfw)

## Libraries Included in Source Code

-  [spdlog](https://github.com/gabime/spdlog)
-  [imgui](https://github.com/ocornut/imgui)
-  [stb](https://github.com/nothings/stb)

## Installation

-  Clone Lina Engine git repository

```shell
# Initialize git in your desired directory.
git init

# Clone Lina Engine
git clone https://github.com/inanevin/LinaEngine

# Alternatively you can clone recursively, this will download all the submodules if there exists any along with Lina Engine source code.
git clone --recursive https://github.com/inanevin/LinaEngine

```
-  You can generate the project files and build using; Lina Engine Build Launcher, CMake with Shell, CMake GUI

#### Using Lina Engine Build Launcher

-  Download a binary release from [Lina Engine Build Launcher Repository](https://github.com/inanevin/Lina-Engine-Build-Launcher).
-  Run the .jar file, launcher will open.

[!](Docs/Images/buildlauncher.png)

-  Select the directory where you downloaded Lina Engine's source. (this repository)
-  Select your desired build directory for project files.
-  Select your target generator & desired build options.
-  Hit "Generate Project Files" to generate only the project files, hit "Generate and Build" to generate project files and then build the binaries using the generated files.


#### Generate project files using CMake with shell.

-  Run your shell, command line or terminal in the repository directory. Afterwards, execute generation / build commands.

```shell
# You can define any options before generating project files.
cmake -DLINA_ENABLE_LOGGING=OFF

# You can define multiple options sequentially.
cmake -DLINA_ENABLE_LOGGING=OFF -DLINA_BUILD_SANDBOX=OFF

# It is recommended to declare the type of the option if multiple options are defined
cmake -DLINA_ENABLE_LOGGING:BOOL=OFF -DLINA_BUILD_SANDBOX:BOOL=OFF -DCMAKE_CONFIGURATION_TYPES:STRING="Debug,Release"

# Above commands will generate project files with default generator, you can specify a generator if you want.
cmake -DLINA_ENABLE_LOGGING=OFF -G "Visual Studio 15 2017"

```

-  After generating project files you can either open your IDE and build the ALL_BUILD project which will build all the targets or you can build the binaries from shell.

```shell
# Create a directory for binaries
mkdir bin

# Navigate to directory
cd bin

# Build the root project
cmake --build ../

```
-  Check out [CMake Documentation](https://cmake.org/cmake/help/v3.2/manual/cmake.1.html) for specifying more options and configurations like project file generation directories and more.

#### Using CMake GUI

-  Choose the directory where the repository is as the source directory.
-  Choose a build directory, this can be the same as the source directory or anywhere you like.
-  Hit configure, select the IDE you want to generate the project files for.
-  Select options, you can leave them as default.
-  Hit generate, this will generate the project files for your desired IDE.

#### After project file generation

-  After generating files, as mentioned above, you can either build from shell or from your IDE.
-  If you choose to build from your IDE, run the project file generated in the root directory
-  Run the project file located in the root directory (or in the root of your output project file directory if you have specified one), Lina.*
-  The sandbox executable application target is automatically set as the startup project only in Visual Studio IDE if CMake 3.6 or above is used, and if you left the LINA_BUILD_SANDBOX option checked.
-  In any case, do not forget to check whether the sandbox target is the startup project in your IDE if you have generated it.
-  Build the BUILD_ALL project.
-  Now you can work on the sandbox project as well as the engine source code via your IDE.

## Run
-  You can run through any attached debugger on your IDE.
-  Alternatively, you can navigate to bin/Debug/ or bin/Release/ (default directories specified in CMakeLists, if you have have built your binaries from shell and overriden output directory navigate there) depending on your configurations and run the binary outputs.

## Build Options

| Option  | Description | Default |
| ------------- | ------------- | ------------- |
| BUILD_SHARED_LIBS  | Whether to build all dependency libraries as shared dynamic libraries instead of static if not explicitly stated.  | OFF  |
| CMAKE_CONFIGURATION_TYPES  | Project configuration types for your IDE.  | Debug, Release, MinSizeRel, RelWithDebInfo  |
| CMAKE_INSTALL_PREFIX  | Default installd directory for CMake install commands.  | depends on platform |
| LINA_BUILD_SANDBOX  | Whether to build example executable sandbox application. | ON  |
| LINA_ENABLE_LOGGING  | Whether to enable sink / terminal logging during runtime.  | ON |
| OTHERS | There also exists a set of build options for GLAD and GLFW if OpenGL is used as graphics device. (currently by default it is) See the relative pages for information about those options in "Dependencies" above.  |  |

## License

[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)

## Notes

-  OpenGL version guards have not been implemented yet, so if you are recieving memory crashes due to OpenGL buffers, please do update your graphics card to the latest version. Especially if you are running Intel HD chipsets.

## Design Diagrams

### System Architecture
![Diagram 1](Docs/Images/SCS.png?raw=true "Title")

### Design Document

[Link to the Software Design Document of Lina Engine (PDF)](https://docs.wixstatic.com/ugd/816f17_eef322d102a94845859da33004254fff.pdf)

[Literature Review done for the design of Lina Engine (GDOCS)](https://docs.google.com/document/d/1Pu_fJU6x_tfXkGBrjXe9A6gk-s-qliDu00Ic1RWWDmk/edit?usp=sharing)
