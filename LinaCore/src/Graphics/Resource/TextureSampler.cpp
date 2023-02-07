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

#include "Graphics/Resource/TextureSampler.hpp"

namespace Lina
{
	void TextureSampler::SaveToStream(OStream& stream)
	{
		const uint8 minFilter	 = static_cast<uint8>(m_samplerData.minFilter);
		const uint8 magFilter	 = static_cast<uint8>(m_samplerData.magFilter);
		const uint8 mode		 = static_cast<uint8>(m_samplerData.mode);
		const uint8 mipmapFilter = static_cast<uint8>(m_samplerData.mipmapFilter);
		const uint8 mipmapMode	 = static_cast<uint8>(m_samplerData.mipmapMode);
		const uint8 borderColor	 = static_cast<uint8>(m_samplerData.borderColor);
		stream << m_samplerData.minLod << m_samplerData.maxLod << m_samplerData.mipLodBias;
		stream << m_samplerData.anisotropy;
		stream << m_samplerData.anisotropyEnabled;
		stream << minFilter << magFilter << mode << mipmapFilter << mipmapMode << borderColor;
	}

	void TextureSampler::LoadFromStream(IStream& stream)
	{
		uint8 minFilter = 0, magFilter = 0, mode = 0, mipmapFilter = 0, mipmapMode = 0, borderColor = 0;
		stream >> m_samplerData.minLod >> m_samplerData.maxLod >> m_samplerData.mipLodBias;
		stream >> m_samplerData.anisotropy;
		stream >> m_samplerData.anisotropyEnabled;
		stream >> minFilter >> magFilter >> mode >> mipmapFilter >> mipmapMode >> borderColor;
		m_samplerData.minFilter	   = static_cast<Filter>(minFilter);
		m_samplerData.magFilter	   = static_cast<Filter>(magFilter);
		m_samplerData.mode		   = static_cast<SamplerAddressMode>(mode);
		m_samplerData.mipmapFilter = static_cast<MipmapFilter>(mipmapFilter);
		m_samplerData.mipmapMode   = static_cast<MipmapMode>(mipmapMode);
		m_samplerData.borderColor  = static_cast<BorderColor>(borderColor);
	}
} // namespace Lina
