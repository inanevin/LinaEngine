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

	void ParseFullShader(const String& text, HashMap<StringID, ShaderVariant>& outVariants, HashMap<LinaGX::ShaderStage, String>& outStages)
	{
		std::istringstream f(text.c_str());
		std::string		   line				= "";
		bool			   isCmt			= false;
		bool			   isParsingVariant = false;
		ShaderVariant	   variantData;

		const String renderpassIdentifier	= "#lina_renderpass";
		const String targetIdentifier		= "#lina_target";
		const String blendIdentifier		= "#lina_blend";
		const String depthIdentifier		= "#lina_depth";
		const String cullIdentifier			= "#lina_cull";
		const String frontIdentifier		= "#lina_front";
		const String endIdentifier			= "#lina_end";
		const String variantBeginIdentifier = "#lina_define_variant";

		while (std::getline(f, line))
		{
			if (!line.empty() && *line.rbegin() == '\r')
				line.erase(line.end() - 1);

			const size_t cmtPos = line.find("//");
			if (cmtPos != std::string::npos && cmtPos == 0)
				continue;

			if (line.find("/*") != std::string::npos)
			{
				isCmt = true;
				continue;
			}

			if (isCmt)
			{
				if (line.find("*/") != std::string::npos)
					isCmt = false;

				continue;
			}

			// size_t renderpassBlock = lineSqueezed.find(renderpassIdentifier.c_str());
			//
			// if (renderpassBlock != String::npos)
			// {
			// 	variantData.renderPassName = lineSqueezed.substr(renderpassIdentifier.size() + 1, lineSqueezed.size() - renderpassIdentifier.size() - 1).c_str();
			//
			// 	if (variantData.renderPassName.compare("basic") == 0)
			// 		variantData.renderPassType = RenderPassDescriptorType::Basic;
			// }

			if (line.find(variantBeginIdentifier.c_str()) != std::string::npos)
			{
				isParsingVariant = true;
				variantData		 = {};

				const String lineSqueezed = FileSystem::RemoveWhitespaces(line.c_str());
				variantData.name		  = lineSqueezed.substr(variantBeginIdentifier.size() + 1, lineSqueezed.size() - variantBeginIdentifier.size() - 1).c_str();
				continue;
			}

			if (isParsingVariant)
			{
				const String lineSqueezed = FileSystem::RemoveWhitespaces(line.c_str());

				if (line.find(endIdentifier.c_str()) != std::string::npos)
				{
					outVariants[TO_SID(variantData.name)] = variantData;
					isParsingVariant					  = false;
					continue;
				}

				size_t targetBlock = lineSqueezed.find(targetIdentifier.c_str());
				size_t blendBlock  = lineSqueezed.find(blendIdentifier.c_str());
				size_t depthBlock  = lineSqueezed.find(depthIdentifier.c_str());
				size_t cullBlock   = lineSqueezed.find(cullIdentifier.c_str());
				size_t frontBlock  = lineSqueezed.find(frontIdentifier.c_str());

				if (targetBlock != String::npos)
				{
					const String target = lineSqueezed.substr(targetIdentifier.size() + 1, lineSqueezed.size() - targetIdentifier.size() - 1).c_str();

					if (target.compare("rendertarget") == 0)
						variantData.targetType = ShaderWriteTargetType::RenderTarget;
					else if (target.compare("swapchain") == 0)
						variantData.targetType = ShaderWriteTargetType::Swapchain;
				}

				if (blendBlock != String::npos)
				{
					const String blend = lineSqueezed.substr(blendIdentifier.size() + 1, lineSqueezed.size() - blendIdentifier.size() - 1).c_str();

					if (blend.compare("disable") == 0)
						variantData.blendDisable = true;
				}

				if (depthBlock != String::npos)
				{
					const String depth = lineSqueezed.substr(depthIdentifier.size() + 1, lineSqueezed.size() - depthIdentifier.size() - 1).c_str();

					if (depth.compare("disable") == 0)
						variantData.depthDisable = true;
				}

				if (cullBlock != String::npos)
				{
					const String cull = lineSqueezed.substr(cullIdentifier.size() + 1, lineSqueezed.size() - cullIdentifier.size() - 1).c_str();

					if (cull.compare("none") == 0)
						variantData.cullMode = LinaGX::CullMode::None;
					else if (cull.compare("front") == 0)
						variantData.cullMode = LinaGX::CullMode::Front;
				}

				if (frontBlock != String::npos)
				{
					const String front = lineSqueezed.substr(frontIdentifier.size() + 1, lineSqueezed.size() - frontIdentifier.size() - 1).c_str();

					if (front.compare("cw") == 0)
						variantData.frontFace = LinaGX::FrontFace::CW;
				}
			}
		}

		if (outVariants.empty())
		{
			ShaderVariant variant = {
				.name = "Default",
			};
			outVariants["Default"_hs] = variant;
		}

		HashMap<LinaGX::ShaderStage, String> blockIdentifiers;
		blockIdentifiers[LinaGX::ShaderStage::Fragment] = "#lina_fs";
		blockIdentifiers[LinaGX::ShaderStage::Vertex]	= "#lina_vs";
		blockIdentifiers[LinaGX::ShaderStage::Compute]	= "#lina_cs";
		blockIdentifiers[LinaGX::ShaderStage::Geometry] = "#lina_gs";

		String		 globalDataInclude = FileSystem::ReadFileContentsAsString("Resources/Core/Shaders/Common/GlobalData.linashader");
		const size_t commentsEnd	   = globalDataInclude.find("*/") + 2;
		globalDataInclude			   = globalDataInclude.substr(commentsEnd, globalDataInclude.size() - commentsEnd);

		// const String renderPassPath = "Resources/Core/Shaders/Common/RenderPass_" + "" String renderPassFile = FileSystem::ReadFileContentsAsString("Reso")

		const String versionDirective	   = "#version 460 \n";
		const String dynamicIndexDirective = "#extension GL_EXT_nonuniform_qualifier : enable";

		for (const auto& [stage, ident] : blockIdentifiers)
		{
			String					  block = ExtractBlock(text.c_str(), ident.c_str(), "#lina_end");
			LinaGX::ShaderCompileData compileData;

			if (!block.empty())
			{
				block.insert(0, versionDirective);
				block.insert(versionDirective.size(), dynamicIndexDirective);
				block.insert(versionDirective.size() + dynamicIndexDirective.size(), globalDataInclude);
				outStages[stage] = block;
			}
		}
	}

	bool ShaderPreprocessor::Preprocess(const String& text, HashMap<LinaGX::ShaderStage, String>& outStages, HashMap<StringID, ShaderVariant>& outVariants)
	{
		if (text.find("#version") != String::npos)
		{
			LINA_ASSERT(false, "Shaders can't have version directives, these are internally injected by Lina!");
			return false;
		}

		ParseFullShader(text, outVariants, outStages);

		return true;
	}
} // namespace Lina
