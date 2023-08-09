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

#include "Platform/LinaGXIncl.hpp"
#include "Serialization/StringSerialization.hpp"

namespace Lina
{

	void SaveStages(OStream& stream, const LINAGX_VEC<LinaGX::ShaderStage>& stages)
	{
		const uint32 stgSize = static_cast<uint32>(stages.size());
		stream << stgSize;

		for (const auto& stg : stages)
		{
			const uint8 stgi = static_cast<uint8>(stg);
			stream << stgi;
		}
	}

	void LoadStages(IStream& stream, LINAGX_VEC<LinaGX::ShaderStage>& stages)
	{
		uint32 stgSize = 0;
		stream >> stgSize;
		stages.clear();
		stages.resize(stgSize);
		for (uint32 k = 0; k < stgSize; k++)
		{
			uint8 stg = 0;
			stream >> stg;
			stages[k] = static_cast<LinaGX::ShaderStage>(stg);
		}
	}

	void SaveSRVTexture2D(OStream& stream, const LinaGX::ShaderSRVTexture2D& txt)
	{
		stream << txt.set << txt.binding << txt.elementSize;
		StringSerialization::SaveToStream(stream, txt.name.c_str());
		SaveStages(stream, txt.stages);
	}

	void LoadSRVTextre2D(IStream& stream, LinaGX::ShaderSRVTexture2D& txt)
	{
		stream >> txt.set >> txt.binding >> txt.elementSize;
		String str = "";
		StringSerialization::LoadFromStream(stream, str);
		txt.name = str.c_str();
		LoadStages(stream, txt.stages);
	}

	void SaveUBOMember(OStream& stream, const LinaGX::ShaderUBOMember& member)
	{
		const uint8 type = static_cast<uint8>(member.type);
		stream << type << member.size << member.offset << member.elementSize << member.arrayStride << member.matrixStride;
		StringSerialization::SaveToStream(stream, member.name.c_str());
	}

	void LoadUBOMember(IStream& stream, LinaGX::ShaderUBOMember& member)
	{
		String str	= "";
		uint8  type = 0;
		stream >> type >> member.size >> member.offset >> member.elementSize >> member.arrayStride >> member.matrixStride;
		StringSerialization::LoadFromStream(stream, str);
		member.name = str.c_str();
		member.type = static_cast<LinaGX::ShaderMemberType>(type);
	}

	void SaveLinaGXShaderLayout(OStream& stream, const LinaGX::ShaderLayout& layout)
	{
		const uint32 szVertexInputs			 = static_cast<uint32>(layout.vertexInputs.size());
		const uint32 szUbos					 = static_cast<uint32>(layout.ubos.size());
		const uint32 szSsbos				 = static_cast<uint32>(layout.ssbos.size());
		const uint32 szCombinedImageSamplers = static_cast<uint32>(layout.combinedImageSamplers.size());
		const uint32 seperateImages			 = static_cast<uint32>(layout.separateImages.size());
		const uint32 szSamplers				 = static_cast<uint32>(layout.samplers.size());
		const uint32 szSetsAndBindings		 = static_cast<uint32>(layout.setsAndBindings.size());
		stream << szVertexInputs << szUbos << szSsbos << szCombinedImageSamplers << seperateImages << szSamplers << szSetsAndBindings;
		stream << layout.totalDescriptors << layout.hasGLDrawID << layout.drawIDBinding;

		for (const auto& vi : layout.vertexInputs)
		{
			const uint8 fmt = static_cast<uint8>(vi.format);
			StringSerialization::SaveToStream(stream, vi.name.c_str());
			stream << vi.location << vi.elements << vi.size << fmt << vi.offset;
		}

		for (const auto& ubo : layout.ubos)
		{
			stream << ubo.set << ubo.binding << ubo.size << ubo.elementSize;
			StringSerialization::SaveToStream(stream, ubo.name.c_str());

			SaveStages(stream, ubo.stages);

			const uint32 membersSz = static_cast<uint32>(ubo.members.size());
			stream << membersSz;

			for (const auto& member : ubo.members)
				SaveUBOMember(stream, member);
		}

		for (const auto& ssbo : layout.ssbos)
		{
			stream << ssbo.set << ssbo.binding << ssbo.isReadOnly;
			StringSerialization::SaveToStream(stream, ssbo.name.c_str());
			SaveStages(stream, ssbo.stages);
		}

		for (const auto& combinedImg : layout.combinedImageSamplers)
			SaveSRVTexture2D(stream, combinedImg);

		for (const auto& combinedImg : layout.separateImages)
			SaveSRVTexture2D(stream, combinedImg);

		for (const auto& samp : layout.samplers)
		{
			stream << samp.set << samp.binding << samp.elementSize;
			StringSerialization::SaveToStream(stream, samp.name.c_str());
			SaveStages(stream, samp.stages);
		}

		for (const auto& [set, bindingVec] : layout.setsAndBindings)
		{
			const uint32 sz = static_cast<uint32>(bindingVec.size());
			stream << set << sz;
			for (auto b : bindingVec)
				stream << b;
		}

		const uint32 membersSz = static_cast<uint32>(layout.constantBlock.members.size());
		stream << layout.constantBlock.size << layout.constantBlock.set << layout.constantBlock.binding << membersSz;

		for (const auto& mem : layout.constantBlock.members)
			SaveUBOMember(stream, mem);

		StringSerialization::SaveToStream(stream, layout.constantBlock.name.c_str());
		SaveStages(stream, layout.constantBlock.stages);
	}

	void LoadLinaGXShaderLayout(IStream& stream, LinaGX::ShaderLayout& layout)
	{
		uint32 szVertexInputs		   = 0;
		uint32 szUbos				   = 0;
		uint32 szSsbos				   = 0;
		uint32 szCombinedImageSamplers = 0;
		uint32 szSeperateImages		   = 0;
		uint32 szSamplers			   = 0;
		uint32 szSetsAndBindings	   = 0;
		stream >> szVertexInputs >> szUbos >> szSsbos >> szCombinedImageSamplers >> szSeperateImages >> szSamplers >> szSetsAndBindings;
		stream >> layout.totalDescriptors >> layout.hasGLDrawID >> layout.drawIDBinding;

		for (uint32 i = 0; i < szVertexInputs; i++)
		{
			LinaGX::ShaderStageInput vi;
			uint8					 fmt = 0;
			String					 str = "";
			StringSerialization::LoadFromStream(stream, str);
			vi.name = str.c_str();
			stream >> vi.location >> vi.elements >> vi.size >> fmt >> vi.offset;
			vi.format = static_cast<LinaGX::Format>(fmt);
			layout.vertexInputs.push_back(vi);
		}

		for (uint32 i = 0; i < szUbos; i++)
		{
			LinaGX::ShaderUBO ubo;
			stream >> ubo.set >> ubo.binding >> ubo.size >> ubo.elementSize;
			String str = "";
			StringSerialization::LoadFromStream(stream, str);
			ubo.name = str.c_str();

			LoadStages(stream, ubo.stages);

			uint32 membersSz = 0;
			stream >> membersSz;

			ubo.members.resize(membersSz);

			for (uint32 k = 0; k < membersSz; k++)
			{
				LinaGX::ShaderUBOMember member;
				LoadUBOMember(stream, member);
				ubo.members[k] = member;
			}
			layout.ubos.push_back(ubo);
		}

		for (uint32 i = 0; i < szSsbos; i++)
		{
			LinaGX::ShaderSSBO ssbo;
			stream >> ssbo.set >> ssbo.binding >> ssbo.isReadOnly;
			String str = "";
			StringSerialization::LoadFromStream(stream, str);
			LoadStages(stream, ssbo.stages);
			ssbo.name = str.c_str();
			layout.ssbos.push_back(ssbo);
		}

		for (uint32 i = 0; i < szCombinedImageSamplers; i++)
		{
			LinaGX::ShaderSRVTexture2D txt;
			LoadSRVTextre2D(stream, txt);
			layout.combinedImageSamplers.push_back(txt);
		}

		for (uint32 i = 0; i < szSeperateImages; i++)
		{
			LinaGX::ShaderSRVTexture2D txt;
			LoadSRVTextre2D(stream, txt);
			layout.separateImages.push_back(txt);
		}

		for (uint32 i = 0; i < szSamplers; i++)
		{
			LinaGX::ShaderSampler samp;
			stream >> samp.set >> samp.binding >> samp.elementSize;
			String str = "";
			StringSerialization::LoadFromStream(stream, str);
			samp.name = str.c_str();
			LoadStages(stream, samp.stages);
			layout.samplers.push_back(samp);
		}

		for (uint32 i = 0; i < szSetsAndBindings; i++)
		{
			uint32 sz  = 0;
			uint32 set = 0;
			stream >> set >> sz;
			LINAGX_VEC<uint32> bindings;
			bindings.resize(sz);

			for (uint32 k = 0; k < sz; k++)
				stream >> bindings[k];

			layout.setsAndBindings[set] = bindings;
		}

		uint32 membersSz = 0;
		stream >> layout.constantBlock.size >> layout.constantBlock.set >> layout.constantBlock.binding >> membersSz;

		for (uint32 i = 0; i < membersSz; i++)
		{
			LinaGX::ShaderUBOMember member;
			LoadUBOMember(stream, member);
			layout.constantBlock.members.push_back(member);
		}
		String str = "";
		StringSerialization::LoadFromStream(stream, str);
		layout.constantBlock.name = str.c_str();
		LoadStages(stream, layout.constantBlock.stages);
	}
} // namespace Lina
