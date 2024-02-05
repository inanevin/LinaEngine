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
#include "Core/Graphics/LGXWrapper.hpp"

namespace Lina
{
	Texture::~Texture()
	{
		// for (const auto& b : m_allLevels)
		//	LINA_ASSERT(b.pixels == nullptr, "Texture buffers are still filled, are you trying to delete mid-transfer?");

		auto lgxWrapper = m_resourceManager->GetSystem()->CastSubsystem<LGXWrapper>(SubsystemType::LGXWrapper);
		auto lgx		= lgxWrapper->GetLGX();
		lgx->DestroyTexture(m_gpuHandle);
	}

	uint32 Texture::GetSamplerSID() const
	{
		if (m_sampler == 0)
			return DEFAULT_SAMPLER_SID;
		return m_sampler;
	}

	void Texture::SetCustomData(uint8* pixels, uint32 width, uint32 height, uint32 bytesPerPixel, LinaGX::Format format)
	{
		if (!m_userManaged)
		{
			LINA_ERR("Custom pixels can only be set on user managed textures!");
			return;
		}

		auto lgxWrapper = m_resourceManager->GetSystem()->CastSubsystem<LGXWrapper>(SubsystemType::LGXWrapper);
		auto lgx		= lgxWrapper->GetLGX();
		auto gfxManager = m_resourceManager->GetSystem()->CastSubsystem<GfxManager>(SubsystemType::GfxManager);

		// Means we are refreshing data.
		if (!m_allLevels.empty())
		{
			lgx->Join();
			lgx->DestroyTexture(m_gpuHandle);

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

		m_channelMask = static_cast<LinaGX::ImageChannelMask>(bytesPerPixel);

		const size_t sz = width * height * bytesPerPixel;
		level0.pixels	= new uint8[sz];
		MEMCPY(level0.pixels, pixels, sz);

		m_allLevels.push_back(level0);

		LinaGX::TextureDesc desc = LinaGX::TextureDesc{
			.format	   = format,
			.flags	   = LinaGX::TextureFlags::TF_Sampled | LinaGX::TextureFlags::TF_CopyDest,
			.width	   = m_allLevels[0].width,
			.height	   = m_allLevels[0].height,
			.mipLevels = static_cast<uint32>(m_allLevels.size()),
			.debugName = m_path.c_str(),
		};
		m_gpuHandle = lgx->CreateTexture(desc);

		gfxManager->GetResourceUploadQueue().AddTextureRequest(this, [this]() {
			delete[] m_allLevels[0].pixels;
			m_allLevels[0].pixels = nullptr;
		});
		gfxManager->MarkBindlessTexturesDirty();
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
		const bool				   generateMipmaps = m_metadata.GetBool(TEXTURE_META_GENMIPS, true);
		const LinaGX::MipmapFilter mipFilter	   = static_cast<LinaGX::MipmapFilter>(m_metadata.GetUInt8(TEXTURE_META_MIPFILTER, static_cast<uint8>(LinaGX::MipmapFilter::Mitchell)));
		const bool				   isLinear		   = m_metadata.GetBool(TEXTURE_META_ISLINEAR, false);
		m_channelMask							   = static_cast<LinaGX::ImageChannelMask>(m_metadata.GetUInt8(TEXTURE_META_CHANNEL_COUNT, 4));

		m_metadata.GetUInt8(TEXTURE_META_FORMAT, static_cast<uint8>(LinaGX::Format::R8G8B8A8_SRGB));
		m_metadata.GetUInt8(TEXTURE_META_MIPMODE, static_cast<uint8>(LinaGX::MipmapMode::Linear));
		m_metadata.GetSID(TEXTURE_META_SAMPLER_SID, DEFAULT_SAMPLER_SID);

		const uint32 bytesPerPixel = static_cast<uint32>(m_channelMask);

		LinaGX::TextureBuffer outBuffer = {};
		LinaGX::LoadImageFromFile(path, outBuffer);
		LINA_ASSERT(outBuffer.pixels != nullptr, "Failed loading texture! {0}", path);

		m_allLevels.push_back(outBuffer);

		if (generateMipmaps)
		{
			LINAGX_VEC<LinaGX::TextureBuffer> mipData;
			LinaGX::GenerateMipmaps(outBuffer, mipData, mipFilter, LinaGX::ImageChannelMask::RGBA, isLinear);

			for (const auto& mp : mipData)
				m_allLevels.push_back(mp);
		}
	}

	void Texture::SaveToStream(OStream& stream)
	{
		m_metadata.SaveToStream(stream);
		const uint32 bytesPerPixel = static_cast<uint32>(m_channelMask);
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

	void Texture::LoadFromStream(IStream& stream)
	{
		m_metadata.LoadFromStream(stream);
		m_channelMask = static_cast<LinaGX::ImageChannelMask>(m_metadata.GetUInt8(TEXTURE_META_CHANNEL_COUNT, 4));

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

			buffer.bytesPerPixel = static_cast<uint32>(m_channelMask);

			m_allLevels.push_back(buffer);
		}
	}

	void Texture::BatchLoaded()
	{
		auto lgxWrapper = m_resourceManager->GetSystem()->CastSubsystem<LGXWrapper>(SubsystemType::LGXWrapper);
		auto lgx		= lgxWrapper->GetLGX();
		auto format		= static_cast<LinaGX::Format>(m_metadata.GetUInt8(TEXTURE_META_FORMAT));
		auto gfxManager = m_resourceManager->GetSystem()->CastSubsystem<GfxManager>(SubsystemType::GfxManager);

		LinaGX::TextureDesc desc = LinaGX::TextureDesc{
			.format	   = format,
			.flags	   = LinaGX::TextureFlags::TF_Sampled | LinaGX::TextureFlags::TF_CopyDest,
			.width	   = m_allLevels[0].width,
			.height	   = m_allLevels[0].height,
			.mipLevels = static_cast<uint32>(m_allLevels.size()),
			.debugName = m_path.c_str(),
		};

		m_gpuHandle = lgx->CreateTexture(desc);

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
} // namespace Lina
