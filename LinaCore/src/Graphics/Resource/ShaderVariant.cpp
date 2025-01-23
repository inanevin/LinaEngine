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

#include "Core/Graphics/Resource/ShaderVariant.hpp"

namespace Lina
{
	void VariantColorTarget::SaveToStream(OStream& stream) const
	{
		stream << format;
	}

	void VariantColorTarget::LoadFromStream(IStream& stream)
	{
		stream >> format;
	}

	void ShaderVariant::SaveToStream(OStream& stream) const
	{
		stream << id;
		stream << blendDisable << depthTest << depthWrite << cullMode << frontFace << depthOp << depthBiasEnable << depthBiasClamp << depthBiasConstant << depthBiasSlope << depthFormat << topology;
		stream << name;
		stream << targets;
		stream << blendSrcFactor << blendDstFactor << blendSrcAlphaFactor << blendDstAlphaFactor << blendColorOp << blendAlphaOp;
		stream << enableSampleShading;
		stream << msaaSamples;
		stream << vertexWrap << fragWrap << renderPass << renderPassName;
		stream << componentFlags;
		stream << depthMode << blendMode;

		const uint32 size = static_cast<uint32>(_compileData.size());
		stream << size;

		for (const LinaGX::ShaderCompileData& data : _compileData)
		{
			const uint32 bsz = static_cast<uint32>(data.outBlob.size);
			stream << data.stage;
			stream << data.text;
			stream << data.includePath;
			stream << bsz;
			stream.WriteRaw(data.outBlob.ptr, data.outBlob.size);
		}
		SaveLinaGXShaderLayout(stream, _outLayout);
	}

	void ShaderVariant::LoadFromStream(IStream& stream)
	{
		stream >> id;
		stream >> blendDisable >> depthTest >> depthWrite >> cullMode >> frontFace >> depthOp >> depthBiasEnable >> depthBiasClamp >> depthBiasConstant >> depthBiasSlope >> depthFormat >> topology;
		stream >> name;
		stream >> targets;
		stream >> blendSrcFactor >> blendDstFactor >> blendSrcAlphaFactor >> blendDstAlphaFactor >> blendColorOp >> blendAlphaOp;
		stream >> enableSampleShading;
		stream >> msaaSamples;
		stream >> vertexWrap >> fragWrap >> renderPass >> renderPassName;
		stream >> componentFlags;
		stream >> depthMode >> blendMode;

		uint32 size = 0;
		stream >> size;

		for (uint32 i = 0; i < size; i++)
		{
			uint32				sz = 0;
			LinaGX::ShaderStage stage;
			LINAGX_STRING		text = "", includePath = "";
			stream >> stage;
			stream >> text;
			stream >> includePath;
			stream >> sz;

			LinaGX::DataBlob blob = {.ptr = nullptr, .size = static_cast<size_t>(sz)};

			if (blob.size != 0)
			{
				blob.ptr = new uint8[blob.size];
				stream.ReadToRaw(blob.ptr, blob.size);
			}

			_compileData.push_back({.stage = stage, .text = text, .includePath = includePath, .outBlob = blob});
		}

		_outLayout = {};
		LoadLinaGXShaderLayout(stream, _outLayout);
	}

} // namespace Lina
