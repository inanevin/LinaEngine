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

#include "LinaPch.hpp"
#include "PackageManager/OpenGL/Shader_GLSLBasic.hpp"  

namespace LinaEngine
{
	Shader_GLSLBasic::Shader_GLSLBasic() : Shader_GLSL()
	{

	}

	void Shader_GLSLBasic::Initialize()
	{
		Shader_GLSL::Initialize("Basic");

		// Add uniforms.
		//AddUniform("gWVP", "mat4");
		//AddUniform("gSampler", "sampler2D");
		AddUniform("texture1", "sampler2D");
		AddUniform("texture2", "sampler2D");
		//AddUniform("model", "mat4");
		//AddUniform("view", "mat4");
		//AddUniform("projection", "mat4");
		AddUniform("gWVP", "mat4");

	}


	void Shader_GLSLBasic::SetTextureUnit1()
	{
		SetUniform("texture1", 0);
	}

	void Shader_GLSLBasic::SetTextureUnit2()
	{
		SetUniform("texture2", 1);

	}

	void Shader_GLSLBasic::SetWVP(const Matrix4F& mat)
	{
		SetUniform("gWVP", mat);
	}
}
