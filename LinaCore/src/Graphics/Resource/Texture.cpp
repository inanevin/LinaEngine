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

#include "Core/Graphics/Resource/Texture.hpp"
#include "Common/Math/Math.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Common/System/System.hpp"
#include "Core/Graphics/GfxManager.hpp"

namespace Lina
{
	void Texture::Metadata::SaveToStream(OStream& out) const
	{
		const uint8 formatInt	   = static_cast<uint8>(format);
		const uint8 mipmapModeInt  = static_cast<uint8>(mipmapMode);
		const uint8 mipFilterInt   = static_cast<uint8>(mipFilter);
		const uint8 channelMaskInt = static_cast<uint8>(channelMask);
		out << formatInt << mipmapModeInt << mipFilterInt << channelMaskInt;
		out << samplerSID << isLinear << generateMipmaps;
	}

	void Texture::Metadata::LoadFromStream(IStream& in)
	{
		uint8 formatInt = 0, mipmapModeInt = 0, mipFilterInt = 0, channelMaskInt = 0;
		in >> formatInt >> mipmapModeInt >> mipFilterInt >> channelMaskInt;
		format		= static_cast<LinaGX::Format>(formatInt);
		mipmapMode	= static_cast<LinaGX::MipmapMode>(mipmapModeInt);
		mipFilter	= static_cast<LinaGX::MipmapFilter>(mipFilterInt);
		channelMask = static_cast<LinaGX::ImageChannelMask>(channelMaskInt);
		in >> samplerSID >> isLinear >> generateMipmaps;
	}

	Texture::~Texture()
	{
		for (const auto& b : m_allLevels)
			LINA_ASSERT(b.pixels == nullptr, "Texture buffers are still filled, are you trying to delete mid-transfer?");

		auto gfxMan = m_resourceManager->GetSystem()->CastSubsystem<GfxManager>(SubsystemType::GfxManager);
		gfxMan->GetLGX()->DestroyTexture(m_gpuHandle);
	}

	uint32 Texture::GetSamplerSID() const
	{
		if (m_sampler == 0)
			return m_meta.samplerSID;
		return m_sampler;
	}

	void Texture::SetCustomData(uint8* pixels, uint32 width, uint32 height, uint32 bytesPerPixel, LinaGX::Format format, bool generateMipMaps)
	{
		if (m_owner != ResourceOwner::UserCode)
		{
			LINA_ERR("Custom pixels can only be set on user managed textures!");
			return;
		}

		auto gfxManager = m_resourceManager->GetSystem()->CastSubsystem<GfxManager>(SubsystemType::GfxManager);

		// Means we are refreshing data.
		if (!m_allLevels.empty())
		{
			gfxManager->GetLGX()->Join();
			gfxManager->GetLGX()->DestroyTexture(m_gpuHandle);

			for (auto& b : m_allLevels)
			{
				delete[] b.pixels;
				b.pixels = nullptr;
			}

			m_allLevels.clear();
		}

		LinaGX::TextureBuffer level0 = {
			.width		   = width,
			.height		   = height,
			.bytesPerPixel = bytesPerPixel,
		};

		m_meta.channelMask = static_cast<LinaGX::ImageChannelMask>(bytesPerPixel - 1);

		const size_t sz = width * height * bytesPerPixel;
		level0.pixels	= new uint8[sz];
		MEMCPY(level0.pixels, pixels, sz);

		m_allLevels.push_back(level0);

		if (generateMipMaps)
		{
			LINAGX_VEC<LinaGX::TextureBuffer> mipData;
			LinaGX::GenerateMipmaps(m_allLevels[0], mipData, m_meta.mipFilter, m_meta.channelMask, m_meta.isLinear);
			for (const auto& mp : mipData)
				m_allLevels.push_back(mp);
		}

		LinaGX::TextureDesc desc = LinaGX::TextureDesc{
			.format	   = format,
			.flags	   = LinaGX::TextureFlags::TF_Sampled | LinaGX::TextureFlags::TF_CopyDest,
			.width	   = m_allLevels[0].width,
			.height	   = m_allLevels[0].height,
			.mipLevels = static_cast<uint32>(m_allLevels.size()),
			.debugName = m_path.c_str(),
		};
		m_gpuHandle = gfxManager->GetLGX()->CreateTexture(desc);

		gfxManager->GetResourceUploadQueue().AddTextureRequest(this, [this]() {
			for (auto& l : m_allLevels)
			{
				delete[] l.pixels;
				l.pixels = nullptr;
			}
		});
	}

	Vector<TextureSheetItem> Texture::GetSheetItems(uint32 columns, uint32 rows)
	{
		const uint32 width	= m_allLevels[0].width;
		const uint32 height = m_allLevels[0].height;

		Vector<TextureSheetItem> items;
		const float				 gridX = static_cast<float>(width) / static_cast<float>(columns);
		const float				 gridY = static_cast<float>(height) / static_cast<float>(rows);
		const Vector2ui			 size  = Vector2ui(static_cast<uint32>(gridX), static_cast<uint32>(gridY));

		items.reserve(columns * rows);
		const float uvProgX = 1.0f / static_cast<float>(columns);
		const float uvProgY = 1.0f / static_cast<float>(rows);

		for (uint32 i = 0; i < rows; i++)
		{
			for (uint32 j = 0; j < columns; j++)
			{
				const Vector2 uvTL = Vector2(j * uvProgX, i * uvProgY);
				const Vector2 uvBR = Vector2((j + 1) * uvProgX, (i + 1) * uvProgY);

				TextureSheetItem item = TextureSheetItem{
					.texture = this,
					.uvTL	 = uvTL,
					.uvBR	 = uvBR,
					.size	 = size,
				};

				items.push_back(item);
			}
		}

		return items;
	}

	void Texture::LoadFromFile(const char* path)
	{
		LinaGX::TextureBuffer outBuffer = {};
		LinaGX::LoadImageFromFile(path, outBuffer, m_meta.channelMask);
		LINA_ASSERT(outBuffer.pixels != nullptr, "Failed loading texture! {0}", path);

		m_allLevels.push_back(outBuffer);

		if (m_meta.generateMipmaps)
		{
			LINAGX_VEC<LinaGX::TextureBuffer> mipData;
			LinaGX::GenerateMipmaps(outBuffer, mipData, m_meta.mipFilter, m_meta.channelMask, m_meta.isLinear);

			for (const auto& mp : mipData)
				m_allLevels.push_back(mp);
		}
	}

	void Texture::LoadFromStream(IStream& stream)
	{
		m_meta.LoadFromStream(stream);

		uint32 allLevels = 0;
		stream >> allLevels;
		m_loadedFromStream = true;

		for (uint32 i = 0; i < allLevels; i++)
		{
			uint32				  pixelSize = 0;
			LinaGX::TextureBuffer buffer;
			stream >> buffer.width >> buffer.height >> pixelSize;

			if (pixelSize != 0)
			{
				buffer.pixels = new uint8[pixelSize];
				stream.ReadEndianSafe(buffer.pixels, pixelSize);
			}

			buffer.bytesPerPixel = static_cast<uint32>(m_meta.channelMask) + 1;

			m_allLevels.push_back(buffer);
		}
	}

	void Texture::SaveToStream(OStream& stream) const
	{
		m_meta.SaveToStream(stream);

		const uint32 bytesPerPixel = static_cast<uint32>(m_meta.channelMask) + 1;
		const uint32 allLevels	   = static_cast<uint32>(m_allLevels.size());

		stream << allLevels;

		for (const auto& buffer : m_allLevels)
		{
			const uint32 pixelSize = buffer.width * buffer.height * bytesPerPixel;
			stream << buffer.width << buffer.height << pixelSize;

			if (pixelSize != 0)
				stream.WriteEndianSafe(buffer.pixels, pixelSize);
		}
	}

	void Texture::BatchLoaded()
	{
		auto gfxManager = m_resourceManager->GetSystem()->CastSubsystem<GfxManager>(SubsystemType::GfxManager);

		LinaGX::TextureDesc desc = LinaGX::TextureDesc{
			.format	   = m_meta.format,
			.flags	   = LinaGX::TextureFlags::TF_Sampled | LinaGX::TextureFlags::TF_CopyDest,
			.width	   = m_allLevels[0].width,
			.height	   = m_allLevels[0].height,
			.mipLevels = static_cast<uint32>(m_allLevels.size()),
			.debugName = m_path.c_str(),
		};

		m_gpuHandle = gfxManager->GetLGX()->CreateTexture(desc);

		gfxManager->GetResourceUploadQueue().AddTextureRequest(this, [this]() {
			for (auto& buffer : m_allLevels)
			{
				if (m_loadedFromStream)
				{
					delete buffer.pixels;
				}
				else
					LinaGX::FreeImage(buffer.pixels);

				buffer.pixels = nullptr;
			}

			m_allLevels.clear();
		});
	}

	Vector2ui Texture::GetSize()
	{
		return Vector2ui(m_allLevels[0].width, m_allLevels[0].height);
	}

	Vector2 Texture::GetSizeF()
	{
		return Vector2(static_cast<float>(m_allLevels[0].width), static_cast<float>(m_allLevels[0].height));
	}
} // namespace Lina
