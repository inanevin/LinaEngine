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

		void ProcessMaterialData(String& block, Vector<ShaderProperty>& outProperties)
		{
			const String materialIdent = "struct LINA_MATERIAL";

			std::istringstream f(block.c_str());
			std::string		   line = "";

			bool parsingMaterialStruct = false;

			auto property = [](const String& line, const String& propType, String& outName) -> bool {
				const String str   = propType;
				const size_t found = line.find(str);
				if (found != String::npos)
				{
					const String rest = line.substr(found + str.length(), line.length() - found - str.length());
					const size_t bgn  = rest.find_first_not_of(" ");
					const size_t end  = rest.find_last_of(";");
					outName			  = rest.substr(bgn, end - bgn);
					return true;
				}

				return false;
			};

			ShaderProperty prop = {};

			while (std::getline(f, line))
			{
				if (line.find(materialIdent) != String::npos)
				{
					parsingMaterialStruct = true;
					continue;
				}

				if (parsingMaterialStruct)
				{
					if (property(line, "float", prop.name))
					{
						prop.type = ShaderPropertyType::Float;
						prop.size = sizeof(float);
						prop.sid  = TO_SID(prop.name);
						outProperties.push_back(prop);
						continue;
					}

					if (property(line, "uint", prop.name))
					{
						prop.type = ShaderPropertyType::UInt32;
						prop.size = sizeof(uint32);
						prop.sid  = TO_SID(prop.name);
						outProperties.push_back(prop);
						continue;
					}

					if (property(line, "bool", prop.name))
					{
						prop.type = ShaderPropertyType::Bool;
						prop.size = sizeof(bool);
						prop.sid  = TO_SID(prop.name);
						outProperties.push_back(prop);
						continue;
					}

					if (property(line, "vec2", prop.name))
					{
						prop.type = ShaderPropertyType::Vec2;
						prop.size = sizeof(Vector2);
						prop.sid  = TO_SID(prop.name);
						outProperties.push_back(prop);
						continue;
					}

					if (property(line, "vec3", prop.name))
					{
						prop.type = ShaderPropertyType::Vec3;
						prop.size = sizeof(Vector3);
						prop.sid  = TO_SID(prop.name);
						outProperties.push_back(prop);
						continue;
					}

					if (property(line, "vec4", prop.name))
					{
						prop.type = ShaderPropertyType::Vec4;
						prop.size = sizeof(Vector4);
						prop.sid  = TO_SID(prop.name);
						outProperties.push_back(prop);
						continue;
					}

					if (property(line, "ivec2", prop.name))
					{
						prop.type = ShaderPropertyType::IVec2;
						prop.size = sizeof(Vector2);
						prop.sid  = TO_SID(prop.name);
						outProperties.push_back(prop);
						continue;
					}

					if (property(line, "ivec3", prop.name))
					{
						prop.type = ShaderPropertyType::IVec3;
						prop.size = sizeof(Vector3);
						prop.sid  = TO_SID(prop.name);
						outProperties.push_back(prop);
						continue;
					}

					if (property(line, "ivec4", prop.name))
					{
						prop.type = ShaderPropertyType::IVec4;
						prop.size = sizeof(Vector4);
						prop.sid  = TO_SID(prop.name);
						outProperties.push_back(prop);
						continue;
					}

					if (property(line, "mat4", prop.name))
					{
						prop.type = ShaderPropertyType::Matrix4;
						prop.size = sizeof(Matrix4);
						prop.sid  = TO_SID(prop.name);
						outProperties.push_back(prop);
						continue;
					}

					if (property(line, "LinaTexture2D", prop.name))
					{
						prop.type = ShaderPropertyType::Texture2D;
						prop.size = sizeof(LinaTexture2D);
						prop.sid  = TO_SID(prop.name);
						outProperties.push_back(prop);
						continue;
					}

					if (line.find("}") != String::npos)
					{
						break;
					}

					if (line.find("{") != String::npos)
						continue;

					LINA_ASSERT(false, "Unkown type!");
				}
			}
		}
	} // namespace

	bool ShaderPreprocessor::Preprocess(const String& text, HashMap<LinaGX::ShaderStage, String>& outStages, const RenderPassDescriptorType& rpType, Vector<ShaderProperty>& outProperties)
	{
		if (text.find("#version") != String::npos)
		{
			LINA_ASSERT(false, "Shaders can't have version directives, these are internally injected by Lina!");
			return false;
		}

		std::istringstream f(text.c_str());
		std::string		   line	 = "";
		bool			   isCmt = false;

		// const String endIdentifier		  = "#lina_end";
		// while (std::getline(f, line))
		// {
		// 	if (!line.empty() && *line.rbegin() == '\r')
		// 		line.erase(line.end() - 1);
		// 	const size_t cmtPos = line.find("//");
		// 	if (cmtPos != std::string::npos && cmtPos == 0)
		// 		continue;
		// 	if (line.find("/*") != std::string::npos)
		// 	{
		// 		isCmt = true;
		// 		continue;
		// 	}
		// 	if (isCmt)
		// 	{
		// 		if (line.find("*/") != std::string::npos)
		// 			isCmt = false;
		// 		continue;
		// 	}
		// 	// const String lineSqueezed = FileSystem::RemoveWhitespaces(line.c_str());
		//
		// }

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

		String rpText = "";
		if (rpType == RenderPassDescriptorType::Main)
			rpText = "main";
		else if (rpType == RenderPassDescriptorType::Gui)
			rpText = "gui";
		else if (rpType == RenderPassDescriptorType::Lighting)
			rpText = "lighting";

		const String renderPassPath	   = String("Resources/Core/Shaders/Common/RenderPass_") + rpText + ".linashader";
		const String globalDataInclude = getInclude("Resources/Core/Shaders/Common/GlobalData.linashader");
		const String renderPassInclude = getInclude(renderPassPath.c_str());

		const String versionDirective	   = "#version 460 \n";
		const String dynamicIndexDirective = "#extension GL_EXT_nonuniform_qualifier : enable";

		for (const auto& [stage, ident] : blockIdentifiers)
		{
			String					  block = ExtractBlock(text.c_str(), ident.c_str(), "#lina_end");
			LinaGX::ShaderCompileData compileData;

			if (outProperties.empty())
				ProcessMaterialData(block, outProperties);

			if (!block.empty())
			{
				block.insert(0, versionDirective);
				block.insert(versionDirective.size(), dynamicIndexDirective);
				block.insert(versionDirective.size() + dynamicIndexDirective.size(), globalDataInclude);
				block.insert(versionDirective.size() + dynamicIndexDirective.size() + globalDataInclude.size(), renderPassInclude);
				outStages[stage] = block;
			}
		}

		return true;
	}
} // namespace Lina
