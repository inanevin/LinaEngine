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

#include "Resources/ShaderResource.hpp"
#include "EventSystem/Events.hpp"
#include "EventSystem/EventSystem.hpp"
#include "Core/Log.hpp"
#include "Utility/FileUtility.hpp"
#include <fstream>
#include <cstdlib>
#include <shaderc/shaderc.hpp>

namespace Lina::Resources
{

	bool ShaderResource::LoadFromMemory(StringIDType sid, unsigned char* buffer, size_t bufferSize,  Event::EventSystem* eventSys)
	{
		
#ifdef LINA_GRAPHICS_VULKAN
		// Save data.
		std::vector<char> charbuf = std::vector<char>(buffer, buffer + bufferSize);
		uint32_t* intbuf = reinterpret_cast<uint32_t*>(charbuf.data());
		m_data = std::vector<uint32_t>(intbuf, intbuf + charbuf.size());
		LINA_TRACE("[Shader Loader] -> Shader loaded from memory.");
		return true;
#elif
#endif
	}

	bool ShaderResource::LoadFromFile(const std::string& path, ResourceType type, Event::EventSystem* eventSys)
	{	

#ifdef LINA_GRAPHICS_VULKAN
		
		if (CompileShader(path, type, false))
		{
			LINA_TRACE("[Shader Loader] -> Successfuly compiled shader from file: {0}", path);
			return true;
		}

		return false;
#elif
		std::ifstream file(path, std::ios::ate | std::ios::binary);

		if (!file.is_open())
		{
			LINA_ERR("[Shader Resource] -> Failed to open file: {0}", path);
			return false;
		}

		// Store resource data.
		size_t fileSize = (size_t)file.tellg();
		file.seekg(0);
		file.read(m_textData.data(), fileSize);
		return true;
#endif

		//// Send event.
		//std::string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		//Event::EShaderResourceLoaded ev;
		//ev.m_sid = StringID(path.c_str()).value();
		//ev.m_textData = contents.c_str();
		//ev.m_dataSize = fileSize;
		//eventSys->Trigger<Event::EShaderResourceLoaded>(ev);
		//LINA_TRACE("[Shader Loader] -> Audio loaded from file: {0}", path);
		//return true;
	}

	bool ShaderResource::CompileShader(const std::string& path, ResourceType type, bool saveToFile)
	{
		shaderc::Compiler compiler;
		shaderc::CompileOptions options;

		bool optimize = true;

		if (optimize) options.SetOptimizationLevel(shaderc_optimization_level_size);

		shaderc_shader_kind kind;

		if (type == ResourceType::GLSLFrag)
			kind = shaderc_glsl_fragment_shader;
		else if (type == ResourceType::GLSLVertex)
			kind = shaderc_glsl_vertex_shader;
		else if (type == ResourceType::GLSLGeo)
			kind = shaderc_glsl_geometry_shader;
		else
		{
			LINA_ERR("[Shader Loader] -> Could not determine the shader kind for compilation! {0}", path);
			return false;
	}

		std::string sourceName = FileUtility::RemoveExtensionFromFileName(FileUtility::GetFileName(path));
		std::ifstream stream(path);
		std::string shaderText((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
		shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(shaderText, kind, sourceName.c_str(), options);
		
		if (module.GetCompilationStatus() != shaderc_compilation_status_success)
		{
			LINA_ERR("[Shader Loader] -> Could not compile shader at path {0}, error msg: {1}", path, module.GetErrorMessage());
			return false;
		}
	
		m_data = { module.cbegin(), module.cend() };

		if (saveToFile)
		{
			//shaderc::AssemblyCompilationResult result = compiler.CompileGlslToSpvAssembly(shaderText, kind, sourceName.c_str(), options);

			std::ofstream fout(FileUtility::RemoveExtensionFromFileName(path) + ".spv", std::ios::out | std::ios::binary);
			fout.write((char*)&m_data[0], m_data.size() * sizeof(uint32_t));
			fout.close();
		}
		return true;
	}
}