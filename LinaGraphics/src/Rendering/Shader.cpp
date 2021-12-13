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

#include "Rendering/Shader.hpp"
#include "Core/RenderEngineBackend.hpp"
#include "Utility/UtilityFunctions.hpp"

namespace Lina::Graphics
{
	std::map<int, Shader*> Shader::s_loadedShaders;

	Shader::~Shader()
	{
	m_engineBoundID = m_renderDevice->ReleaseShaderProgram(m_engineBoundID);
	}

	Shader& Shader::Construct(const std::string& text, bool usesGeometryShader)
	{
		m_renderDevice = RenderEngineBackend::Get()->GetRenderDevice();
		m_engineBoundID = m_renderDevice->CreateShaderProgram(text, &m_uniformData, usesGeometryShader);
		return *this;
	}

	void Shader::SetUniformBuffer(const std::string& name, UniformBuffer& buffer)
	{
		m_renderDevice->SetShaderUniformBuffer(m_engineBoundID, name, buffer.GetID());
	}

	void Shader::BindBlockToBuffer(uint32 bindingPoint, std::string blockName)
	{
		m_renderDevice->BindShaderBlockToBufferPoint(m_engineBoundID, bindingPoint, blockName);
	}

	Shader& Shader::CreateShader(const std::string& path, bool usesGeometryShader)
	{
		std::string shaderText;
		Utility::LoadTextFileWithIncludes(shaderText, path, "#include");
		Shader* shader = new Shader();
		shader->Construct(shaderText, usesGeometryShader);
		shader->m_path = path;
		s_loadedShaders[shader->GetID()] = shader;
		return *shader;
	}

	Shader& Shader::GetShader(const std::string& path)
	{
		const auto it = std::find_if(s_loadedShaders.begin(), s_loadedShaders.end(), [path]
		(const auto& item) -> bool { return item.second->GetPath().compare(path) == 0; });

		if (it == s_loadedShaders.end())
		{
			// Mesh not found.
			LINA_WARN("Shader with the path {0} was not found, returning default shader", path);
			return RenderEngineBackend::GetDefaultShader();
		}

		return *it->second;
	}

	Shader& Shader::GetShader(int id)
	{
		return *s_loadedShaders[id];
	}

	bool Shader::ShaderExists(const std::string& path)
	{
		const auto it = std::find_if(s_loadedShaders.begin(), s_loadedShaders.end(), [path]
		(const auto& it) -> bool { 	return it.second->GetPath().compare(path) == 0; 	});
		return it != s_loadedShaders.end();
	}

	bool Shader::ShaderExists(int id)
	{
		if (id < 0) return false;
		return !(s_loadedShaders.find(id) == s_loadedShaders.end());
	}

	void Shader::UnloadAll()
	{
		// Delete textures.
		for (std::map<int, Shader*>::iterator it = s_loadedShaders.begin(); it != s_loadedShaders.end(); it++)
			delete it->second;

		s_loadedShaders.clear();
	}

}