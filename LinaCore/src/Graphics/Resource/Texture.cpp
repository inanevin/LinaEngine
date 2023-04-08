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

#include "Graphics/Resource/Texture.hpp"
#include "Math/Math.hpp"
#include "Resources/Core/ResourceManager.hpp"
#include "System/ISystem.hpp"
#include "Graphics/Core/GfxManager.hpp"
#include "Graphics/Platform/RendererIncl.hpp"
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "Graphics/Utility/stb/stb_image.h"
#include "Graphics/Utility/stb/stb_image_resize.h"

namespace Lina
{
	Texture::Texture(ResourceManager* rm, bool isUserManaged, const String& path, StringID sid) : IResource(rm, isUserManaged, path, sid, GetTypeID<Texture>())
	{
		m_renderer	   = rm->GetSystem()->CastSubsystem<GfxManager>(SubsystemType::GfxManager)->GetRenderer();
		m_sampler	   = nullptr; // will be loaded later
		m_resourceType = TextureResourceType::Texture2DDefault;
	}

	Texture::Texture(ResourceManager* rm, const UserGeneratedTextureData& textureData) : IResource(rm, true, textureData.path, textureData.sid, GetTypeID<Texture>())
	{
		m_renderer				   = rm->GetSystem()->CastSubsystem<GfxManager>(SubsystemType::GfxManager)->GetRenderer();
		m_extent				   = textureData.extent;
		m_channels				   = textureData.channels;
		m_destroyPixelsAfterUpload = textureData.destroyPixelBufferAfterUpload;
		m_samplerSID			   = textureData.targetSampler == 0 ? DEFAULT_SAMPLER_SID : textureData.targetSampler;
		m_sampler				   = nullptr; // will be loaded later
		m_resourceType			   = textureData.resourceType;
		CheckFormat(m_channels);
		m_metadata.SetUInt8("Format"_hs, static_cast<uint8>(textureData.format));
		m_metadata.SetUInt8("ImageTiling"_hs, static_cast<uint8>(textureData.tiling));

		if (textureData.createPixelBuffer)
		{
			m_pixelsLoadedFromStream = true;
			const size_t sz			 = m_extent.width * m_extent.height * m_channels;
			m_pixels				 = new unsigned char[sz];
			memset(m_pixels, 0, sz);
		}
	}

	Texture::~Texture()
	{
		if (m_pixels)
			delete[] m_pixels;

		m_pixels = nullptr;

		if (m_gpuResource == nullptr)
			return;

		m_renderer->DestroyImage(this);
	}

	void Texture::LoadFromFile(const char* path)
	{
		// Populate metadata.
		m_metadata.GetUInt8("Format"_hs, static_cast<uint8>(Format::R8G8B8A8_SRGB));
		m_metadata.GetBool("GenerateMipmaps"_hs, true);
		m_metadata.GetUInt8("MipmapFilter"_hs, static_cast<uint8>(MipmapFilter::Mitchell));
		m_metadata.GetUInt8("MipmapMode"_hs, static_cast<uint8>(MipmapMode::Linear));
		m_metadata.GetBool("isLinear"_hs, false);
		m_metadata.GetUInt8("ImageTiling"_hs, static_cast<uint8>(ImageTiling::Optimal));
		m_samplerSID = m_metadata.GetSID("Sampler"_hs, DEFAULT_SAMPLER_SID);

		int texWidth, texHeight, texChannels;
		m_pixels		= stbi_load(path, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		m_mipLevels		= m_metadata.GetBool("GenerateMipmaps"_hs) == 1 ? static_cast<uint32>(Math::FloorLog2(Math::Max(texWidth, texHeight))) + 1 : 1;
		m_channels		= 4;
		m_extent.width	= static_cast<uint32>(texWidth);
		m_extent.height = static_cast<uint32>(texHeight);
		m_extent.depth	= 1;
		CheckFormat(m_channels);
		GenerateMipmaps();
	}

	void Texture::SaveToStream(OStream& stream)
	{
		m_metadata.SaveToStream(stream);
		stream << m_mipLevels << m_channels << m_extent.width << m_extent.height << m_extent.depth;

		// Pixels.
		const uint32 pixelsSize = m_channels * m_extent.width * m_extent.height;
		stream << pixelsSize;
		stream.WriteEndianSafe(m_pixels, pixelsSize);

		// Mips
		const uint32 mipCount = static_cast<uint32>(m_mipmaps.size());
		stream << mipCount;

		for (auto& mm : m_mipmaps)
		{
			const uint32 mmPixelsSize = m_channels * mm.width * mm.height;
			stream << mm.width << mm.height << mmPixelsSize;

			if (mmPixelsSize != 0)
				stream.WriteEndianSafe(mm.pixels, mmPixelsSize);
		}

		stream << m_samplerSID;
	}

	void Texture::LoadFromStream(IStream& stream)
	{
		m_metadata.LoadFromStream(stream);
		stream >> m_mipLevels >> m_channels >> m_extent.width >> m_extent.height >> m_extent.depth;

		// Pixels.
		uint32 pixelsSize = 0;
		stream >> pixelsSize;
		m_pixels				 = new unsigned char[pixelsSize];
		m_pixelsLoadedFromStream = true;
		stream.ReadEndianSafe(m_pixels, pixelsSize);

		// Mips
		uint32 mipCount = 0;
		stream >> mipCount;
		for (uint32 i = 0; i < mipCount; i++)
		{
			Mipmap mm			= {};
			uint32 mmPixelsSize = 0;
			stream >> mm.width >> mm.height >> mmPixelsSize;

			if (mmPixelsSize != 0)
			{
				mm.pixels = new unsigned char[mmPixelsSize];
				stream.ReadEndianSafe(mm.pixels, mmPixelsSize);
			}

			m_mipmaps.push_back(mm);
		}

		stream >> m_samplerSID;
	}

	void Texture::Upload()
	{
		if (m_pixels == nullptr)
		{
			LINA_ERR("[Texture] -> Texture pixel data is empty but Upload is called!");
			return;
		}

		ImageGenerateRequest req;

		if (m_destroyPixelsAfterUpload)
		{
			req.onGenerated = [this]() {
				if (m_pixelsLoadedFromStream)
					delete[] m_pixels;
				else
					stbi_image_free(m_pixels);

				for (auto& mm : m_mipmaps)
					delete[] mm.pixels;

				m_pixels = nullptr;
				m_mipmaps.clear();
			};
		}

		m_renderer->GenerateImage(this, req);
	}

	void Texture::BatchLoaded()
	{
		SetSampler(m_samplerSID);
	}

	void Texture::SetSampler(StringID samplerSID)
	{
		m_samplerSID = samplerSID;
		m_sampler	 = m_resourceManager->GetResource<TextureSampler>(m_samplerSID);
	}

	TextureSampler* Texture::GetSampler()
	{
		if (m_sampler == nullptr)
			m_sampler = m_resourceManager->GetResource<TextureSampler>(m_samplerSID);

		return m_sampler;
	}

	void Texture::CheckFormat(int channels)
	{
		if (channels == 2)
			m_metadata.SetUInt8("Format"_hs, static_cast<uint8>(Format::R8G8_UNORM));
		else if (channels == 1)
			m_metadata.SetUInt8("Format"_hs, static_cast<uint8>(Format::R8_UNORM));
		else
			m_metadata.SetUInt8("Format"_hs, m_metadata.GetBool("isLinear"_hs) ? static_cast<uint8>(Format::R8G8B8A8_UNORM) : static_cast<uint8>(Format::R8G8B8A8_SRGB));
	}

	void Texture::GenerateMipmaps()
	{
		if (!m_metadata.GetBool("GenerateMipmaps"_hs) || m_mipLevels == 1)
			return;

		for (auto& mm : m_mipmaps)
			delete[] mm.pixels;

		m_mipmaps.clear();

		int width  = static_cast<uint8>(m_extent.width);
		int height = static_cast<uint8>(m_extent.height);

		m_mipmaps.resize(m_mipLevels - 1);

		unsigned char*	   lastPixels	= m_pixels;
		uint32			   lastWidth	= m_extent.width;
		uint32			   lastHeight	= m_extent.height;
		const bool		   isLinear		= m_metadata.GetBool("isLinear"_hs);
		const MipmapFilter mipmapFilter = static_cast<MipmapFilter>(m_metadata.GetUInt8("MipmapFilter"_hs));
		for (uint32 i = 0; i < m_mipLevels - 1; i++)
		{
			uint32 width  = lastWidth / 2;
			uint32 height = lastHeight / 2;

			if (width < 1)
				width = 1;

			if (height < 1)
				height = 1;

			Mipmap mipmap;
			mipmap.width			  = width;
			mipmap.height			  = height;
			mipmap.pixels			  = new unsigned char[width * height * m_channels];
			const stbir_colorspace cs = isLinear ? stbir_colorspace::STBIR_COLORSPACE_LINEAR : stbir_colorspace::STBIR_COLORSPACE_SRGB;
			int retval				  = stbir_resize_uint8_generic(lastPixels, lastWidth, lastHeight, 0, mipmap.pixels, width, height, 0, m_channels, STBIR_ALPHA_CHANNEL_NONE, 0, stbir_edge::STBIR_EDGE_CLAMP, static_cast<stbir_filter>(mipmapFilter), cs, 0);

			lastWidth	 = width;
			lastHeight	 = height;
			lastPixels	 = mipmap.pixels;
			m_mipmaps[i] = mipmap;
		}
	}

} // namespace Lina
