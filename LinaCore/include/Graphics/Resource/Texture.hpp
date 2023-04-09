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

#include "Resources/Core/IResource.hpp"
#include "Graphics/Resource/TextureSampler.hpp"
#include "Resources/Data/ResourceMetadata.hpp"
#include "Graphics/Data/DescriptorHandle.hpp"

namespace Lina
{
	class Renderer;
	class IGfxResourceTexture;

	struct UserGeneratedTextureData
	{
		TextureResourceType resourceType				  = TextureResourceType::Texture2DDefault;
		String				path						  = "";
		StringID			sid							  = 0;
		Extent3D			extent						  = Extent3D();
		Format				format						  = Format::R32G32B32A32_SFLOAT;
		StringID			targetSampler				  = 0;
		ImageTiling			tiling						  = ImageTiling::Optimal;
		int					channels					  = 4;
		bool				createPixelBuffer			  = false;
		bool				destroyPixelBufferAfterUpload = true;
	};

	class Texture;

	struct TextureSheetItem
	{
		Texture* texture = nullptr;
		Vector2	 uvTL	 = Vector2::Zero;
		Vector2	 uvBR	 = Vector2::Zero;
		Vector2i size	 = Vector2i::Zero;
	};

	class Texture : public IResource
	{

	public:
		struct Mipmap
		{
			uint32		   width  = 0;
			uint32		   height = 0;
			unsigned char* pixels = nullptr;
		};

	public:
		Texture(ResourceManager* rm, bool isUserManaged, const String& path, StringID sid);
		Texture(ResourceManager* rm, const UserGeneratedTextureData& textureData);
		virtual ~Texture();

		virtual void	Upload() override;
		void			SetSampler(StringID samplerSID);
		TextureSampler* GetSampler();

		Vector<TextureSheetItem> GetSheetItems(uint32 columns, uint32 rows);

		inline const Extent3D& GetExtent() const
		{
			return m_extent;
		}

		inline int32 GetGPUBindlessIndex() const
		{
			return m_gpuBindlessIndex;
		}

		inline uint32 GetMipLevels() const
		{
			return m_mipLevels;
		}

		inline uint32 GetChannels() const
		{
			return m_channels;
		}

		inline StringID GetSamplerSID() const
		{
			return m_samplerSID;
		}

		inline const Vector<Mipmap>& GetMipmaps() const
		{
			return m_mipmaps;
		}

		inline unsigned char* GetPixels()
		{
			return m_pixels;
		}

		inline TextureResourceType GetResourceType() const
		{
			return m_resourceType;
		}

	protected:
		// Inherited via IResource
		virtual void LoadFromFile(const char* path) override;
		virtual void SaveToStream(OStream& stream) override;
		virtual void LoadFromStream(IStream& stream) override;
		virtual void BatchLoaded() override;

	private:
		void CheckFormat(int channels);
		void GenerateMipmaps();

	private:
		friend class Renderer;

		Renderer*			m_renderer	   = nullptr;
		TextureSampler*		m_sampler	   = nullptr;
		TextureResourceType m_resourceType = TextureResourceType::Texture2DDefault;
		Vector<Mipmap>		m_mipmaps;
		StringID			m_samplerSID = 0;
		Extent3D			m_extent;
		uint32				m_mipLevels				   = 1;
		uint32				m_channels				   = 0;
		int32				m_gpuBindlessIndex		   = -1;
		unsigned char*		m_pixels				   = nullptr;
		bool				m_pixelsLoadedFromStream   = false;
		bool				m_destroyPixelsAfterUpload = true;

		// GPU
		IGfxResourceTexture* m_gpuResource		   = nullptr;
		DescriptorHandle	 m_descriptor		   = {};
		DescriptorHandle	 m_descriptorSecondary = {};
	};
} // namespace Lina

#endif
