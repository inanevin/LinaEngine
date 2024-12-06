/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-] [Inan Evin]

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

#include "Core/Graphics/Utility/ShaderPreprocessor.hpp"
#include "Common/FileSystem//FileSystem.hpp"
#include "Common/Data/HashMap.hpp"

namespace Lina
{

#define HEADER_END "//lina_##_header_##_end\n"

	namespace
	{
		String ExtractBlock(const String& source, const String& startDelimiter, const String& endDelimiter)
		{
			size_t startPos = source.find(startDelimiter);
			if (startPos == String::npos)
			{
				return ""; // Start delimiter not found
			}

			startPos += startDelimiter.length(); // Move past the delimiter
			size_t endPos = source.find(endDelimiter, startPos);
			if (endPos == String::npos)
			{
				return ""; // End delimiter not found
			}

			return source.substr(startPos, endPos - startPos);
		}

		bool ProcessMaterialData(String& block, Vector<ShaderPropertyDefinition>& outProperties)
		{
			const String materialIdent = "struct LinaMaterial";

			if (block.find(materialIdent) == String::npos)
				return true;

			std::istringstream f(block.c_str());
			std::string		   line = "";

			bool parsingMaterialStruct = false;

			auto property = [](const String& line, const String& propType, String& outName) -> bool {
				const String str   = propType;
				const size_t found = line.find(str);
				if (found != String::npos)
				{
					const String trimmedLine = FileSystem::RemoveWhitespaces(line);
					outName					 = trimmedLine.substr(propType.length(), trimmedLine.find(";") - propType.length());
					return true;
				}

				return false;
			};

			size_t materialBlockEnd = 0;
			bool   commentBlock		= false;

			auto fetchValuesFromString = [](String& str) -> Vector<float> {
				Vector<float> vals;

				if (str.empty())
					return vals;

				const size_t pBegin = str.find("(");

				if (pBegin != String::npos)
					str = str.substr(pBegin, str.length() - pBegin);

				str.erase(linatl::remove(str.begin(), str.end(), ';'), str.end());
				str.erase(linatl::remove(str.begin(), str.end(), '('), str.end());
				str.erase(linatl::remove(str.begin(), str.end(), ')'), str.end());
				Vector<String> valsStr = FileSystem::Split(str, ',');

				for (const String& val : valsStr)
				{
					uint32 decimals = 0;
					vals.push_back(UtilStr::StringToFloat(val, decimals));
				}

				return vals;
			};

			const HashMap<String, ShaderPropertyType> nameToPropType = {
				linatl::make_pair("float", ShaderPropertyType::Float),
				linatl::make_pair("bool", ShaderPropertyType::Bool),
				linatl::make_pair("uint", ShaderPropertyType::UInt32),
				linatl::make_pair("vec2", ShaderPropertyType::Vec2),
				linatl::make_pair("vec3", ShaderPropertyType::Vec3),
				linatl::make_pair("vec4", ShaderPropertyType::Vec4),
				linatl::make_pair("ivec2", ShaderPropertyType::IVec2),
				linatl::make_pair("ivec3", ShaderPropertyType::IVec3),
				linatl::make_pair("ivec4", ShaderPropertyType::IVec4),
				linatl::make_pair("mat4", ShaderPropertyType::Matrix4),
				linatl::make_pair("LinaTexture2D", ShaderPropertyType::Texture2D),
			};

			while (std::getline(f, line))
			{
				// + 1 \n
				materialBlockEnd += line.length() + 1;

				const String lineTrimmed = FileSystem::RemoveWhitespaces(line);

				if (commentBlock)
				{
					if (lineTrimmed.length() >= 2 && lineTrimmed.substr(0, 2).compare("*/") == 0)
						commentBlock = false;
					continue;
				}

				if (lineTrimmed.length() >= 2 && lineTrimmed.substr(0, 2).compare("//") == 0)
					continue;

				if (lineTrimmed.length() >= 2 && lineTrimmed.substr(0, 2).compare("/*") == 0)
				{
					commentBlock = true;
					continue;
				}

				if (line.find(materialIdent) != String::npos)
				{
					parsingMaterialStruct = true;
					continue;
				}

				if (parsingMaterialStruct)
				{
					ShaderPropertyDefinition def = {};

					bool found = false;
					for (const auto& [str, propType] : nameToPropType)
					{
						if (property(line, str, def.name))
						{
							def.sid	 = TO_SID(def.name);
							def.type = propType;
							outProperties.push_back(def);
							found = true;
							break;
						}
					}

					if (found)
						continue;

					if (line.find("}") != String::npos)
						break;

					if (line.find("{") != String::npos)
						continue;

					LINA_ERR("LinaMaterial has an unsupported property type!");
					return false;
				}
			}

			if (parsingMaterialStruct)
			{
				String materialCastFunction = "\nLinaMaterial LINA_GET_MATERIAL(uint startOffset)\n";
				materialCastFunction += "{\n";
				materialCastFunction += "\tLinaMaterial m;\n";
				materialCastFunction += "\tuint index = startOffset;\n";

				for (const ShaderPropertyDefinition& prop : outProperties)
				{
					if (prop.type == ShaderPropertyType::Float)
					{
						materialCastFunction += "\tm." + prop.name + " = uintBitsToFloat(LINA_MATERIALS.data[index++]);\n\n";
						continue;
					}

					if (prop.type == ShaderPropertyType::UInt32)
					{
						materialCastFunction += "\tm." + prop.name + " = LINA_MATERIALS.data[index++];\n\n";
						continue;
					}

					if (prop.type == ShaderPropertyType::Bool)
					{
						materialCastFunction += "\tm." + prop.name + " = LINA_MATERIALS.data[index] != 0u;\n";
						materialCastFunction += "\tindex += 1;\n\n";
						continue;
					}

					if (prop.type == ShaderPropertyType::Vec2)
					{
						materialCastFunction += "\tm." + prop.name + ".x = uintBitsToFloat(LINA_MATERIALS.data[index++]);\n";
						materialCastFunction += "\tm." + prop.name + ".y = uintBitsToFloat(LINA_MATERIALS.data[index++]);\n\n";
						continue;
					}

					if (prop.type == ShaderPropertyType::Vec3)
					{
						materialCastFunction += "\tm." + prop.name + ".x = uintBitsToFloat(LINA_MATERIALS.data[index++]);\n";
						materialCastFunction += "\tm." + prop.name + ".y = uintBitsToFloat(LINA_MATERIALS.data[index++]);\n";
						materialCastFunction += "\tm." + prop.name + ".z = uintBitsToFloat(LINA_MATERIALS.data[index++]);\n\n";
						continue;
					}

					if (prop.type == ShaderPropertyType::Vec4)
					{
						materialCastFunction += "\tm." + prop.name + ".x = uintBitsToFloat(LINA_MATERIALS.data[index++]);\n";
						materialCastFunction += "\tm." + prop.name + ".y = uintBitsToFloat(LINA_MATERIALS.data[index++]);\n";
						materialCastFunction += "\tm." + prop.name + ".z = uintBitsToFloat(LINA_MATERIALS.data[index++]);\n";
						materialCastFunction += "\tm." + prop.name + ".w = uintBitsToFloat(LINA_MATERIALS.data[index++]);\n\n";
						continue;
					}

					if (prop.type == ShaderPropertyType::IVec2)
					{
						materialCastFunction += "\tm." + prop.name + ".x = LINA_MATERIALS.data[index++];\n";
						materialCastFunction += "\tm." + prop.name + ".y = LINA_MATERIALS.data[index++];\n\n";
						continue;
					}

					if (prop.type == ShaderPropertyType::IVec3)
					{
						materialCastFunction += "\tm." + prop.name + ".x = LINA_MATERIALS.data[index++];\n";
						materialCastFunction += "\tm." + prop.name + ".y = LINA_MATERIALS.data[index++];\n";
						materialCastFunction += "\tm." + prop.name + ".z = LINA_MATERIALS.data[index++];\n\n";
						continue;
					}

					if (prop.type == ShaderPropertyType::IVec4)
					{
						materialCastFunction += "\tm." + prop.name + ".x = LINA_MATERIALS.data[index++];\n";
						materialCastFunction += "\tm." + prop.name + ".y = LINA_MATERIALS.data[index++];\n";
						materialCastFunction += "\tm." + prop.name + ".z = LINA_MATERIALS.data[index++];\n";
						materialCastFunction += "\tm." + prop.name + ".w = LINA_MATERIALS.data[index++];\n\n";
						continue;
					}

					if (prop.type == ShaderPropertyType::Texture2D)
					{
						materialCastFunction += "\tm." + prop.name + ".txt = LINA_MATERIALS.data[index++];\n";
						materialCastFunction += "\tm." + prop.name + ".smp = LINA_MATERIALS.data[index++];\n\n";
						continue;
					}

					if (prop.type == ShaderPropertyType::Matrix4)
					{
						materialCastFunction += "\tm." + prop.name + "[0] = vec4(\n";
						materialCastFunction += "\t\tuintBitsToFloat(LINA_MATERIALS.data[index++]),\n";
						materialCastFunction += "\t\tuintBitsToFloat(LINA_MATERIALS.data[index++]),\n";
						materialCastFunction += "\t\tuintBitsToFloat(LINA_MATERIALS.data[index++]),\n";
						materialCastFunction += "\t\tuintBitsToFloat(LINA_MATERIALS.data[index++])\n";
						materialCastFunction += "\t);\n\n";

						materialCastFunction += "\tm." + prop.name + "[1] = vec4(\n";
						materialCastFunction += "\t\tuintBitsToFloat(LINA_MATERIALS.data[index++]),\n";
						materialCastFunction += "\t\tuintBitsToFloat(LINA_MATERIALS.data[index++]),\n";
						materialCastFunction += "\t\tuintBitsToFloat(LINA_MATERIALS.data[index++]),\n";
						materialCastFunction += "\t\tuintBitsToFloat(LINA_MATERIALS.data[index++])\n";
						materialCastFunction += "\t);\n\n";

						materialCastFunction += "\tm." + prop.name + "[2] = vec4(\n";
						materialCastFunction += "\t\tuintBitsToFloat(LINA_MATERIALS.data[index++]),\n";
						materialCastFunction += "\t\tuintBitsToFloat(LINA_MATERIALS.data[index++]),\n";
						materialCastFunction += "\t\tuintBitsToFloat(LINA_MATERIALS.data[index++]),\n";
						materialCastFunction += "\t\tuintBitsToFloat(LINA_MATERIALS.data[index++])\n";
						materialCastFunction += "\t);\n\n";

						materialCastFunction += "\tm." + prop.name + "[3] = vec4(\n";
						materialCastFunction += "\t\tuintBitsToFloat(LINA_MATERIALS.data[index++]),\n";
						materialCastFunction += "\t\tuintBitsToFloat(LINA_MATERIALS.data[index++]),\n";
						materialCastFunction += "\t\tuintBitsToFloat(LINA_MATERIALS.data[index++]),\n";
						materialCastFunction += "\t\tuintBitsToFloat(LINA_MATERIALS.data[index++])\n";
						materialCastFunction += "\t);\n\n";
						continue;
					}
				}

				materialCastFunction += "\treturn m;\n";
				materialCastFunction += "}\n";
				block.insert(materialBlockEnd, materialCastFunction);
			}

			return true;
		}
	} // namespace

	bool ShaderPreprocessor::VerifyFullShader(const String& input)
	{
		if (input.find("#version") != String::npos)
		{
			LINA_ERR("Shaders can't have version directives, they are internally managed.");
			return false;
		}

		if (input.find("#extension") != String::npos)
		{
			LINA_ERR("Shaders can't have extension directives, they are internally managed.");
			return false;
		}

		return true;
	}

	bool ShaderPreprocessor::ExtractVertexFrag(const String& input, String& outVertex, String& outFrag)
	{
		outVertex.clear();
		outFrag.clear();
		outVertex = ExtractBlock(input, "#lina_vs", "#lina_end");
		outFrag	  = ExtractBlock(input, "#lina_fs", "#lina_end");

		if (outVertex.empty())
		{
			LINA_ERR("Vertex block could not be found, double check #lina_vs and #lina_end directives.");
			return false;
		}

		if (outFrag.empty())
		{
			LINA_ERR("Frag block could not be found, double check #lina_fs and #lina_end directives.");
			return false;
		}

		// Normalize line endings.
		outVertex.erase(linatl::remove(outVertex.begin(), outVertex.end(), '\r'), outVertex.end());
		outFrag.erase(linatl::remove(outFrag.begin(), outFrag.end(), '\r'), outFrag.end());

		return true;
	}

	void ShaderPreprocessor::InjectVersionAndExtensions(String& input)
	{
		const String versionDirective	   = "#version 460 \n";
		const String dynamicIndexDirective = "#extension GL_EXT_nonuniform_qualifier : enable\n";

		input.insert(input.length(), versionDirective);
		input.insert(input.length(), dynamicIndexDirective);
	}

	bool ShaderPreprocessor::InjectMaterialIfRequired(String& input, Vector<ShaderPropertyDefinition>& outProperties)
	{
		outProperties.clear();
		return ProcessMaterialData(input, outProperties);
	}

	void ShaderPreprocessor::InjectRenderPassInputs(String& input, RenderPassType type)
	{
		const String globalHeader = "#include \"Resources/Core/Shaders/Common/GlobalData.linashader\"\n";

		input.insert(input.length(), globalHeader);

		if (type == RenderPassType::Deferred)
		{
			const String header = "#include \"Resources/Core/Shaders/Common/RenderPass_Deferred.linashader\"\n";
			input.insert(input.length(), header.c_str());
			return;
		}

		if (type == RenderPassType::Forward)
		{
			const String header = "#include \"Resources/Core/Shaders/Common/RenderPass_Forward.linashader\"\n";
			input.insert(input.length(), header.c_str());
			return;
		}

		if (type == RenderPassType::Lighting)
		{
			const String header = "#include \"Resources/Core/Shaders/Common/RenderPass_Lighting.linashader\"\n";
			input.insert(input.length(), header.c_str());
			return;
		}
	}

	void ShaderPreprocessor::InjectVertexMain(String& input, ShaderType type)
	{
		String path = "";

		if (type == ShaderType::OpaqueSurface)
			path = "Resources/Core/Shaders/Common/MainVertex_Deferred.linashader";
		else if (type == ShaderType::TransparentSurface)
			path = "Resources/Core/Shaders/Common/MainVertex_Forward.linashader";
		else if (type == ShaderType::Sky)
			path = "Resources/Core/Shaders/Common/MainVertex_Sky.linashader";
		else
		{
			LINA_ASSERT(false, "");
		}

		String str = FileSystem::ReadFileContentsAsString(path);
		input.insert(input.length(), str);
	}

	void ShaderPreprocessor::InjectSkinnedVertexMain(String& input, ShaderType type)
	{
		String path = "";

		if (type == ShaderType::OpaqueSurface)
			path = "Resources/Core/Shaders/Common/MainVertexSkinned_Deferred.linashader";
		else if (type == ShaderType::TransparentSurface)
			path = "Resources/Core/Shaders/Common/MainVertexSkinned_Forward.linashader";
		else
		{
			LINA_ASSERT(false, "");
		}

		String str = FileSystem::ReadFileContentsAsString(path);
		input.insert(input.length(), str);
	}

	void ShaderPreprocessor::InjectFragMain(String& input, ShaderType type)
	{
		String path = "";

		if (type == ShaderType::OpaqueSurface)
			path = "Resources/Core/Shaders/Common/MainFrag_Deferred.linashader";
		else if (type == ShaderType::TransparentSurface)
			path = "Resources/Core/Shaders/Common/MainFrag_Forward.linashader";
		else if (type == ShaderType::Sky)
			path = "Resources/Core/Shaders/Common/MainFrag_Sky.linashader";
		else
		{
			LINA_ASSERT(false, "");
		}

		String str = FileSystem::ReadFileContentsAsString(path);
		input.insert(input.length(), str);
	}

	void ShaderPreprocessor::InjectUserShader(String& input, const String& shader)
	{
		static String ident = "//##user_shader_injected_here";
		const size_t  pos	= input.find(ident);
		if (pos == String::npos)
			return;

		input.insert(pos, shader);
	}

	ShaderType ShaderPreprocessor::GetShaderType(const String& input)
	{
		if (input.find("#lina_shader_opaque") != String::npos)
			return ShaderType::OpaqueSurface;

		if (input.find("#lina_shader_transparent") != String::npos)
			return ShaderType::TransparentSurface;

		if (input.find("#lina_shader_sky") != String::npos)
			return ShaderType::Sky;

		if (input.find("#lina_shader_lighting") != String::npos)
			return ShaderType::Lighting;

		if (input.find("#lina_shader_postprocess") != String::npos)
			return ShaderType::PostProcess;

		return ShaderType::Custom;
	}
} // namespace Lina
