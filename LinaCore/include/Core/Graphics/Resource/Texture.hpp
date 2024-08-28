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

#pragma once

#include "Core/Resources/Resource.hpp"
#include "Common/Platform/LinaGXIncl.hpp"
#include "Common/Data/Vector.hpp"
#include "Core/Graphics/CommonGraphics.hpp"

namespace Lina
{
	class Texture;
	class ResourceUploadQueue;

	class Texture : public Resource
	{
	public:
		struct Metadata
		{
			LinaGX::Format			 format			 = LinaGX::Format::R8G8B8A8_SRGB;
			LinaGX::MipmapMode		 mipmapMode		 = LinaGX::MipmapMode::Linear;
			LinaGX::MipmapFilter	 mipFilter		 = LinaGX::MipmapFilter::Mitchell;
			LinaGX::ImageChannelMask channelMask	 = LinaGX::ImageChannelMask::RGBA;
			bool					 isLinear		 = false;
			bool					 generateMipmaps = true;

			void SaveToStream(OStream& out) const;
			void LoadFromStream(IStream& in);
		};

		static constexpr uint32 VERSION = 0;

		Texture(const String& path, StringID sid) : Resource(path, sid, GetTypeID<Texture>()){};
		Texture(ResourceID id) : Resource(id){};
		virtual ~Texture();

		virtual void SaveToStream(OStream& stream) const override;
		virtual void LoadFromFile(const char* path) override;
		virtual void LoadFromStream(IStream& stream) override;
		void		 LoadFromBuffer(uint8* pixels, uint32 width, uint32 height, uint32 bytesPerPixel, LinaGX::ImageChannelMask channelMask, LinaGX::Format format, bool generateMipMaps = false);

		void	  GenerateHWFromDesc(const LinaGX::TextureDesc& desc);
		void	  GenerateHW();
		void	  DestroyHW();
		void	  DestroySW();
		void	  AddToUploadQueue(ResourceUploadQueue& queue);
		Vector2ui GetSize();
		Vector2	  GetSizeF();
		void	  OnUploadCompleted();

		virtual void SetCustomMeta(IStream& stream) override
		{
			m_meta.LoadFromStream(stream);
		}

		inline uint32 GetGPUHandle() const
		{
			return m_gpuHandle;
		}

		inline Vector<LinaGX::TextureBuffer> GetAllLevels() const
		{
			return m_allLevels;
		}

		inline uint32 GetBindlessIndex() const
		{
			return m_bindlessIndex;
		}

		inline const Metadata& GetMeta() const
		{
			return m_meta;
		}

		inline void SetBindlessIndex(uint32 bindless)
		{
			m_bindlessIndex = bindless;
		}

		inline bool IsGPUValid() const
		{
			return m_gpuHandleExists;
		}

	private:
		ALLOCATOR_BUCKET_MEM;
		Vector<LinaGX::TextureBuffer> m_allLevels;
		uint32						  m_bindlessIndex	= 0;
		uint32						  m_gpuHandle		= 0;
		uint32						  m_bytesPerPixel	= 0;
		Metadata					  m_meta			= {};
		Vector2ui					  m_size			= Vector2ui::Zero;
		bool						  m_useGlobalDelete = false;
		bool						  m_gpuHandleExists = false;
	};

	LINA_REFLECTRESOURCE_BEGIN(Texture);
	LINA_REFLECTRESOURCE_END(Texture);
} // namespace Lina
