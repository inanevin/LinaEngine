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

#include "Common/Platform/LinaGXIncl.hpp"

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

	void SaveMembers(OStream& stream, const LINAGX_VEC<LinaGX::ShaderStructMember>& members)
	{
		stream << static_cast<int32>(members.size());

		for (const auto& member : members)
		{
			stream << static_cast<uint8>(member.type);
			stream << static_cast<int32>(member.size);
			stream << static_cast<int32>(member.offset);
			stream << static_cast<int32>(member.alignment);
			stream << String(member.name.c_str());
			stream << member.elementSize;
			stream << static_cast<int32>(member.arrayStride);
			stream << static_cast<int32>(member.matrixStride);
		}
	}

	void LoadMembers(IStream& stream, LINAGX_VEC<LinaGX::ShaderStructMember>& members)
	{
		int32 sz = 0;
		stream >> sz;
		members.clear();
		members.resize(sz);

		for (int32 i = 0; i < sz; i++)
		{
			auto& member  = members[i];
			uint8 typeInt = 0;
			int32 mSize = 0, offset = 0, alignment = 0, arrayStride = 0, matrixStride = 0;

			String mName = "";
			stream >> typeInt;
			stream >> mSize;
			stream >> offset;
			stream >> alignment;
			stream >> mName;
			stream >> member.elementSize;
			stream >> arrayStride;
			stream >> matrixStride;
			member.name = LINAGX_STRING(mName.c_str());

			member.type			= static_cast<LinaGX::ShaderMemberType>(typeInt);
			member.size			= static_cast<size_t>(mSize);
			member.offset		= static_cast<size_t>(offset);
			member.alignment	= static_cast<size_t>(alignment);
			member.arrayStride	= static_cast<size_t>(arrayStride);
			member.matrixStride = static_cast<size_t>(matrixStride);
		}
	}

	void SaveConstantBlock(OStream& stream, const LinaGX::ShaderConstantBlock& block)
	{
		stream << static_cast<uint32>(block.size);
		stream << block.set;
		stream << block.binding;
		SaveMembers(stream, block.members);
		SaveStages(stream, block.stages);
		stream << String(block.name.c_str());

		stream << static_cast<int32>(block.isActive.size());

		for (const auto& [stg, act] : block.isActive)
		{
			stream << static_cast<uint8>(stg);
			stream << act;
		}
	}

	void LoadConstantBlock(IStream& stream, LinaGX::ShaderConstantBlock& block)
	{
		uint32 bSize = 0;
		stream >> bSize;
		block.size = static_cast<size_t>(bSize);
		stream >> block.set;
		stream >> block.binding;
		LoadMembers(stream, block.members);
		LoadStages(stream, block.stages);

		String name = "";
		stream >> name;
		block.name = name.c_str();

		int32 sz = 0;
		stream >> sz;

		for (int32 i = 0; i < sz; i++)
		{
			uint8 stg	 = 0;
			bool  active = false;
			stream >> stg;
			stream >> active;
			block.isActive.push_back(linatl::make_pair(static_cast<LinaGX::ShaderStage>(stg), active));
		}
	}

	void SaveDescriptorSetBinding(OStream& stream, const LinaGX::ShaderDescriptorSetBinding& binding)
	{
		stream << static_cast<uint8>(binding.type);
		stream << String(binding.name.c_str());

		SaveStages(stream, binding.stages);
		SaveMembers(stream, binding.structMembers);

		stream << static_cast<int32>(binding.isActive.size());

		for (const auto& [stg, act] : binding.isActive)
		{
			stream << static_cast<uint8>(stg);
			stream << act;
		}

		stream << static_cast<int32>(binding.mslBufferID.size());

		for (const auto& [stg, id] : binding.mslBufferID)
		{
			stream << static_cast<uint8>(stg);
			stream << id;
		}

		stream << binding.spvID;
		stream << binding.descriptorCount;
		stream << static_cast<uint32>(binding.size);
		stream << binding.isWritable;
		stream << binding.isArrayType;
	}

	void LoadDescriptorSetBinding(IStream& stream, LinaGX::ShaderDescriptorSetBinding& binding)
	{
		String bName		  = "";
		uint8  bindingTypeInt = 0;
		stream >> bindingTypeInt;
		binding.type = static_cast<LinaGX::DescriptorType>(bindingTypeInt);
		stream >> bName;
		binding.name = bName.c_str();

		LoadStages(stream, binding.stages);
		LoadMembers(stream, binding.structMembers);

		int32 isActiveSz = 0;
		stream >> isActiveSz;

		for (int32 i = 0; i < isActiveSz; i++)
		{
			uint8 stg = 0;
			bool  act = false;
			stream >> stg;
			stream >> act;
			binding.isActive.push_back({static_cast<LinaGX::ShaderStage>(stg), act});
		}

		int32 mslBufferSz = 0;
		stream >> mslBufferSz;

		for (int32 i = 0; i < mslBufferSz; i++)
		{
			uint8  stg = 0;
			uint32 id  = 0;
			stream >> stg;
			stream >> id;
			binding.mslBufferID.push_back({static_cast<LinaGX::ShaderStage>(stg), id});
		}

		uint32 bindingSz = 0;
		stream >> binding.spvID;
		stream >> binding.descriptorCount;
		stream >> bindingSz;
		stream >> binding.isWritable;
		stream >> binding.isArrayType;
		binding.size = static_cast<size_t>(bindingSz);
	}

	void SaveLinaGXShaderLayout(OStream& stream, const LinaGX::ShaderLayout& layout)
	{
		stream << static_cast<int32>(layout.vertexInputs.size());
		stream << static_cast<int32>(layout.descriptorSetLayouts.size());
		stream << static_cast<int32>(layout.constants.size());
		stream << static_cast<int32>(layout.constantsMSLBuffers.size());
		stream << static_cast<int32>(layout.entryPoints.size());
		stream << static_cast<int32>(layout.mslMaxBufferIDs.size());

		for (const auto& vi : layout.vertexInputs)
		{
			stream << String(vi.name.c_str());
			stream << vi.location << vi.elements << static_cast<uint32>(vi.size) << static_cast<uint8>(vi.format) << static_cast<uint32>(vi.offset);
		}

		for (const auto& dsl : layout.descriptorSetLayouts)
		{
			stream << static_cast<int32>(dsl.bindings.size());

			for (const auto& b : dsl.bindings)
				SaveDescriptorSetBinding(stream, b);
		}

		for (const auto& c : layout.constants)
			SaveConstantBlock(stream, c);

		for (const auto& [stg, buf] : layout.constantsMSLBuffers)
		{
			stream << static_cast<uint8>(stg);
			stream << buf;
		}

		for (const auto& [stg, ep] : layout.entryPoints)
		{
			stream << static_cast<uint8>(stg);
			stream << String(ep.c_str());
		}

		for (const auto& [stg, id] : layout.mslMaxBufferIDs)
		{
			stream << static_cast<uint8>(stg);
			stream << id;
		}

		stream << layout.constantsSet;
		stream << layout.constantsBinding;
		stream << layout.totalDescriptors;
		stream << layout.hasGLDrawID;
		stream << layout.drawIDBinding;
	}

	void LoadLinaGXShaderLayout(IStream& stream, LinaGX::ShaderLayout& layout)
	{
		int32 viSz = 0, dslSz = 0, cSz = 0, cmslSz = 0, epSz = 0, maxBufSz = 0;
		stream >> viSz >> dslSz >> cSz >> cmslSz >> epSz >> maxBufSz;
		layout.vertexInputs.clear();
		layout.descriptorSetLayouts.clear();
		layout.constants.clear();
		layout.constantsMSLBuffers.clear();
		layout.entryPoints.clear();
		layout.vertexInputs.resize(viSz);
		layout.descriptorSetLayouts.resize(dslSz);
		layout.constants.resize(cSz);

		for (int32 i = 0; i < viSz; i++)
		{
			auto& vi = layout.vertexInputs[i];

			String name = "";
			stream >> name;
			vi.name = name.c_str();

			uint8  fmt = 0;
			uint32 sz = 0, offset = 0;
			stream >> vi.location >> vi.elements >> sz >> fmt >> offset;
			vi.size	  = static_cast<size_t>(sz);
			vi.offset = static_cast<size_t>(offset);
			vi.format = static_cast<LinaGX::Format>(fmt);
		}

		for (int32 i = 0; i < dslSz; i++)
		{
			auto& dsl = layout.descriptorSetLayouts[i];
			int32 sz  = 0;
			stream >> sz;

			dsl.bindings.resize(sz);

			for (int32 j = 0; j < sz; j++)
				LoadDescriptorSetBinding(stream, dsl.bindings[j]);
		}

		for (int32 i = 0; i < cSz; i++)
			LoadConstantBlock(stream, layout.constants[i]);

		for (int32 i = 0; i < cmslSz; i++)
		{
			uint8  stg = 0;
			uint32 buf;
			stream >> stg;
			stream >> buf;
			layout.constantsMSLBuffers.push_back({static_cast<LinaGX::ShaderStage>(stg), buf});
		}

		for (int32 i = 0; i < epSz; i++)
		{
			uint8 stg = 0;
			stream >> stg;
			String ep = "";
			stream >> ep;
			layout.entryPoints.push_back({static_cast<LinaGX::ShaderStage>(stg), ep});
		}

		for (int32 i = 0; i < maxBufSz; i++)
		{
			uint8  stg = 0;
			uint32 id  = 0;
			stream >> stg;
			stream >> id;
			layout.mslMaxBufferIDs.push_back({static_cast<LinaGX::ShaderStage>(stg), id});
		}

		stream >> layout.constantsSet;
		stream >> layout.constantsBinding;
		stream >> layout.totalDescriptors;
		stream >> layout.hasGLDrawID;
		stream >> layout.drawIDBinding;
	}
} // namespace Lina
