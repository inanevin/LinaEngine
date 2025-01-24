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
#include "Common/System/SystemInfo.hpp"

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
					const String trimmedLine = UtilStr::RemoveWhitespaces(line);
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

				const String lineTrimmed = UtilStr::RemoveWhitespaces(line);

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

		if (!outVertex.empty())
			outVertex.erase(linatl::remove(outVertex.begin(), outVertex.end(), '\r'), outVertex.end());

		if (!outFrag.empty())
			outFrag.erase(linatl::remove(outFrag.begin(), outFrag.end(), '\r'), outFrag.end());

		return true;
	}

	void ShaderPreprocessor::InjectVersionAndExtensions(String& input, bool insertAtHeader)
	{
		const String versionDirective	   = "#version 460 \n";
		const String dynamicIndexDirective = "#extension GL_EXT_nonuniform_qualifier : enable\n";

		if (insertAtHeader)
		{
			input.insert(0, versionDirective);
			input.insert(versionDirective.length(), dynamicIndexDirective);
		}
		else
		{
			input.insert(input.length(), versionDirective);
			input.insert(input.length(), dynamicIndexDirective);
		}
	}

	bool ShaderPreprocessor::InjectMaterialIfRequired(String& input, Vector<ShaderPropertyDefinition>& outProperties)
	{
		outProperties.clear();
		return ProcessMaterialData(input, outProperties);
	}

	void ShaderPreprocessor::InjectUserShader(String& input, const String& shader)
	{
		static String ident = "//##user_shader_injected_here";
		const size_t  pos	= input.find(ident);
		if (pos == String::npos)
			return;

		input.insert(pos, shader);
	}

	String ShaderPreprocessor::MakeVariantBlock(const String& userBlock, const String& variant, const String& renderPassInclude)
	{
		String finalStr = "";
		InjectVersionAndExtensions(finalStr);
		finalStr += "\n";
		finalStr.insert(finalStr.length(), "#include \"Resources/Core/Shaders/Common/GlobalData.linashader\"\n");

		const String inc = "#include \"" + renderPassInclude + "\"\n";
		finalStr.insert(finalStr.length(), inc);
		finalStr += "\n";

		String variantStr = FileSystem::ReadFileContentsAsString(variant);
		finalStr.insert(finalStr.length(), variantStr);
		finalStr += "\n";

		static String ident = "//##user_shader_injected_here";
		const size_t  pos	= finalStr.find(ident);
		if (pos == String::npos)
			return "";
		finalStr.insert(pos, userBlock);

		return finalStr;
	}

	ShaderVariant ShaderPreprocessor::MakeVariant(const String& name, const String& vertexBlock, const String& fragBlock, LinaGX::CullMode cull, BlendMode blend, DepthTesting depth, const Vector<LinaGX::Format>& targets)
	{
		const String includePath = FileSystem::GetRunningDirectory();

		ShaderVariant variant = {
			.id		   = TO_SID(name),
			.name	   = name,
			.cullMode  = cull,
			.frontFace = LinaGX::FrontFace::CW,
			._compileData =
				{
					{
						.stage		 = LinaGX::ShaderStage::Vertex,
						.text		 = vertexBlock,
						.includePath = includePath.c_str(),
					},
					{
						.stage		 = LinaGX::ShaderStage::Fragment,
						.text		 = fragBlock,
						.includePath = includePath.c_str(),
					},
				},
		};

		ApplyBlending(variant, blend);
		ApplyDepth(variant, depth);

		for (LinaGX::Format format : targets)
			variant.targets.push_back({
				.format = format,
			});

		return variant;
	}

	void ShaderPreprocessor::ApplyBlending(ShaderVariant& variant, BlendMode blendMode)
	{
		variant.blendMode = blendMode;
		switch (blendMode)
		{
		case BlendMode::Opaque:
			variant.blendDisable = true;
			break;
		case BlendMode::AlphaBlend:
			variant.blendDisable		= false;
			variant.blendSrcFactor		= LinaGX::BlendFactor::SrcAlpha;
			variant.blendDstFactor		= LinaGX::BlendFactor::OneMinusSrcAlpha;
			variant.blendColorOp		= LinaGX::BlendOp::Add;
			variant.blendSrcAlphaFactor = LinaGX::BlendFactor::One;
			variant.blendDstAlphaFactor = LinaGX::BlendFactor::Zero;
			variant.blendAlphaOp		= LinaGX::BlendOp::Add;
			break;
		case BlendMode::TransparentBlend:
			variant.blendDisable		= false;
			variant.blendSrcFactor		= LinaGX::BlendFactor::SrcAlpha;
			variant.blendDstFactor		= LinaGX::BlendFactor::OneMinusSrcAlpha;
			variant.blendColorOp		= LinaGX::BlendOp::Add;
			variant.blendSrcAlphaFactor = LinaGX::BlendFactor::One;
			variant.blendDstAlphaFactor = LinaGX::BlendFactor::OneMinusSrcAlpha;
			variant.blendAlphaOp		= LinaGX::BlendOp::Add;
			break;
		default:
			break;
		}
	}

	void ShaderPreprocessor::ApplyDepth(ShaderVariant& variant, DepthTesting depth)
	{
		variant.depthMode = depth;

		switch (depth)
		{
		case DepthTesting::None:
			variant.depthTest  = false;
			variant.depthWrite = false;
			break;
		case DepthTesting::Equal:
			variant.depthTest		  = true;
			variant.depthWrite		  = true;
			variant.depthBiasEnable	  = true;
			variant.depthBiasConstant = 5.0f;
			variant.depthOp			  = LinaGX::CompareOp::Equal;
			break;
		case DepthTesting::Less:
			variant.depthTest  = true;
			variant.depthWrite = true;
			variant.depthOp	   = LinaGX::CompareOp::Less;
			break;
		case DepthTesting::Always:
			variant.depthTest  = true;
			variant.depthWrite = true;
			variant.depthOp	   = LinaGX::CompareOp::Always;
			break;
		}
	}

	ShaderType ShaderPreprocessor::GetShaderType(const String& str)
	{
		if (str.find("#lina_shader_deferred") != String::npos)
			return ShaderType::DeferredSurface;

		if (str.find("#lina_shader_sky") != String::npos)
			return ShaderType::Sky;

		if (str.find("#lina_shader_forward") != String::npos)
			return ShaderType::ForwardSurface;

		if (str.find("#lina_shader_gui") != String::npos)
			return ShaderType::GUI;

		return ShaderType::Custom;
	}

	BlendMode ShaderPreprocessor::GetBlendModeFromStr(const String& str)
	{
		if (str.compare("LINA_BLEND_NONE") == 0)
			return BlendMode::Opaque;

		if (str.compare("LINA_BLEND_ALPHA") == 0)
			return BlendMode::AlphaBlend;

		if (str.compare("LINA_BLEND_TRANSPARENT") == 0)
			return BlendMode::TransparentBlend;

		return BlendMode::Opaque;
	}

	DepthTesting ShaderPreprocessor::GetDepthTestingFromStr(const String& str)
	{
		if (str.compare("LINA_DEPTH_NONE") == 0)
			return DepthTesting::None;

		if (str.compare("LINA_DEPTH_ALWAYS") == 0)
			return DepthTesting::Always;

		if (str.compare("LINA_DEPTH_LESS") == 0)
			return DepthTesting::Less;

		if (str.compare("LINA_DEPTH_EQUAL") == 0)
			return DepthTesting::Equal;

		return DepthTesting::None;
	}

	LinaGX::Format ShaderPreprocessor::GetTargetFromStr(const String& str, bool& isMSAA)
	{
		if (str.compare("LINA_TARGET_LDR") == 0)
			return LinaGX::Format::R8G8B8A8_SRGB;

		if (str.compare("LINA_TARGET_D32") == 0)
			return LinaGX::Format::D32_SFLOAT;

		if (str.compare("LINA_TARGET_LDR_MS") == 0)
		{
			isMSAA = true;
			return LinaGX::Format::R8G8B8A8_SRGB;
		}

		if (str.compare("LINA_TARGET_HDR") == 0)
			return SystemInfo::GetHDRFormat();

		if (str.compare("LINA_TARGET_HDR_MS") == 0)
		{
			isMSAA = true;
			return SystemInfo::GetHDRFormat();
		}

		if (str.compare("LINA_TARGET_SWAPCHAIN") == 0)
			return SystemInfo::GetSwapchainFormat();

		if (str.compare("LINA_TARGET_R32U") == 0)
			return LinaGX::Format::R32_UINT;

		return LinaGX::Format::UNDEFINED;
	}

	LinaGX::CullMode ShaderPreprocessor::GetCullFromStr(const String& str)
	{
		if (str.compare("LINA_CULL_BACK") == 0)
			return LinaGX::CullMode::Back;

		if (str.compare("LINA_CULL_FRONT") == 0)
			return LinaGX::CullMode::Front;

		return LinaGX::CullMode::None;
	}

	LinaGX::FrontFace ShaderPreprocessor::GetFaceFromStr(const String& str)
	{
		if (str.compare("LINA_FACE_CW") == 0)
			return LinaGX::FrontFace::CW;

		return LinaGX::FrontFace::CCW;
	}

	void ShaderPreprocessor::ExtractVariants(const String& input, Vector<ShaderVariant>& variants)
	{
		std::istringstream f(input.c_str());
		std::string		   line = "";

		bool parsingVariant = false;
		bool commentBlock	= false;

		ShaderVariant variant = {
			._outLayout			 = {},
			.name				 = "",
			.blendDisable		 = false,
			.blendSrcFactor		 = LinaGX::BlendFactor::SrcAlpha,
			.blendDstFactor		 = LinaGX::BlendFactor::OneMinusSrcAlpha,
			.blendColorOp		 = LinaGX::BlendOp::Add,
			.blendSrcAlphaFactor = LinaGX::BlendFactor::One,
			.blendDstAlphaFactor = LinaGX::BlendFactor::Zero,
			.blendAlphaOp		 = LinaGX::BlendOp::Add,
			.depthTest			 = true,
			.depthWrite			 = true,
			.depthFormat		 = LinaGX::Format::UNDEFINED,
			.targets			 = {},
			.depthOp			 = LinaGX::CompareOp::Less,
			.cullMode			 = LinaGX::CullMode::Back,
			.frontFace			 = LinaGX::FrontFace::CW,
			.topology			 = LinaGX::Topology::TriangleList,
			.depthBiasEnable	 = false,
			.depthBiasConstant	 = 0.0f,
			.depthBiasClamp		 = 0.0f,
			.depthBiasSlope		 = 0.0f,
			.enableSampleShading = false,
			.msaaSamples		 = 1,
		};

		while (std::getline(f, line))
		{
			if (line.empty())
				continue;

			const String lineTrimmed = UtilStr::RemoveWhitespaces(line);

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

			if (parsingVariant)
			{
				const size_t colonPos = lineTrimmed.find(":");

				String lineValue = "";

				lineValue = lineTrimmed.substr(colonPos + 1, lineTrimmed.length() - colonPos + 1);

				if (line.find("#lina_end") != String::npos)
				{
					parsingVariant = false;
					variants.push_back(variant);
					continue;
				}

				if (line.find("#") == String::npos || colonPos == String::npos)
					continue;

				if (line.find("#name") != String::npos)
				{
					variant.name = lineValue;
					continue;
				}

				if (line.find("#blend") != String::npos)
				{
					ApplyBlending(variant, GetBlendModeFromStr(lineValue));
				}

				if (line.find("#depth_target") != String::npos)
				{
					bool isMSAA			= false;
					variant.depthFormat = GetTargetFromStr(lineValue, isMSAA);
				}
				else if (line.find("#depth_bias") != String::npos)
				{
					uint32 outDecimals		  = 0;
					float  bias				  = UtilStr::StringToFloat(lineValue, outDecimals);
					variant.depthBiasEnable	  = true;
					variant.depthBiasConstant = bias;
					// variant.depthBiasClamp	  = bias * 2;
				}
				else if (line.find("#depth") != String::npos)
				{
					ApplyDepth(variant, GetDepthTestingFromStr(lineValue));
				}

				if (line.find("#target") != String::npos)
				{
					const Vector<String> targets = FileSystem::Split(lineValue, ',');
					for (const String& target : targets)
					{
						bool				 isMSAA = false;
						const LinaGX::Format fmt	= GetTargetFromStr(target, isMSAA);

						if (isMSAA)
							variant.msaaSamples = SystemInfo::GetAllowedMSAASamples();

						variant.targets.push_back({
							.format = fmt,
						});
					}
				}

				if (line.find("#cull") != String::npos)
				{
					variant.cullMode = GetCullFromStr(lineValue);
				}

				if (line.find("#face") != String::npos)
				{
					variant.frontFace = GetFaceFromStr(lineValue);
				}

				if (lineValue.empty() || lineValue.compare("\"\"") == 0)
					continue;

				if (line.find("#vertex") != String::npos)
					variant.vertexWrap = lineValue;

				if (line.find("#frag") != String::npos)
					variant.fragWrap = lineValue;

				if (line.find("#pass") != String::npos)
				{
					variant.renderPass = lineValue;

					const size_t us		   = lineTrimmed.find_last_of("_");
					const size_t dot	   = lineTrimmed.find_first_of(".");
					variant.renderPassName = lineTrimmed.substr(us + 1, dot - us - 1);
				}
			}

			if (line.find("#lina_variant") != String::npos)
			{
				parsingVariant = true;
				variant		   = {};
				continue;
			}
		}
	}

} // namespace Lina
