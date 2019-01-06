/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

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
	class TypedData
	{
	public:
		TypedData(const std::string& name, const std::string& type) :
			m_Name(name),
			m_Type(type) {}

		inline const std::string& GetName() const { return m_Name; }
		inline const std::string& GetType() const { return m_Type; }
	private:
		std::string m_Name;
		std::string m_Type;
	};

	class UniformStruct
	{
	public:
		UniformStruct(const std::string& name, const std::vector<TypedData>& memberNames) :
			m_Name(name),
			m_MemberNames(memberNames) {}

		inline const std::string& GetName()                   const { return m_Name; }
		inline const std::vector<TypedData>& GetMemberNames() const { return m_MemberNames; }
	private:
		std::string            m_Name;
		std::vector<TypedData> m_MemberNames;
	};

	class Shader_GLSL
	{
	public:

		Shader_GLSL();
		~Shader_GLSL();
		void AddShader(std::string pShaderText, unsigned int ShaderType);
		void CompileShaders();
		void Bind();

		void AddUniform(const std::string& uniformName, const std::string& uniformType);

		void SetUniform(const std::string& uniformName, int value) const;
		void SetUniform(const std::string& uniformName, float value) const;
		void SetUniform(const std::string& uniformName, const Vector3F& value) const;
		void SetUniform(const std::string& uniformName, const Matrix4F& value) const;


		static std::string LoadShader(std::string path);

	private:

		
		void CheckError(unsigned int, int, std::string);

		unsigned int m_Program;
		std::vector<int> m_Shaders;
		std::map<std::string, unsigned int> m_UniformMap;
	};
}


#endif