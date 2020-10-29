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
#include "Rendering/RenderEngine.hpp"
#include "Utility/UtilityFunctions.hpp"

namespace LinaEngine::Graphics
{
	std::map<int, Shader> Shader::s_loadedShaders;

	Shader& Shader::CreateShader(Shaders shader, const std::string& path, bool usesGeometryShader)
	{
		// Create shader
		if (!ShaderExists(shader))
		{
			std::string shaderText;
			Utility::LoadTextFileWithIncludes(shaderText, path, "#include");
			return s_loadedShaders[shader].Construct(RenderEngine::GetRenderDevice(), shaderText, usesGeometryShader);
		}
		else
		{
			// Shader with this name already exists!
			LINA_CORE_WARN("Shader with the id {0} already exists, returning that...", shader);
			return s_loadedShaders[shader];
		}
	}

	Shader& Shader::GetShader(Shaders shader)
	{
		if (!ShaderExists(shader))
		{
			// Shader not found.
			LINA_CORE_WARN("Shader with the ID {0} was not found, returning standardUnlit Shader", shader);
			return GetShader(Shaders::Standard_Unlit);
		}

		return s_loadedShaders[shader];
	}

	bool Shader::ShaderExists(Shaders shader)
	{
		return !(s_loadedShaders.find(shader) == s_loadedShaders.end());
	}
}