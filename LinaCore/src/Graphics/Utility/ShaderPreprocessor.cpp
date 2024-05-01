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

	void ParseFullShader(const String& text, HashMap<LinaGX::ShaderStage, String>& outStages, const RenderPassDescriptorType& rpType)
	{
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

		String rpText = "basic";
		// Check RPType

		const String renderPassPath	   = String("Resources/Core/Shaders/Common/RenderPass_") + rpText + ".linashader";
		const String globalDataInclude = getInclude("Resources/Core/Shaders/Common/GlobalData.linashader");
		const String renderPassInclude = getInclude(renderPassPath.c_str());

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
				block.insert(versionDirective.size() + dynamicIndexDirective.size() + globalDataInclude.size(), renderPassInclude);
				outStages[stage] = block;
			}
		}
	}

	bool ShaderPreprocessor::Preprocess(const String& text, HashMap<LinaGX::ShaderStage, String>& outStages, const RenderPassDescriptorType& rpType)
	{
		if (text.find("#version") != String::npos)
		{
			LINA_ASSERT(false, "Shaders can't have version directives, these are internally injected by Lina!");
			return false;
		}

		ParseFullShader(text, outStages, rpType);

		return true;
	}
} // namespace Lina
