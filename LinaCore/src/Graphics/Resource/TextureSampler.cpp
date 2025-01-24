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

#include "Common/Serialization/Serialization.hpp"
#include "Common/FileSystem/FileSystem.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Application.hpp"

namespace Lina
{
	TextureSampler::~TextureSampler()
	{
	}

	void TextureSampler::SaveToStream(OStream& stream) const
	{
		Resource::SaveToStream(stream);
		stream << VERSION;
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
		Resource::LoadFromStream(stream);
		uint32 version = 0;
		stream >> version;

		uint8 minFilter = 0, magFilter = 0, mode = 0, mipmapMode = 0, borderColor = 0;
		stream >> m_samplerDesc.minLod >> m_samplerDesc.maxLod >> m_samplerDesc.mipLodBias;
		stream >> m_samplerDesc.anisotropy;
		stream >> minFilter >> magFilter >> mode >> mipmapMode >> borderColor;
		m_samplerDesc.minFilter	 = static_cast<LinaGX::Filter>(minFilter);
		m_samplerDesc.magFilter	 = static_cast<LinaGX::Filter>(magFilter);
		m_samplerDesc.mode		 = static_cast<LinaGX::SamplerAddressMode>(mode);
		m_samplerDesc.mipmapMode = static_cast<LinaGX::MipmapMode>(mipmapMode);
	}

	bool TextureSampler::LoadFromFile(const String& path)
	{
		IStream stream = Serialization::LoadFromFile(path.c_str());

		if (stream.GetDataRaw() != nullptr)
			LoadFromStream(stream);
		else
			return false;

		stream.Destroy();
		return true;
	}

	void TextureSampler::GenerateHW()
	{
		LINA_ASSERT(m_hwValid == false, "");

		m_samplerDesc.debugName = m_name.c_str();
		m_gpuHandle				= Application::GetLGX()->CreateSampler(m_samplerDesc);
		m_hwValid				= true;
	}

	void TextureSampler::GenerateHW(const LinaGX::SamplerDesc& desc)
	{
		LINA_ASSERT(m_hwValid == false, "");
		m_samplerDesc = desc;
		m_gpuHandle	  = Application::GetLGX()->CreateSampler(m_samplerDesc);
		m_hwValid	  = true;
	}

	void TextureSampler::DestroyHW()
	{
		LINA_ASSERT(m_hwValid, "");

		Application::GetLGX()->DestroySampler(m_gpuHandle);
		m_hwValid = false;
	}

} // namespace Lina
