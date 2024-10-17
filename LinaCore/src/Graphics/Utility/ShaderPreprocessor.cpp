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

namespace Lina
{

#define HEADER_END "//lina_##_header_##_end\n"

	namespace
	{
		bool FindShaderType(ShaderType& outType, const String& txt)
		{
			// if (txt.find("#LINA_SHADER_DEFERRED_OBJECT") != String::npos)
			// {
			// 	outType = ShaderType::DeferredObject;
			// 	return true;
			// }
			//
			// if (txt.find("#LINA_SHADER_FORWARD_OBJECT") != String::npos)
			// {
			// 	outType = ShaderType::ForwardObject;
			// 	return true;
			// }
			//
			// if (txt.find("#LINA_SHADER_LIGHTING") != String::npos)
			// {
			// 	outType = ShaderType::Lighting;
			// 	return true;
			// }
			//
			// if (txt.find("#LINA_SHADER_SKY") != String::npos)
			// {
			// 	outType = ShaderType::Sky;
			// 	return true;
			// }
			//
			// if (txt.find("#LINA_SHADER_POST_PROCESS") != String::npos)
			// {
			// 	outType = ShaderType::PostProcess;
			// 	return true;
			// }
			//
			// if (txt.find("#LINA_SHADER_CUSTOM") != String::npos)
			// {
			// 	outType = ShaderType::Custom;
			// 	return true;
			// }

			return false;
		}

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

		void ProcessMaterialData(String& block, Vector<ShaderProperty*>& outProperties)
		{
			const String materialIdent = "struct LinaMaterial";

			if (block.find(materialIdent) == String::npos)
				return;

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
					ShaderProperty* prop = new ShaderProperty();

					if (property(line, "float", prop->name))
					{
						float value = 0.0f;
						prop->type	= ShaderPropertyType::Float;
						prop->sid	= TO_SID(prop->name);
						prop->data	= {new uint8[sizeof(uint32)], sizeof(float)};
						MEMCPY(prop->data.data(), &value, sizeof(float));
						outProperties.push_back(prop);
						continue;
					}

					if (property(line, "uint", prop->name))
					{
						uint32 value = 0;
						prop->type	 = ShaderPropertyType::UInt32;
						prop->sid	 = TO_SID(prop->name);
						prop->data	 = {new uint8[sizeof(uint32)], sizeof(uint32)};
						MEMCPY(prop->data.data(), &value, sizeof(uint32));
						outProperties.push_back(prop);
						continue;
					}

					if (property(line, "bool", prop->name))
					{
						bool value = false;
						prop->type = ShaderPropertyType::Bool;
						prop->sid  = TO_SID(prop->name);
						prop->data = {new uint8[sizeof(uint32)], sizeof(uint32)};
						MEMSET(prop->data.data(), 0, sizeof(uint32));
						MEMCPY(prop->data.data(), &value, sizeof(bool));
						outProperties.push_back(prop);
						continue;
					}

					if (property(line, "vec2", prop->name))
					{
						Vector2 val = Vector2::Zero;
						prop->type	= ShaderPropertyType::Vec2;
						prop->sid	= TO_SID(prop->name);
						prop->data	= {new uint8[sizeof(Vector2)], sizeof(Vector2)};
						MEMCPY(prop->data.data(), &val, sizeof(Vector2));
						outProperties.push_back(prop);
						continue;
					}

					if (property(line, "vec3", prop->name))
					{
						Vector3 val = Vector3::Zero;
						prop->type	= ShaderPropertyType::Vec3;
						prop->sid	= TO_SID(prop->name);
						prop->data	= {new uint8[sizeof(uint32) * 3], sizeof(uint32) * 3};
						MEMCPY(prop->data.data(), &val, sizeof(uint32) * 3);
						outProperties.push_back(prop);
						continue;
					}

					if (property(line, "vec4", prop->name))
					{
						Vector4 val = Vector4::Zero;
						prop->type	= ShaderPropertyType::Vec4;
						prop->sid	= TO_SID(prop->name);
						prop->data	= {new uint8[sizeof(Vector4)], sizeof(Vector4)};
						MEMCPY(prop->data.data(), &val, sizeof(Vector4));
						outProperties.push_back(prop);
						continue;
					}

					if (property(line, "ivec2", prop->name))
					{
						Vector2i val = Vector2i::Zero;
						prop->type	 = ShaderPropertyType::IVec2;
						prop->sid	 = TO_SID(prop->name);
						prop->data	 = {new uint8[sizeof(Vector2i)], sizeof(Vector2i)};
						MEMCPY(prop->data.data(), &val, sizeof(Vector2i));
						outProperties.push_back(prop);
						continue;
					}

					if (property(line, "ivec3", prop->name))
					{
						Vector3i val = Vector3i::Zero;
						prop->type	 = ShaderPropertyType::IVec3;
						prop->sid	 = TO_SID(prop->name);
						prop->data	 = {new uint8[sizeof(Vector3i)], sizeof(Vector3i)};
						MEMCPY(prop->data.data(), &val, sizeof(Vector3i));
						outProperties.push_back(prop);
						continue;
					}

					if (property(line, "ivec4", prop->name))
					{
						Vector4i val = Vector4i::Zero;
						prop->type	 = ShaderPropertyType::IVec4;
						prop->sid	 = TO_SID(prop->name);
						prop->data	 = {new uint8[sizeof(Vector4i)], sizeof(Vector4i)};
						MEMCPY(prop->data.data(), &val, sizeof(Vector4i));
						outProperties.push_back(prop);
						continue;
					}

					if (property(line, "mat4", prop->name))
					{
						Matrix4 value = Matrix4::Identity();
						prop->type	  = ShaderPropertyType::Matrix4;
						prop->sid	  = TO_SID(prop->name);
						prop->data	  = {new uint8[sizeof(Matrix4)], sizeof(Matrix4)};
						MEMCPY(prop->data.data(), &value, sizeof(Matrix4));
						outProperties.push_back(prop);
						continue;
					}

					if (property(line, "LinaTexture2D", prop->name))
					{
						prop->type = ShaderPropertyType::Texture2D;
						prop->sid  = TO_SID(prop->name);
						prop->data = {new uint8[sizeof(ResourceID) * 2], sizeof(ResourceID) * 2};
						MEMSET(prop->data.data(), 0, sizeof(ResourceID) * 2);
						outProperties.push_back(prop);
						continue;
					}

					delete prop;

					if (line.find("}") != String::npos)
					{
						break;
					}

					if (line.find("{") != String::npos)
						continue;

					LINA_ASSERT(false, "Unknown type!");
					LINA_ERR("LinaMaterial has an unsupported property type!");
				}
			}

			if (parsingMaterialStruct)
			{
				String materialCastFunction = "\nLinaMaterial LINA_GET_MATERIAL(uint startOffset)\n";
				materialCastFunction += "{\n";
				materialCastFunction += "\tLinaMaterial m;\n";
				materialCastFunction += "\tuint index = startOffset;\n";

				for (ShaderProperty* prop : outProperties)
				{
					if (prop->type == ShaderPropertyType::Float)
					{
						materialCastFunction += "\tm." + prop->name + " = uintBitsToFloat(LINA_MATERIALS.data[index++]);\n\n";
						continue;
					}

					if (prop->type == ShaderPropertyType::UInt32)
					{
						materialCastFunction += "\tm." + prop->name + " = LINA_MATERIALS.data[index++];\n\n";
						continue;
					}

					if (prop->type == ShaderPropertyType::Bool)
					{
						materialCastFunction += "\tm." + prop->name + " = LINA_MATERIALS.data[index] != 0u;\n";
						materialCastFunction += "\tindex += 1;\n\n";
						continue;
					}

					if (prop->type == ShaderPropertyType::Vec2)
					{
						materialCastFunction += "\tm." + prop->name + ".x = uintBitsToFloat(LINA_MATERIALS.data[index++]);\n";
						materialCastFunction += "\tm." + prop->name + ".y = uintBitsToFloat(LINA_MATERIALS.data[index++]);\n\n";
						continue;
					}

					if (prop->type == ShaderPropertyType::Vec3)
					{
						materialCastFunction += "\tm." + prop->name + ".x = uintBitsToFloat(LINA_MATERIALS.data[index++]);\n";
						materialCastFunction += "\tm." + prop->name + ".y = uintBitsToFloat(LINA_MATERIALS.data[index++]);\n";
						materialCastFunction += "\tm." + prop->name + ".z = uintBitsToFloat(LINA_MATERIALS.data[index++]);\n\n";
						continue;
					}

					if (prop->type == ShaderPropertyType::Vec4)
					{
						materialCastFunction += "\tm." + prop->name + ".x = uintBitsToFloat(LINA_MATERIALS.data[index++]);\n";
						materialCastFunction += "\tm." + prop->name + ".y = uintBitsToFloat(LINA_MATERIALS.data[index++]);\n";
						materialCastFunction += "\tm." + prop->name + ".z = uintBitsToFloat(LINA_MATERIALS.data[index++]);\n";
						materialCastFunction += "\tm." + prop->name + ".w = uintBitsToFloat(LINA_MATERIALS.data[index++]);\n\n";
						continue;
					}

					if (prop->type == ShaderPropertyType::IVec2)
					{
						materialCastFunction += "\tm." + prop->name + ".x = LINA_MATERIALS.data[index++];\n";
						materialCastFunction += "\tm." + prop->name + ".y = LINA_MATERIALS.data[index++];\n\n";
						continue;
					}

					if (prop->type == ShaderPropertyType::IVec3)
					{
						materialCastFunction += "\tm." + prop->name + ".x = LINA_MATERIALS.data[index++];\n";
						materialCastFunction += "\tm." + prop->name + ".y = LINA_MATERIALS.data[index++];\n";
						materialCastFunction += "\tm." + prop->name + ".z = LINA_MATERIALS.data[index++];\n\n";
						continue;
					}

					if (prop->type == ShaderPropertyType::IVec4)
					{
						materialCastFunction += "\tm." + prop->name + ".x = LINA_MATERIALS.data[index++];\n";
						materialCastFunction += "\tm." + prop->name + ".y = LINA_MATERIALS.data[index++];\n";
						materialCastFunction += "\tm." + prop->name + ".z = LINA_MATERIALS.data[index++];\n";
						materialCastFunction += "\tm." + prop->name + ".w = LINA_MATERIALS.data[index++];\n\n";
						continue;
					}

					if (prop->type == ShaderPropertyType::Texture2D)
					{
						materialCastFunction += "\tm." + prop->name + ".txt = LINA_MATERIALS.data[index++];\n";
						materialCastFunction += "\tm." + prop->name + ".smp = LINA_MATERIALS.data[index++];\n\n";
						continue;
					}

					if (prop->type == ShaderPropertyType::Matrix4)
					{
						materialCastFunction += "\tm." + prop->name + "[0] = vec4(\n";
						materialCastFunction += "\t\tuintBitsToFloat(LINA_MATERIALS.data[index++]),\n";
						materialCastFunction += "\t\tuintBitsToFloat(LINA_MATERIALS.data[index++]),\n";
						materialCastFunction += "\t\tuintBitsToFloat(LINA_MATERIALS.data[index++]),\n";
						materialCastFunction += "\t\tuintBitsToFloat(LINA_MATERIALS.data[index++])\n";
						materialCastFunction += "\t);\n\n";

						materialCastFunction += "\tm." + prop->name + "[1] = vec4(\n";
						materialCastFunction += "\t\tuintBitsToFloat(LINA_MATERIALS.data[index++]),\n";
						materialCastFunction += "\t\tuintBitsToFloat(LINA_MATERIALS.data[index++]),\n";
						materialCastFunction += "\t\tuintBitsToFloat(LINA_MATERIALS.data[index++]),\n";
						materialCastFunction += "\t\tuintBitsToFloat(LINA_MATERIALS.data[index++])\n";
						materialCastFunction += "\t);\n\n";

						materialCastFunction += "\tm." + prop->name + "[2] = vec4(\n";
						materialCastFunction += "\t\tuintBitsToFloat(LINA_MATERIALS.data[index++]),\n";
						materialCastFunction += "\t\tuintBitsToFloat(LINA_MATERIALS.data[index++]),\n";
						materialCastFunction += "\t\tuintBitsToFloat(LINA_MATERIALS.data[index++]),\n";
						materialCastFunction += "\t\tuintBitsToFloat(LINA_MATERIALS.data[index++])\n";
						materialCastFunction += "\t);\n\n";

						materialCastFunction += "\tm." + prop->name + "[3] = vec4(\n";
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
		}
	} // namespace

	bool ShaderPreprocessor::Preprocess(const String& text, HashMap<LinaGX::ShaderStage, String>& outStages, Vector<ShaderProperty*>& outProperties, ShaderType& outShaderType)
	{
		if (text.find("#version") != String::npos)
		{
			LINA_ASSERT(false, "Shaders can't have version directives, these are internally injected by Lina!");
			return false;
		}

		std::istringstream f(text.c_str());
		std::string		   line	 = "";
		bool			   isCmt = false;

		if (!FindShaderType(outShaderType, text))
		{
			LINA_ERR("Shader type can't be found!");
			return false;
		}

		HashMap<LinaGX::ShaderStage, String> blockIdentifiers;
		blockIdentifiers[LinaGX::ShaderStage::Fragment] = "#lina_fs";
		blockIdentifiers[LinaGX::ShaderStage::Vertex]	= "#lina_vs";
		blockIdentifiers[LinaGX::ShaderStage::Compute]	= "#lina_cs";
		blockIdentifiers[LinaGX::ShaderStage::Geometry] = "#lina_gs";

		auto getInclude = [](const char* path) -> String {
			String		 include	 = FileSystem::ReadFileContentsAsString(path);
			const size_t commentsEnd = include.find("*/") + 2;
			include					 = include.substr(commentsEnd, include.size() - commentsEnd);
			return include;
		};

		const String versionDirective	   = "#version 460 \n";
		const String dynamicIndexDirective = "#extension GL_EXT_nonuniform_qualifier : enable";

		for (const auto& [stage, ident] : blockIdentifiers)
		{
			String block = ExtractBlock(text.c_str(), ident.c_str(), "#lina_end");

			// Normalize line endings
			block.erase(linatl::remove(block.begin(), block.end(), '\r'), block.end());

			if (outProperties.empty())
				ProcessMaterialData(block, outProperties);

			if (!block.empty())
			{
				block.insert(0, versionDirective);
				block.insert(versionDirective.size(), dynamicIndexDirective);
				outStages[stage] = block;
			}
		}

		return true;
	}

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

		size_t sz = 0;

		input.insert(0, versionDirective.c_str());
		sz += versionDirective.size();

		input.insert(sz, dynamicIndexDirective.c_str());
		sz += dynamicIndexDirective.size();

		// end.
		input.insert(sz, HEADER_END);
	}

	void ShaderPreprocessor::InjectMaterialIfRequired(String& input, Vector<ShaderProperty*>& outProperties)
	{
		outProperties.clear();
		ProcessMaterialData(input, outProperties);
	}

	void ShaderPreprocessor::InjectRenderPassInputs(String& input, RenderPassDescriptorType type)
	{
		const String headerEnd(HEADER_END);
		const size_t headerEndPos = input.find(HEADER_END);
		size_t		 insertPos	  = headerEndPos + headerEnd.size() + 1;

		const String globalHeader = "#include \"Resources/Core/Shaders/Common/GlobalData.linashader\"\n";
		input.insert(insertPos, globalHeader.c_str());
		insertPos += globalHeader.size();

		if (type == RenderPassDescriptorType::Deferred)
		{
			const String rpHeader = "#include \"Resources/Core/Shaders/Common/RenderPass_Deferred.linashader\"\n";
			input.insert(insertPos, rpHeader.c_str());
			return;
		}

		if (type == RenderPassDescriptorType::Lighting)
		{
			const String rpHeader = "#include \"Resources/Core/Shaders/Common/RenderPass_Lighting.linashader\"\n";
			input.insert(insertPos, rpHeader.c_str());
			return;
		}
	}

	void ShaderPreprocessor::InjectVertexMain(String& input, ShaderType type)
	{
		input += "\n";

		if (type == ShaderType::OpaqueSurface)
		{
			input += "#include \"Resources/Core/Shaders/Common/MainVertex_Deferred.linashader\"\n";
			return;
		}

		if (type == ShaderType::Sky)
		{
			input += "#include \"Resources/Core/Shaders/Common/MainVertex_Sky.linashader\"\n";
			return;
		}
	}

	void ShaderPreprocessor::InjectSkinnedVertexMain(String& input, ShaderType type)
	{
		input += "\n";

		if (type == ShaderType::OpaqueSurface)
		{
			input += "#include \"Resources/Core/Shaders/Common/MainVertexSkinned_Deferred.linashader\"\n";
			return;
		}

		if (type == ShaderType::Sky)
		{
			LINA_ASSERT(false, "");
			return;
		}
	}

	void ShaderPreprocessor::InjectFragMain(String& input, ShaderType type)
	{
		input += "\n";

		if (type == ShaderType::OpaqueSurface)
		{
			input += "#include \"Resources/Core/Shaders/Common/MainFrag_Deferred.linashader\"\n";
			return;
		}

		if (type == ShaderType::Sky)
		{
			input += "#include \"Resources/Core/Shaders/Common/MainFrag_Sky.linashader\"\n";
			return;
		}
	}
} // namespace Lina
