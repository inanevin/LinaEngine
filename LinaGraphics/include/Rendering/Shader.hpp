/* 
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/*
Class: Shader

Wrapper for shader functionalities, responsible for creating shaders, binding uniform buffers
and all related shader operations.

Timestamp: 2/16/2019 1:47:28 AM
*/

#pragma once

#ifndef Shader_HPP
#define Shader_HPP

#include "Core/Common.hpp"
#include "UniformBuffer.hpp"
#include <string>

namespace Lina::Graphics
{

	class Shader
	{
	public:

		Shader() {};

		~Shader();

		Shader& Construct(const std::string& text, bool usesGeometryShader);
		void SetUniformBuffer(const std::string& name, UniformBuffer& buffer);
		void BindBlockToBuffer(uint32 bindingPoint, std::string blockName);

		static Shader& CreateShader(const std::string& path, bool usesGeometryShader = false);
		static Shader& GetShader(const std::string& path);
		static Shader& GetShader(StringIDType id);
		static bool ShaderExists(const std::string& path);
		static bool ShaderExists(StringIDType id);
		static void UnloadAll();
		static std::map<StringIDType, Shader*>& GetLoadedShaders() { return s_loadedShaders; }


		ShaderUniformData& GetUniformData() { return m_uniformData; }
		StringIDType GetSID() { return m_sid; }
		uint32 GetID() { return m_engineBoundID; }
		const std::string& GetPath() { return m_path; }


	private:

		ShaderUniformData m_uniformData;
		RenderDevice* m_renderDevice = nullptr;
		uint32 m_engineBoundID = 0;
		StringIDType m_sid;
		std::string m_path = "";
		static std::map<StringIDType, Shader*> s_loadedShaders;

	
	};
}


#endif