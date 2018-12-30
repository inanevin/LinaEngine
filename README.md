## Announcements

Lina Engine is currently being carried over to a new version (30.12.2018). There exists _LinaEngine_ and _Sandbox_ folders which is consists of the structure for sandbox application and new engine version. The old project files, that include examples for input, rendering, event and ECS systems can be found under the folder __VERSION0_SRC__. There is no build generation for the old version. 

## Lina Engine ![Size](https://github-size-badge.herokuapp.com/inanevin/LinaEngine.svg) [![Codacy Badge](https://api.codacy.com/project/badge/Grade/12c68c16c719427786597403aee43eb4)](https://app.codacy.com/app/inanevin/LinaEngine?utm_source=github.com&utm_medium=referral&utm_content=inanevin/LinaEngine&utm_campaign=Badge_Grade_Dashboard) [![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)



Lina Engine is an open-source game engine, that is dedicated to bring alternative and unique solutions to solve existing problems in the architecture of many game engines. It offers an architecture that has the purpose to create lightweight systems and focus on the robustness of core structure rather than stacking thousands of features for market competence. 

Lina Engine is a research project, and it is aimed to be the basis of development for more open-source game engine projects. It is our hope that the techniques and architecture used in this game engine can play the initiative role for more projects to develop open source game engines.

## Dependencies
- [spdlog](https://github.com/gabime/spdlog)
- SDL 2.0
- OpenGL 4.6
- GLEW 2.1.0
- OpenAL
- GLM
- stb

## Installation

- git clone https://github.com/inanevin/LinaEngine
- This project uses Premake5, .bat files for various environments are generated & inside the root directory.
- Run one of the .bat files to generate projects for Visual Studio, GNU Make, XCode or CodeLite.
- Open your project files in your IDE, build the projects. 
- After first build, you might receive path error, if so just build again, this will disappear 
in the following builds.
- Now you can work on the sandbox project as well as the engine source code via your IDE. 

## Run

- Navigate to bin/.../Sandbox/
- Run Sandbox.exe

## License

[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)

## Notes
- OpenGL version guards have not been implemented yet, so if you are recieving memory crashes due to OpenGL buffers, please do update your graphics card to the latest version. Especially if you are running Intel HD chipsets.

## Design Diagrams

#### System Architecture
![Diagram 1](Docs/Images/SystemArchitecture.png?raw=true "Title")

## Design Document

[Link to the Software Design Document of Lina Engine](https://docs.google.com/document/d/13Z1D77WbLJkyq0Q2Q9DdKKSM4tfqiZHQi_j_x0Rs5Ec/edit?usp=sharing)
