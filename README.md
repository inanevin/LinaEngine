![Lina](Docs/Images/LinaLogo.png?raw=true "Title")

## Lina Engine  
[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0) 
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/12c68c16c719427786597403aee43eb4)](https://app.codacy.com/app/inanevin/LinaEngine?utm_source=github.com&utm_medium=referral&utm_content=inanevin/LinaEngine&utm_campaign=Badge_Grade_Dashboard)
[![contributions welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg?style=flat)](https://github.com/inanevin/LinaEngine/issues) 
[![HitCount](http://hits.dwyl.io/inanevin/LinaEngine.svg)](http://hits.dwyl.io/inanevin/LinaEngine)

Lina Engine is an open-source game engine, that is dedicated to bring alternative and unique solutions to solve existing problems in the architecture of many game engines. It offers an architecture that has the purpose to create lightweight systems and focus on the robustness of core structure rather than stacking thousands of features for market competence. 

Lina Engine is a research project, and it is aimed to be the basis of development for more open-source game engine projects. It is our hope that the techniques and architecture used in this game engine can play the initiative role for more projects to develop open source game engines.

## Announcements

- As a build system, this project uses CMake 3.6. Please make sure you have a working version of CMake, prerably 3.6 or above installed on your machine.
- This project is currently supported on x64 platforms only.
- The external libraries, vendors used in the current version of Lina Engine is built with CMake as static libraries on Windows and included in the current version. Thus, this repository supports only Windows for now. However, it is possible to manually build the libraries (located under LinaEngine/vendor) as static library using any build system from their sources (links below) then linking them to LinaEngine by editing the CMakeLists file. The engine itself is not dependent on any platform specific code, so there shall not be any problems once libraries are dealt with. Soon vendors will be included with their build configurations.

## Vendor

-  [spdlog](https://github.com/gabime/spdlog)
-  [Glad](https://github.com/Dav1dde/glad)
-  [imgui](https://github.com/ocornut/imgui)
-  [stb](https://github.com/nothings/stb)

## Installation

-  [Git Clone Lina Engine](https://github.com/inanevin/LinaEngine) or download as ZIP.

#### Using terminal

-  Run command line or terminal in the repository directory.
-  Execute "cmake -G <options>" to generate project files for your desired IDE. See CMake options for defining an IDE through the console, or use CMake GUI to generate and build.
-  Make sure you select x64 version of your IDE for the project files, as Lina Engine's vendor libraries are compiled in x64 systems.
-  Run the project file located in the root directory, Lina.*
-  The sandbox executable application target is automatically set as the startup project in Visual Studio IDE if CMake 3.6 or above is used.
-  In any other case, do not forget to set sandbox target as the startup project from your IDE.
-  Build the BUILD_ALL project.
-  Now you can work on the sandbox project as well as the engine source code via your IDE.

## Run

-  Navigate to bin/Debug/ or bin/Release/ depending on your configurations.
-  Run Sandbox.exe

## Build Options

-  CMAKE_CONFIGURATION_TYPES: Project configuration types for your IDE.
-  CMAKE_INSTALL_PREFIX: Install directory used by cmake install.
-  LINA_BUILD_SANDBOX: Whether to build demo sandbox project, if unchecked, only engine shared library project will be generated.
-  LINA_ENABLE_LOGGING: Enables console/terminal debug output.


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
