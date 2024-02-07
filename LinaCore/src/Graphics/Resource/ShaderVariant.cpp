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
#include "Common/Serialization/StringSerialization.hpp"

namespace Lina
{
	void ShaderVariant::SaveToStream(OStream& stream) const
	{
		const uint8 targetTypeInt = static_cast<uint8>(targetType);
		const uint8 cullModeInt	  = static_cast<uint8>(cullMode);
		const uint8 frontFaceInt  = static_cast<uint8>(frontFace);
		stream << gpuHandle << blendDisable << depthDisable << targetTypeInt << cullModeInt << frontFaceInt;
		StringSerialization::SaveToStream(stream, name);
	}

	void ShaderVariant::LoadFromStream(IStream& stream)
	{
		uint8 targetTypeInt = 0, cullModeInt = 0, frontFaceInt = 0;
		stream >> gpuHandle >> blendDisable >> depthDisable >> targetTypeInt >> cullModeInt >> frontFaceInt;
		StringSerialization::LoadFromStream(stream, name);
		targetType = static_cast<ShaderWriteTargetType>(targetTypeInt);
		cullMode   = static_cast<LinaGX::CullMode>(cullModeInt);
		frontFace  = static_cast<LinaGX::FrontFace>(frontFaceInt);
	}

} // namespace Lina
