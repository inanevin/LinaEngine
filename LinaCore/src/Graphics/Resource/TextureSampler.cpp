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

#include "Core/Graphics/Resource/TextureSampler.hpp"
#include "Core/Graphics/GfxManager.hpp"
#include "Common/System/System.hpp"
#include "Core/Resources/ResourceManager.hpp"

namespace Lina
{
	TextureSampler::~TextureSampler()
	{
		DestroyHW();
	}

	void TextureSampler::SaveToStream(OStream& stream) const
	{
		stream << VERSION;
		stream << m_id;

		const uint8 minFilter	= static_cast<uint8>(m_samplerDesc.minFilter);
		const uint8 magFilter	= static_cast<uint8>(m_samplerDesc.magFilter);
		const uint8 mode		= static_cast<uint8>(m_samplerDesc.mode);
		const uint8 mipmapMode	= static_cast<uint8>(m_samplerDesc.mipmapMode);
		const uint8 borderColor = static_cast<uint8>(m_samplerDesc.borderColor);
		stream << m_samplerDesc.minLod << m_samplerDesc.maxLod << m_samplerDesc.mipLodBias;
		stream << m_samplerDesc.anisotropy;
		stream << minFilter << magFilter << mode << mipmapMode << borderColor;
	}

	void TextureSampler::LoadFromStream(IStream& stream)
	{
		uint32 version = 0;
		stream >> version;
		stream >> m_id;

		uint8 minFilter = 0, magFilter = 0, mode = 0, mipmapMode = 0, borderColor = 0;
		stream >> m_samplerDesc.minLod >> m_samplerDesc.maxLod >> m_samplerDesc.mipLodBias;
		stream >> m_samplerDesc.anisotropy;
		stream >> minFilter >> magFilter >> mode >> mipmapMode >> borderColor;
		m_samplerDesc.minFilter	 = static_cast<LinaGX::Filter>(minFilter);
		m_samplerDesc.magFilter	 = static_cast<LinaGX::Filter>(magFilter);
		m_samplerDesc.mode		 = static_cast<LinaGX::SamplerAddressMode>(mode);
		m_samplerDesc.mipmapMode = static_cast<LinaGX::MipmapMode>(mipmapMode);
	}

	void TextureSampler::GenerateHW()
	{
		m_samplerDesc.debugName = m_path.c_str();
		m_gpuHandle				= GfxManager::GetLGX()->CreateSampler(m_samplerDesc);
		m_hwExists				= true;
	}

	void TextureSampler::GenerateHW(const LinaGX::SamplerDesc& desc)
	{
		m_samplerDesc = desc;
		m_gpuHandle	  = GfxManager::GetLGX()->CreateSampler(m_samplerDesc);
		m_hwExists	  = true;
	}

	void TextureSampler::DestroyHW()
	{
		if (!m_hwExists)
			return;
		GfxManager::GetLGX()->DestroySampler(m_gpuHandle);
		m_hwExists = false;
	}

} // namespace Lina
