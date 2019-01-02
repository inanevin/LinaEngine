![Lina](Docs/Images/LinaLogo.png?raw=true "Title")

## Lina Engine  
[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0) 
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/12c68c16c719427786597403aee43eb4)](https://app.codacy.com/app/inanevin/LinaEngine?utm_source=github.com&utm_medium=referral&utm_content=inanevin/LinaEngine&utm_campaign=Badge_Grade_Dashboard)
[![contributions welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg?style=flat)](https://github.com/inanevin/LinaEngine/issues) 
[![HitCount](http://hits.dwyl.io/inanevin/LinaEngine.svg)](http://hits.dwyl.io/inanevin/LinaEngine)

Lina Engine is an open-source game engine, that is dedicated to bring alternative and unique solutions to solve existing problems in the architecture of many game engines. It offers an architecture that has the purpose to create lightweight systems and focus on the robustness of core structure rather than stacking thousands of features for market competence. 

Lina Engine is a research project, and it is aimed to be the basis of development for more open-source game engine projects. It is our hope that the techniques and architecture used in this game engine can play the initiative role for more projects to develop open source game engines.

## Announcements

-  Lina Engine is currently being carried over to a new version (30.12.2018). There exists _LinaEngine_ and _Sandbox_ folders which is consists of the structure for sandbox application and new engine version. The old project files, that include examples for input, rendering, event and ECS systems can be found under the folder __VERSION0_SRC__. There is no build generation support for the old version. 
-  The dependency installations for used libraries in Lina Engine are not automated yet. This repository includes the dependency packages as they are, for Windows builds. That is why, the current version of Lina Engine is for Windows only. The dependency installation will be transferred to CMake soon and the libraries will be removed from the repository. It is still possible to build the engine in Linux and MacOS, as it requires manual edit on premake file for dynamic linking as well as downloading the relative binaries and putting them into the include directories.

## Dependencies
-  [spdlog](https://github.com/gabime/spdlog)
-  [SDL2](https://www.libsdl.org/)
-  [OpenGL Mathematics(GLM)](https://glm.g-truc.net/0.9.9/index.html)
-  [GLEW](http://glew.sourceforge.net/)

## Installation

-  [Git Clone Lina Engine](https://github.com/inanevin/LinaEngine)
-  This project uses Premake5, .bat files for various environments are generated & inside the root directory.
-  After you installed all the dependencies as instructed, run one of the .bat files to generate projects for Visual Studio, GNU Make, XCode or CodeLite.
-  Open your project files in your IDE, build the projects. 
-  Set the sandbox project as the startup project.
-  Build.
-  Now you can work on the sandbox project as well as the engine source code via your IDE. 

## Run

-  Navigate to bin/.../Sandbox/
-  Run Sandbox.exe

## License

[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)

## Notes
-  OpenGL version guards have not been implemented yet, so if you are recieving memory crashes due to OpenGL buffers, please do update your graphics card to the latest version. Especially if you are running Intel HD chipsets.

## Design Diagrams

### System Architecture
![Diagram 1](Docs/Images/SCS.png?raw=true "Title")

### Design Document

[Link to the Software Design Document of Lina Engine](https://docs.google.com/document/d/13Z1D77WbLJkyq0Q2Q9DdKKSM4tfqiZHQi_j_x0Rs5Ec/edit?usp=sharing)
