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

#pragma once
#ifndef RenderingEngine_OpenGL_HPP
#define RenderingEngine_OpenGL_HPP

#include "Lina/Rendering/RenderingEngine.hpp"
#include "glew.h"
#include "Lina/Camera.hpp"


namespace LinaEngine
{
	
	class Shader_GLSL;

	class LINA_API RenderingEngine_OpenGL : public RenderingEngine
	{
	public:

		RenderingEngine_OpenGL();
		~RenderingEngine_OpenGL();

		void OnUpdate() override;
		void OnWindowEvent(Event& e) override;
		void CreateVertexBuffer();
		void CreateIndexBuffer();

	private:
		Camera cam;
		Shader_GLSL* test;
		GLuint m_VAO;
		GLuint m_VAB;
		int drawCount;
	};
}


#endif