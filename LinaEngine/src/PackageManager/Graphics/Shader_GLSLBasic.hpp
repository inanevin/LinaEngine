/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: Shader_GLSLBasic
Timestamp: 2/19/2019 10:53:02 AM

*/

#pragma once

#ifndef Shader_GLSLBasic_HPP
#define Shader_GLSLBasic_HPP

#include "Shader_GLSL.hpp"


namespace LinaEngine
{
	class Shader_GLSLBasic : public Shader_GLSL
	{

	public:

		Shader_GLSLBasic();
		void Initialize();
		void SetWVP(const Matrix4F& wvp);
		void SetTextureUnit1();
		void SetTextureUnit2();
		void SetModel(const glm::mat4&);
		void SetView(const glm::mat4&);
		void SetProjection(const glm::mat4&);

	};
}


#endif