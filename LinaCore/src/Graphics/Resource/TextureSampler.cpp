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
#include "Graphics/Platform/RendererIncl.hpp"
#include "Graphics/Core/GfxManager.hpp"
#include "System/ISystem.hpp"
#include "Resources/Core/ResourceManager.hpp"

namespace Lina
{
	TextureSampler::TextureSampler(ResourceManager* rm, bool isUserManaged, const String& path, StringID sid) : IResource(rm, isUserManaged, path, sid, GetTypeID<TextureSampler>())
	{
		m_renderer = rm->GetSystem()->CastSubsystem<GfxManager>(SubsystemType::GfxManager)->GetRenderer();
	}

	TextureSampler::~TextureSampler()
	{
		m_renderer->DestroySampler(m_gpuHandle);
	}

	void TextureSampler::SetSamplerData(const SamplerData& data)
	{
		m_samplerData = data;
		Upload();
	}

	void TextureSampler::Upload()
	{
		m_renderer->GenerateSampler(this);
	}

	void TextureSampler::SaveToStream(OStream& stream)
	{
		const uint8 minFilter = static_cast<uint8>(m_samplerData.minFilter);
		const uint8 magFilter = static_cast<uint8>(m_samplerData.magFilter);
		const uint8 mode	  = static_cast<uint8>(m_samplerData.mode);
		stream << m_samplerData.minLod << m_samplerData.maxLod << m_samplerData.mipLodBias;
		stream << m_samplerData.anisotropy;
		stream << minFilter << magFilter << mode;
		m_samplerData.borderColor.SaveToStream(stream);
	}

	void TextureSampler::LoadFromStream(IStream& stream)
	{
		uint8 minFilter = 0, magFilter = 0, mode = 0, mipmapFilter = 0, mipmapMode = 0, borderColor = 0;
		stream >> m_samplerData.minLod >> m_samplerData.maxLod >> m_samplerData.mipLodBias;
		stream >> m_samplerData.anisotropy;
		stream >> minFilter >> magFilter >> mode >> mipmapFilter >> mipmapMode >> borderColor;
		m_samplerData.minFilter = static_cast<Filter>(minFilter);
		m_samplerData.magFilter = static_cast<Filter>(magFilter);
		m_samplerData.mode		= static_cast<SamplerAddressMode>(mode);
		m_samplerData.borderColor.LoadFromStream(stream);
	}

} // namespace Lina
