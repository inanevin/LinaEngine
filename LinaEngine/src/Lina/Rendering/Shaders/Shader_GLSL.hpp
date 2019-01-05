/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Ýnan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: Shader_GLSL
Timestamp: 1/5/2019 12:53:08 AM

*/

#pragma once
#ifndef Shader_GLSL_HPP
#define Shader_GLSL_HPP



namespace LinaEngine
{
	class Shader_GLSL
	{
	public:

		Shader_GLSL();
		~Shader_GLSL();
		void AddShader(std::string pShaderText, unsigned int ShaderType);
		void CompileShaders();
		void Bind();
		static std::string LoadShader(std::string path);

	private:

		void CheckError(unsigned int, int, std::string);

		unsigned int m_Program;
		std::vector<int> m_Shaders;

	};
}


#endif