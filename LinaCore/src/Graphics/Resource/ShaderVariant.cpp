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

#include "Common/Serialization/VectorSerialization.hpp"

namespace Lina
{
	void VariantColorTarget::SaveToStream(OStream& stream) const
	{
		stream << static_cast<uint8>(format);
	}

	void VariantColorTarget::LoadFromStream(IStream& stream)
	{
		uint8 fmt = 0;
		stream >> fmt;
		format = static_cast<LinaGX::Format>(fmt);
	}

	void ShaderVariant::SaveToStream(OStream& stream) const
	{
		const uint8 cullModeInt	   = static_cast<uint8>(cullMode);
		const uint8 frontFaceInt   = static_cast<uint8>(frontFace);
		const uint8 depthOpInt	   = static_cast<uint8>(depthOp);
		const uint8 depthFormatInt = static_cast<uint8>(depthFormat);
		const uint8 topoInt		   = static_cast<uint8>(topology);
		stream << blendDisable << depthTest << depthWrite << cullModeInt << frontFaceInt << depthOpInt << depthBiasEnable << depthBiasClamp << depthBiasConstant << depthBiasSlope << depthFormatInt << topoInt;
		stream << name;
		VectorSerialization::SaveToStream_OBJ(stream, targets);
	}

	void ShaderVariant::LoadFromStream(IStream& stream)
	{
		uint8 cullModeInt = 0, frontFaceInt = 0, depthOpInt = 0, depthFormatInt = 0, topoInt = 8;
		stream >> blendDisable >> depthTest >> depthWrite >> cullModeInt >> frontFaceInt >> depthOpInt >> depthBiasEnable >> depthBiasClamp >> depthBiasConstant >> depthBiasSlope >> depthFormatInt >> topoInt;
		stream >> name;
		VectorSerialization::LoadFromStream_OBJ(stream, targets);
		depthOp		= static_cast<LinaGX::CompareOp>(depthOpInt);
		cullMode	= static_cast<LinaGX::CullMode>(cullModeInt);
		frontFace	= static_cast<LinaGX::FrontFace>(frontFaceInt);
		depthFormat = static_cast<LinaGX::Format>(depthFormatInt);
		topology	= static_cast<LinaGX::Topology>(topoInt);
	}

} // namespace Lina
