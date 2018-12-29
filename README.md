[Lina Engine](https://github.com/inanevin/LinaEngine#announcements)

## Announcements

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/12c68c16c719427786597403aee43eb4)](https://app.codacy.com/app/inanevin/LinaEngine?utm_source=github.com&utm_medium=referral&utm_content=inanevin/LinaEngine&utm_campaign=Badge_Grade_Dashboard)

Lina Engine is currently being carried over to a new version (30.12.2018). There exists _LinaEngine_ and _Sandbox_ folders which is consists of the structure for sandbox application and DLL linkage setup. The old project files, that include examples for input, rendering, event and ECS systems can be found under the folder __VERSION0_SRC_.  

## Lina Engine

Lina Engine is an open-source game engine, that is dedicated to bring alternative and unique solutions to solve existing problems in the architecture of many game engines. It offers an architecture that has the purpose to create lightweight systems and focus on the robustness of core structure rather than stacking thousands of features for market competence. 

Lina Engine is a research project, and it is aimed to be the basis of development for more open-source game engine projects. It is our hope that the techniques and architecture used in this game engine can play the initiative role for more projects to develop open source game engines.

## Important

- The new version under development works on only Windows platform for now. The build process requires the given mention of VS in the _Prequisites_. Later on CMake integration will be added for abstract build support.

## Prequisites
- Visual Studio 2017 15.8.5 or newer
- Microsoft .NET Framework 4.7.02558
- Visual C++ 2017

## Dependencies
- SDL 2.0
- OpenGL 4.6
- GLEW 2.1.0
- OpenAL
- GLM
- stb

## Installation
- git clone https://github.com/inanevin/LinaEngine
- Open LinaEngine.sln in the VS version mentioned in _Prequisites_.
- Build LinaEngine project, then build Sandbox project.

## Run
- Copy bin/Debug-x64/LinaEngine/LinaEngine.dll into bin/Sandbox/ directory.
- Run Sandbox.exe

## License
Licensed under the Apache License, Version 2.0 See [LICENSE](https://github.com/lineupthesky/LinaEngine/blob/master/LICENSE.md) for details.

## Notes
- OpenGL version guards have not been implemented yet, so if you are recieving memory crashes due to OpenGL buffers, please do update your graphics card to the latest version. Especially if you are running Intel HD chipsets.

## Design Diagrams

#### System Architecture
![Diagram 1](Docs/Images/SystemArchitecture.png?raw=true "Title")

## Design Document

[Link to the Software Design Document of Lina Engine](https://docs.google.com/document/d/13Z1D77WbLJkyq0Q2Q9DdKKSM4tfqiZHQi_j_x0Rs5Ec/edit?usp=sharing)
