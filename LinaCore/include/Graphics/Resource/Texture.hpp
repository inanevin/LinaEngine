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

namespace Lina
{
	class Renderer;

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
		Texture(ResourceManager* rm, StringID sid, const Extent3D ext, StringID targetSampler, Format format, ImageTiling tiling, int channels = 4);
		virtual ~Texture();

		/// <summary>
		/// If creating a programmable texture via manipulating pixels,
		/// use this to upload the texture to the GPU ready to be sampled from shaders.
		/// </summary>
		void UploadToGPU();

		/// <summary>
		/// If using a texture as a render target, use this to generate image buffers on the gpu.
		/// </summary>
		void GenerateImage(ImageType type);

		void SetSampler(StringID samplerSID);

		inline const Extent3D& GetExtent() const
		{
			return m_extent;
		}

		inline int32 GetGPUHandle()
		{
			return m_gpuHandle;
		}

		inline uint32 GetMipLevels()
		{
			return m_mipLevels;
		}

		inline uint32 GetChannels()
		{
			return m_channels;
		}

		inline StringID GetSampler()
		{
			return m_samplerSID;
		}

		inline const Vector<Mipmap>& GetMipmaps()
		{
			return m_mipmaps;
		}

		inline unsigned char*& GetPixels()
		{
			return m_pixels;
		}

	protected:
		// Inherited via IResource
		virtual void LoadFromFile(const char* path) override;
		virtual void SaveToStream(OStream& stream) override;
		virtual void LoadFromStream(IStream& stream) override;
		virtual void Flush() override;
		virtual void Upload() override;
		virtual void BatchLoaded() override;

	private:
		void CheckFormat(int channels);
		void GenerateMipmaps();

	private:
		StringID		m_samplerSID			 = 0;
		TextureSampler* m_sampler				 = nullptr;
		Renderer*		m_renderer				 = nullptr;
		bool			m_pixelsLoadedFromStream = false;
		Extent3D		m_extent;
		uint32			m_mipLevels = 1;
		uint32			m_channels	= 0;
		int32			m_gpuHandle = -1;
		Vector<Mipmap>	m_mipmaps;
		unsigned char*	m_pixels = nullptr;
	};
} // namespace Lina

#endif
