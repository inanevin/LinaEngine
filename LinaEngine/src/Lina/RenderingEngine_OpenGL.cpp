/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Ýnan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: RenderingEngine_OpenGL
Timestamp: 1/2/2019 11:44:41 PM

*/
#include "LinaPch.hpp"
#include "RenderingEngine_OpenGL.hpp"  
#include "glew.h"


namespace LinaEngine
{
	RenderingEngine_OpenGL::RenderingEngine_OpenGL() : RenderingEngine()
	{
		// Initialize GLEW
		GLenum res = glewInit();
		LINA_CORE_ASSERT(res != GLEW_OK, "Glew is not initialized properly");

	}

	RenderingEngine_OpenGL::~RenderingEngine_OpenGL()
	{

	}

	void RenderingEngine_OpenGL::OnUpdate()
	{

		
		RenderingEngine::OnUpdate();
	}
}

