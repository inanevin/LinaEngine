/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: PAMRenderDevice
Timestamp: 4/27/2019 10:23:46 PM

*/

#pragma once

#ifndef PAMRenderDevice_HPP
#define PAMRenderDevice_HPP


#ifdef LINA_GRAPHICS_OPENGL

#include "PackageManager/OpenGL/GLRenderDevice.hpp"

typedef LinaEngine::Graphics::GLRenderDevice RenderDevice;

#endif


#endif