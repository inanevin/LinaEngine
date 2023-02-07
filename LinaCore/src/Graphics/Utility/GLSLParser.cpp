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

#include "Graphics/Utility/GLSLParser.hpp"
#include "FileSystem/FileSystem.hpp"
#include "Data/Vector.hpp"
#include "Data/CommonData.hpp"
#include "Graphics/Resource/Shader.hpp"
#include "Graphics/Resource/MaterialProperty.hpp"

namespace Lina
{
	Vector<Pair<String, String>> GLSLParser::s_replacableKeywords = {
		linatl::make_pair("LINA_PROJ", "LINA_CAM.proj"),
		linatl::make_pair("LINA_VIEW", "LINA_CAM.view"),
		linatl::make_pair("LINA_VP", "LINA_CAM.viewProj"),
		linatl::make_pair("LINA_CAMPOS", "LINA_CAM.camPos"),
		linatl::make_pair("LINA_CAMNEAR", "LINA_CAM.camNearFar.x"),
		linatl::make_pair("LINA_CAMFAR", "LINA_CAM.camNearFar.y"),
		linatl::make_pair("LINA_PLIGHT_COUNT", "LINA_SCENE.lightCounts.x"),
		linatl::make_pair("LINA_SLIGHT_COUNT", "LINA_SCENE.lightCounts.y"),
		linatl::make_pair("LINA_AMBIENT", "LINA_SCENE.ambientColor"),
		linatl::make_pair("LINA_VISUALIZEDEPTH", "LINA_DBG.visualizeDepth"),
		linatl::make_pair("LINA_SCREENSIZE", "LINA_GLOBAL.screenSizeMousePos.rg"),
		linatl::make_pair("LINA_MOUSEPOS", "LINA_GLOBAL.screenSizeMousePos.ba"),
		linatl::make_pair("LINA_DT", "LINA_GLOBAL.deltaElapsed.x"),
		linatl::make_pair("LINA_ET", "LINA_GLOBAL.deltaElapsed.y"),
	};

	void GLSLParser::ParseRaw(
		const String& raw, HashMap<ShaderStage, String>& outStages, Vector<UserBinding>& outBindings, Vector<MaterialPropertyBase*>& outProperties, Vector<MaterialPropertyBase*>& outTextures, Bitmask16& outPassMask, PipelineType& outPipelineType)
	{
		std::istringstream f(raw.c_str());
		std::string		   line					= "";
		std::string		   shaderStageTxt		= "";
		ShaderStage		   shaderStage			= ShaderStage::Vertex;
		bool			   appendingShaderStage = false;

		while (std::getline(f, line))
		{
			if (!line.empty() && *line.rbegin() == '\r')
				line.erase(line.end() - 1);

			const size_t cmtPos = line.find("//");
			if (cmtPos != std::string::npos && cmtPos == 0)
				continue;

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
				outPassMask.Set(DrawPassMask::Opaque);
				continue;
			}
			else if (line.find("LINA_PASS_TRANSPARENT") != std::string::npos)
			{
				outPassMask.Set(DrawPassMask::Transparent);
				continue;
			}
			else if (line.find("LINA_PASS_SHADOWS") != std::string::npos)
			{
				outPassMask.Set(DrawPassMask::Shadow);
				continue;
			}

			// Shader stages.
			if (!appendingShaderStage)
			{

				if (line.find("LINA_VS") != std::string::npos)
				{
					appendingShaderStage = true;
					shaderStage			 = ShaderStage::Vertex;
					continue;
				}
				else if (line.find("LINA_FS") != std::string::npos)
				{
					appendingShaderStage = true;
					shaderStage			 = ShaderStage::Fragment;
					continue;
				}
				else if (line.find("LINA_GEO") != std::string::npos)
				{
					appendingShaderStage = true;
					shaderStage			 = ShaderStage::Geometry;
					continue;
				}
				else if (line.find("LINA_COMP") != std::string::npos)
				{
					appendingShaderStage = true;
					shaderStage			 = ShaderStage::Compute;
					continue;
				}
				else if (line.find("LINA_TESE") != std::string::npos)
				{
					appendingShaderStage = true;
					shaderStage			 = ShaderStage::TesellationEval;
					continue;
				}
				else if (line.find("LINA_TESC") != std::string::npos)
				{
					appendingShaderStage = true;
					shaderStage			 = ShaderStage::TesellationControl;
					continue;
				}
			}

			if (appendingShaderStage)
			{
				if (line.find("LINA_END") != std::string::npos)
				{
					outStages[shaderStage] = String(shaderStageTxt.c_str());
					shaderStageTxt		   = "";
					appendingShaderStage   = false;
					continue;
				}

				shaderStageTxt += line + "\n";
			}
		}

		for (auto& [stage, stageInfo] : outStages)
			FillIncludesAndKeywords(stage, stageInfo, outBindings, outProperties, outTextures);
	}

	void GLSLParser::FillIncludesAndKeywords(ShaderStage stage, String& raw, Vector<UserBinding>& outBindings, Vector<MaterialPropertyBase*>& outProperties, Vector<MaterialPropertyBase*>& outTextures)
	{
		std::istringstream f(raw.c_str());
		std::string		   line		   = "";
		const std::string  includeText = "LINA_INCLUDE";
		std::string		   finalText   = "";

		Vector<Pair<MaterialPropertyType, String>> propertyTypes;
		propertyTypes.push_back(linatl::make_pair(MaterialPropertyType::Float, "float"));
		propertyTypes.push_back(linatl::make_pair(MaterialPropertyType::Int, "int"));
		propertyTypes.push_back(linatl::make_pair(MaterialPropertyType::Bool, "bool"));
		propertyTypes.push_back(linatl::make_pair(MaterialPropertyType::Vector2, "vec2"));
		propertyTypes.push_back(linatl::make_pair(MaterialPropertyType::Vector2i, "ivec2"));
		propertyTypes.push_back(linatl::make_pair(MaterialPropertyType::Vector4, "vec4"));
		propertyTypes.push_back(linatl::make_pair(MaterialPropertyType::Mat4, "mat4"));
		const int typesSize		  = static_cast<uint32>(propertyTypes.size());
		bool	  readingMaterial = false;

		auto addBinding = [&](uint32 binding, uint32 dc, ShaderStage stage, DescriptorType type) {
			auto it = linatl::find_if(outBindings.begin(), outBindings.end(), [&](const UserBinding& bd) { return bd.binding == binding; });

			if (it == outBindings.end())
			{
				UserBinding bindingInfo;
				bindingInfo.binding			= binding;
				bindingInfo.descriptorCount = dc;
				bindingInfo.stages.push_back(stage);
				bindingInfo.type = type;
				outBindings.push_back(bindingInfo);
			}
			else
				it->stages.push_back(stage);
		};

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

			if (line.find("LINA_MATERIAL") != std::string::npos)
			{
				finalText += "layout(set = 2, binding = 0) uniform MaterialData";
				addBinding(0, 1, stage, DescriptorType::UniformBuffer);
				readingMaterial = true;
				continue;
			}
			else
			{
				bool found = false;
				for (int i = 0; i < 10; i++)
				{
					const std::string keyword = "LINA_TEXTURE" + std::to_string(i);

					if (line.find(keyword) != std::string::npos)
					{
						std::string textureName = line.substr(keyword.size() + 1, line.size() - keyword.size() - 2);
						finalText += "layout(set = 2, binding = " + std::to_string(i + 1) + ") uniform sampler2D " + textureName + "; \n";
						addBinding(i + 1, 1, stage, DescriptorType::CombinedImageSampler);
						addTexture(textureName.c_str(), outTextures, MaterialPropertyType::Texture);
						found = true;
						break;
					}
				}

				if (found)
					continue;
			}

			if (readingMaterial)
			{
				if (line.find("}") != std::string::npos)
				{
					readingMaterial = false;
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

			// Look for property keywords.
			std::string replacedLine = line;

			for (auto& pair : s_replacableKeywords)
			{
				const size_t pos = replacedLine.find(pair.first.c_str());
				if (pos != std::string::npos)
					replacedLine.replace(pos, pair.first.size(), pair.second.c_str());
			}

			finalText += replacedLine + "\n";
		}

		raw.clear();
		raw = finalText.c_str();
	}

} // namespace Lina
