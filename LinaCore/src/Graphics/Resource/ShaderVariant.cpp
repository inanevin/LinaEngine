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
		stream << blendDisable << depthTest << depthWrite << cullMode << frontFace << depthOp << depthBiasEnable << depthBiasClamp << depthBiasConstant << depthBiasSlope << depthFormat << topology;
		stream << name;
		stream << targets;

		const uint32 size = static_cast<uint32>(_outCompiledBlobs.size());
		stream << size;

		for (const auto& [stage, blob] : _outCompiledBlobs)
		{
			stream << stage;
			stream << static_cast<uint32>(blob.size);
			stream.WriteRawEndianSafe(blob.ptr, blob.size);
		}
		SaveLinaGXShaderLayout(stream, _outLayout);

		const uint32 compileDataSize = static_cast<uint32>(_compileData.size());
		stream << compileDataSize;

		for (const auto& [stage, data] : _compileData)
		{
			stream << stage;
			stream << data.text;
			stream << data.includePath;
		}
	}

	void ShaderVariant::LoadFromStream(IStream& stream)
	{
		stream >> blendDisable >> depthTest >> depthWrite >> cullMode >> frontFace >> depthOp >> depthBiasEnable >> depthBiasClamp >> depthBiasConstant >> depthBiasSlope >> depthFormat >> topology;
		stream >> name;
		stream >> targets;

		uint32 size = 0;
		stream >> size;

		for (uint32 i = 0; i < size; i++)
		{
			uint32				sz = 0;
			LinaGX::ShaderStage stage;
			stream >> stage;
			stream >> sz;

			LinaGX::DataBlob blob = {.ptr = nullptr, .size = static_cast<size_t>(sz)};

			if (blob.size != 0)
			{
				blob.ptr = new uint8[blob.size];
				stream.ReadToRawEndianSafe(blob.ptr, blob.size);
			}

			_outCompiledBlobs[stage] = blob;
		}

		_outLayout = {};
		LoadLinaGXShaderLayout(stream, _outLayout);

		uint32 compileDataSz = 0;
		stream >> compileDataSz;

		for (uint32 i = 0; i < compileDataSz; i++)
		{
			LinaGX::ShaderStage stage = {};
			stream >> stage;
			auto& data = _compileData[stage];
			stream >> data.text;
			stream >> data.includePath;
		}
	}

} // namespace Lina
