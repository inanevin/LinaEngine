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

#ifndef Texture_HPP
#define Texture_HPP

#include "Core/Resources/Resource.hpp"
#include "Common/Platform/LinaGXIncl.hpp"
#include "Common/Data/Vector.hpp"
#include "Core/Graphics/CommonGraphics.hpp"

namespace Lina
{
	class Texture;

	struct TextureSheetItem
	{
		Texture*  texture = nullptr;
		Vector2	  uvTL	  = Vector2::Zero;
		Vector2	  uvBR	  = Vector2::Zero;
		Vector2ui size	  = Vector2ui::Zero;
	};

	class Texture : public Resource
	{
	public:
		struct Metadata
		{
			LinaGX::Format			 format			 = LinaGX::Format::R8G8B8A8_SRGB;
			LinaGX::MipmapMode		 mipmapMode		 = LinaGX::MipmapMode::Linear;
			LinaGX::MipmapFilter	 mipFilter		 = LinaGX::MipmapFilter::Mitchell;
			LinaGX::ImageChannelMask channelMask	 = LinaGX::ImageChannelMask::RGBA;
			StringID				 samplerSID		 = DEFAULT_SAMPLER_SID;
			bool					 isLinear		 = false;
			bool					 generateMipmaps = true;

			void SaveToStream(OStream& out) const;
			void LoadFromStream(IStream& in);
		};

	public:
		virtual void			 BatchLoaded() override;
		uint32					 GetSamplerSID() const;
		void					 SetCustomData(uint8* pixels, uint32 width, uint32 height, uint32 bytesPerPixel, LinaGX::Format format, bool generateMipMaps = false);
		Vector<TextureSheetItem> GetSheetItems(uint32 columns, uint32 rows);

		inline uint32 GetGPUHandle() const
		{
			return m_gpuHandle;
		}

		inline Vector<LinaGX::TextureBuffer> GetAllLevels() const
		{
			return m_allLevels;
		}

		inline void SetSamplerSID(StringID sid)
		{
			m_sampler = sid;
		}

		inline uint32 GetBindlessIndex() const
		{
			return m_bindlessIndex;
		}

	private:
		FRIEND_RESOURCE_CACHE();
		Texture(ResourceManager* rm, const String& path, StringID sid) : Resource(rm, path, sid, GetTypeID<Texture>()){};
		virtual ~Texture();

	protected:
		virtual void LoadFromFile(const char* path) override;
		virtual void LoadFromStream(IStream& stream) override;
		virtual void SaveToStream(OStream& stream) const override;
		virtual void SetCustomMeta(IStream& stream) override
		{
			m_meta.LoadFromStream(stream);
		}

	private:
		uint32						  m_bindlessIndex = 0;
		Vector<LinaGX::TextureBuffer> m_allLevels;
		uint32						  m_gpuHandle		 = 0;
		StringID					  m_sampler			 = 0;
		bool						  m_loadedFromStream = false;
		Metadata					  m_meta			 = {};
	};
} // namespace Lina

#endif
