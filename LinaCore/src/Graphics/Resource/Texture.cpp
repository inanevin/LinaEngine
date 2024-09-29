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
#include "Common/System/System.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Graphics/GfxManager.hpp"
#include "Core/Graphics/ResourceUploadQueue.hpp"

namespace Lina
{
	void Texture::Metadata::SaveToStream(OStream& out) const
	{
		out << format << mipFilter;
		out << generateMipmaps << force8Bit;
	}

	void Texture::Metadata::LoadFromStream(IStream& in)
	{
		in >> format >> mipFilter;
		in >> generateMipmaps >> force8Bit;
	}

	Texture::~Texture()
	{
		DestroySW();
		DestroyHW();
	}

	void Texture::DestroySW()
	{
		for (auto& b : m_allLevels)
		{
			if (m_useGlobalDelete)
				delete[] b.pixels;
			else
				LinaGX::FreeImage(b.pixels);

			b.pixels = nullptr;
		}
		m_allLevels.clear();
	}

	void Texture::LoadFromBuffer(uint8* pixels, uint32 width, uint32 height, uint32 bytesPerPixel)
	{
		DestroySW();

		m_size = Vector2ui(width, height);

		LinaGX::TextureBuffer level0 = {
			.width		   = width,
			.height		   = height,
			.bytesPerPixel = bytesPerPixel,
		};
		m_bytesPerPixel = bytesPerPixel;

		const size_t sz = width * height * bytesPerPixel;
		level0.pixels	= new uint8[sz];
		MEMCPY(level0.pixels, pixels, sz);

		m_allLevels.push_back(level0);

		const uint32 channels = GetChannels();

		if (m_meta.generateMipmaps)
		{
			LINAGX_VEC<LinaGX::TextureBuffer> mipData;
			LinaGX::GenerateMipmaps(m_allLevels[0], mipData, m_meta.mipFilter, channels, m_meta.format == LinaGX::Format::R8G8B8A8_UNORM);
			for (const auto& mp : mipData)
				m_allLevels.push_back(mp);
		}

		m_useGlobalDelete = true;
		CalculateTotalSize();
	}

	void Texture::LoadFromFile(const String& path)
	{
		DestroySW();

		LinaGX::TextureBuffer outBuffer = {};
		LinaGX::LoadImageFromFile(path.c_str(), outBuffer, 0, &m_importedChannels, m_meta.force8Bit);
		m_bytesPerPixel = outBuffer.bytesPerPixel;
		LINA_ASSERT(outBuffer.pixels != nullptr, "Failed loading texture! {0}", path);

		if (m_importedChannels == 1)
			m_meta.format = m_bytesPerPixel == 1 ? LinaGX::Format::R8_UNORM : LinaGX::Format::R16_UNORM;
		else if (m_importedChannels == 2)
			m_meta.format = m_bytesPerPixel == 2 ? LinaGX::Format::R8G8_UNORM : LinaGX::Format::R16G16_UNORM;
		else if (m_importedChannels == 3)
		{
			// Reimport as 4.
			LinaGX::FreeImage(outBuffer.pixels);
			outBuffer = {};
			LinaGX::LoadImageFromFile(path.c_str(), outBuffer, 4);
			m_importedChannels = 4;
			m_bytesPerPixel	   = outBuffer.bytesPerPixel;
			LINA_ASSERT(outBuffer.pixels != nullptr, "Failed loading texture! {0}", path);
			m_meta.format = m_bytesPerPixel == 4 ? LinaGX::Format::R8G8B8A8_SRGB : LinaGX::Format::R16G16B16A16_UNORM;
		}
		else if (m_importedChannels == 4)
		{
			m_meta.format = m_bytesPerPixel == 4 ? LinaGX::Format::R8G8B8A8_SRGB : LinaGX::Format::R16G16B16A16_UNORM;
		}

		m_size.x = outBuffer.width;
		m_size.y = outBuffer.height;
		m_allLevels.push_back(outBuffer);

		if (m_meta.generateMipmaps)
		{
			LINAGX_VEC<LinaGX::TextureBuffer> mipData;
			LinaGX::GenerateMipmaps(outBuffer, mipData, m_meta.mipFilter, m_importedChannels, m_meta.format == LinaGX::Format::R8G8B8A8_UNORM);

			for (const auto& mp : mipData)
				m_allLevels.push_back(mp);
		}

		CalculateTotalSize();
	}

	void Texture::LoadFromStream(IStream& stream)
	{
		Resource::LoadFromStream(stream);

		uint32 version = 0;
		stream >> version;
		stream >> m_meta;
		stream >> m_importedChannels;
		DestroySW();

		stream >> m_bytesPerPixel;

		uint32 allLevels = 0;
		stream >> allLevels;
		m_useGlobalDelete = true;

		for (uint32 i = 0; i < allLevels; i++)
		{
			uint32				  pixelSize = 0;
			LinaGX::TextureBuffer buffer;
			stream >> buffer.width >> buffer.height >> pixelSize;

			if (i == 0)
			{
				m_size.x = buffer.width;
				m_size.y = buffer.height;
			}

			if (pixelSize != 0)
			{
				buffer.pixels = new uint8[pixelSize];
				stream.ReadToRawEndianSafe(buffer.pixels, pixelSize);
			}

			buffer.bytesPerPixel = m_bytesPerPixel;

			m_allLevels.push_back(buffer);
		}
		CalculateTotalSize();
	}

	void Texture::SaveToStream(OStream& stream) const
	{
		Resource::SaveToStream(stream);

		stream << VERSION;
		stream << m_meta;
		stream << m_importedChannels;
		stream << m_bytesPerPixel;

		const uint32 allLevels = static_cast<uint32>(m_allLevels.size());
		stream << allLevels;

		for (const auto& buffer : m_allLevels)
		{
			const uint32 pixelSize = buffer.width * buffer.height * m_bytesPerPixel;
			stream << buffer.width << buffer.height << pixelSize;

			if (pixelSize != 0)
				stream.WriteRawEndianSafe(buffer.pixels, pixelSize);
		}
	}

	Vector2ui Texture::GetSize()
	{
		return m_size;
	}

	Vector2 Texture::GetSizeF()
	{
		return Vector2(static_cast<float>(m_size.x), static_cast<float>(m_size.y));
	}

	void Texture::GenerateHWFromDesc(const LinaGX::TextureDesc& desc)
	{
		LINA_ASSERT(m_gpuHandleExists == false, "");
		m_gpuHandle		  = GfxManager::GetLGX()->CreateTexture(desc);
		m_gpuHandleExists = true;
		m_size			  = Vector2ui(desc.width, desc.height);
	}

	void Texture::GenerateHW()
	{
		LINA_ASSERT(m_gpuHandleExists == false, "");
		LinaGX::TextureDesc desc = LinaGX::TextureDesc{
			.format	   = m_meta.format,
			.flags	   = LinaGX::TextureFlags::TF_Sampled | LinaGX::TextureFlags::TF_CopyDest,
			.width	   = m_allLevels[0].width,
			.height	   = m_allLevels[0].height,
			.mipLevels = static_cast<uint32>(m_allLevels.size()),
			.debugName = m_name.c_str(),
		};
		m_gpuHandle		  = GfxManager::GetLGX()->CreateTexture(desc);
		m_gpuHandleExists = true;
	}

	void Texture::DestroyHW()
	{
		if (!m_gpuHandleExists)
			return;

		GfxManager::GetLGX()->DestroyTexture(m_gpuHandle);
		m_gpuHandleExists = false;
	}

	void Texture::AddToUploadQueue(ResourceUploadQueue& queue, bool destroySW)
	{
		LINA_ASSERT(m_gpuHandleExists == true, "");
		queue.AddTextureRequest(this, [this, destroySW]() {
			if (destroySW)
				DestroySW();
		});
	}

	uint32 Texture::GetChannels()
	{
		if (m_meta.format == LinaGX::Format::R8_UNORM || m_meta.format == LinaGX::Format::R16_UNORM)
			return 1;
		else if (m_meta.format == LinaGX::Format::R8G8_UNORM || m_meta.format == LinaGX::Format::R16G16_UNORM)
			return 2;
		else if (m_meta.format == LinaGX::Format::R8G8B8A8_UNORM || m_meta.format == LinaGX::Format::R8G8B8A8_SRGB)
			return 4;
		else if (m_meta.format == LinaGX::Format::R16G16B16A16_UNORM)
			return 4;

		LINA_ASSERT(false, "");
		return 4;
	}

	void Texture::CalculateTotalSize()
	{
		m_totalSize = 0;
		for (const auto& level : m_allLevels)
			m_totalSize += static_cast<size_t>(level.width * level.height * level.bytesPerPixel);
	}
} // namespace Lina
