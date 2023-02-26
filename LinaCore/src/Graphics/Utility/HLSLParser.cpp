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

#include "Graphics/Utility/HLSLParser.hpp"
#include "Graphics/Data/RenderData.hpp"
#include "FileSystem/FileSystem.hpp"
#include "Data/CommonData.hpp"
#include "Graphics/Resource/MaterialProperty.hpp"

namespace Lina
{
	String HLSLParser::Parse(const String& raw, Bitmask16& outDrawPassMask, PipelineType& outPipelineType, Vector<MaterialPropertyBase*>& outProperties, Vector<MaterialPropertyBase*>& outTextures, HashMap<ShaderStage, String>& outStages)
	{
		std::istringstream f(raw.c_str());
		std::string		   line			   = "";
		const std::string  includeText	   = "LINA_INCLUDE";
		std::string		   finalText	   = "";
		bool			   parsingMaterial = false;
		bool			   isLicense	   = true;

		Vector<Pair<ShaderStage, String>> stages;
		stages.push_back(linatl::make_pair(ShaderStage::Vertex, "VSMain"));
		stages.push_back(linatl::make_pair(ShaderStage::Fragment, "PSMain"));

		Vector<Pair<MaterialPropertyType, String>> propertyTypes;
		propertyTypes.push_back(linatl::make_pair(MaterialPropertyType::Float, "float"));
		propertyTypes.push_back(linatl::make_pair(MaterialPropertyType::Int, "int"));
		propertyTypes.push_back(linatl::make_pair(MaterialPropertyType::Bool, "bool"));
		propertyTypes.push_back(linatl::make_pair(MaterialPropertyType::Vector2, "float2"));
		propertyTypes.push_back(linatl::make_pair(MaterialPropertyType::Vector2i, "int2"));
		propertyTypes.push_back(linatl::make_pair(MaterialPropertyType::Vector4, "float4"));
		propertyTypes.push_back(linatl::make_pair(MaterialPropertyType::Mat4, "float4x4"));
		const int typesSize = static_cast<uint32>(propertyTypes.size());

		auto addProperty = [](const String& name, Vector<MaterialPropertyBase*>& outProps, MaterialPropertyType type) {
			auto it = linatl::find_if(outProps.begin(), outProps.end(), [name](MaterialPropertyBase* prop) { return prop->GetName().compare(name) == 0; });

			if (it == outProps.end())
			{
				MaterialPropertyBase* createdProp = MaterialPropertyBase::CreateProperty(type, name);
				outProps.push_back(createdProp);
			}
		};

		auto addTexture = [](const String& name, Vector<MaterialPropertyBase*>& outTextures, MaterialPropertyType type) {
			auto it = linatl::find_if(outTextures.begin(), outTextures.end(), [name](MaterialPropertyBase* prop) { return prop->GetName().compare(name) == 0; });

			if (it == outTextures.end())
			{
				MaterialPropertyBase* createdProp = MaterialPropertyBase::CreateProperty(MaterialPropertyType::Texture, name);
				outTextures.push_back(createdProp);
			}
		};

		while (std::getline(f, line))
		{
			if (!line.empty() && *line.rbegin() == '\r')
				line.erase(line.end() - 1);

			const size_t cmtPos = line.find("//");
			if (cmtPos != std::string::npos && cmtPos == 0)
				continue;

			if (isLicense)
			{
				if (line.find("*/") != std::string::npos)
					isLicense = false;

				continue;
			}

			for (auto& [stage, stageMain] : stages)
			{
				if (line.find(stageMain.c_str()) != std::string::npos)
					outStages[stage] = stageMain;
			}

			const size_t include = line.find(includeText);

			if (include != std::string::npos)
			{
				std::string includeFile = line.substr(include + includeText.size() + 1, line.size() - include - includeText.size() - 1);

				const String str		   = String("Resources/Core/Shaders/Common/") + includeFile.c_str() + ".linashader";
				String		 contents	   = FileSystem::ReadFileContentsAsString(str);
				const size_t endOfComments = contents.find("*/");

				// Remove comments if exists.
				if (endOfComments != std::string::npos)
					contents = contents.substr(endOfComments + 2, contents.size() - endOfComments - 2);

				finalText += contents.c_str();
				continue;
			}

			// First look for pipeline type.
			if (line.find("LINA_PIPELINE_STANDARD") != std::string::npos)
			{
				outPipelineType = PipelineType::Standard;
				continue;
			}
			else if (line.find("LINA_PIPELINE_GUI") != std::string::npos)
			{
				outPipelineType = PipelineType::GUI;
				continue;
			}
			else if (line.find("LINA_PIPELINE_NOVERTEX") != std::string::npos)
			{
				outPipelineType = PipelineType::NoVertex;
				continue;
			}

			// Now look for pass mask.
			if (line.find("LINA_PASS_OPAQUE") != std::string::npos)
			{
				outDrawPassMask.Set(DrawPassMask::Opaque);
				continue;
			}
			else if (line.find("LINA_PASS_TRANSPARENT") != std::string::npos)
			{
				outDrawPassMask.Set(DrawPassMask::Transparent);
				continue;
			}
			else if (line.find("LINA_PASS_SHADOWS") != std::string::npos)
			{
				outDrawPassMask.Set(DrawPassMask::Shadow);
				continue;
			}

			if (!parsingMaterial)
			{
				const size_t material = line.find("LINA_MATERIAL");

				if (material != std::string::npos)
				{
					//std::string replacedLine = line;
					//replacedLine.replace(0, line.size(), "cbuffer MaterialBuffer : register(b3)");
					//finalText += replacedLine + "\n";
					finalText += line;
					parsingMaterial = true;
					continue;
				}

				const std::string txtKeyword = "Texture2D";
				const size_t	  txtBegin	 = line.find(txtKeyword);

				if (txtBegin != std::string::npos)
				{

					const size_t sep = line.find(":");
					std::string textureName = line.substr(txtKeyword.size() + 1, sep - txtKeyword.size() -2 );
					addTexture(textureName.c_str(), outTextures, MaterialPropertyType::Texture);
				}
				

				// bool found = false;
				// for (int i = 0; i < 10; i++)
				//{
				//	const std::string keyword = "LINA_TEXTURE" + std::to_string(i);
				//
				//	if (line.find(keyword) != std::string::npos)
				//	{
				//		std::string textureName = line.substr(keyword.size() + 1, line.size() - keyword.size() - 2);
				//		finalText += "Texture2D " + textureName + " : register(t" + std::to_string(i) + ") \n";
				//		finalText += "SamplerState " + textureName + "Sampler : register(s" + std::to_string(i) + ") \n";
				//		addTexture(textureName.c_str(), outTextures, MaterialPropertyType::Texture);
				//		found = true;
				//		break;
				//	}
				// }
				//
				// if (found)
				//	continue;
			}
			else
			{
				if (line.find("}") != std::string::npos)
				{
					parsingMaterial = false;
				}
				else
				{
					for (int i = 0; i < typesSize; i++)
					{
						const String typekw = propertyTypes[i].second;
						size_t		 pos	= line.find(typekw.c_str());

						if (pos != std::string::npos)
						{
							String name = line.substr(pos + typekw.size() + 1, line.size() - pos - typekw.size() - 2).c_str();
							addProperty(name, outProperties, propertyTypes[i].first);
						}
					}
				}
			}

			finalText += line + "\n";
		}

		return String(finalText.c_str());
	}
} // namespace Lina
