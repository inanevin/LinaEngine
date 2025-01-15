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
			LinaGX::Format		 format				  = LinaGX::Format::R8G8B8A8_SRGB;
			LinaGX::MipmapFilter mipFilter			  = LinaGX::MipmapFilter::Mitchell;
			bool				 isLinear			  = false;
			bool				 generateMipmaps	  = true;
			bool				 force8Bit			  = false;
			bool				 isPremultipliedAlpha = false;
			int32				 channels			  = 4;

			void SaveToStream(OStream& out) const;
			void LoadFromStream(IStream& in);
		};

		static constexpr uint32 VERSION = 0;

		Texture(ResourceID id, const String& name) : Resource(id, GetTypeID<Texture>(), name){};
		virtual ~Texture();

		virtual void   SaveToStream(OStream& stream) const override;
		virtual bool   LoadFromFile(const String& path) override;
		virtual void   LoadFromStream(IStream& stream) override;
		void		   LoadFromBuffer(uint8* pixels, uint32 width, uint32 height, uint32 bytesPerPixel);
		virtual void   GenerateHW() override;
		virtual void   DestroyHW() override;
		virtual size_t GetSize() const override
		{
			return GetTotalSize();
		}

		void	  GenerateHWFromDesc(const LinaGX::TextureDesc& desc);
		void	  DestroySW();
		void	  AddToUploadQueue(ResourceUploadQueue& queue, bool destroySW);
		Vector2ui GetSize();
		Vector2	  GetSizeF();
		void	  VerifyMipmaps();
		void	  DetermineFormat();
		void	  ClearAllMipLevels();

		inline size_t GetTotalSize() const
		{
			return m_totalSize;
		}

		virtual void SetCustomMeta(IStream& stream) override
		{
			m_meta.LoadFromStream(stream);
		}

		inline uint32 GetGPUHandle() const
		{
			return m_gpuHandle;
		}

		inline const Vector<LinaGX::TextureBuffer>& GetAllLevels() const
		{
			return m_allLevels;
		}

		inline const Metadata& GetMeta() const
		{
			return m_meta;
		}

		inline Metadata& GetMeta()
		{
			return m_meta;
		}

		inline uint32 GetBytesPerPixel() const
		{
			return m_bytesPerPixel;
		}

		inline bool GetIsMSAA() const
		{
			return m_isMSAA;
		}

		inline uint16 GetCreationFlags() const
		{
			return m_creationFlags;
		}

	private:
		uint32 GetChannels();
		void   CalculateTotalSize();

	private:
		ALLOCATOR_BUCKET_MEM;
		LINA_REFLECTION_ACCESS(Texture);

		Vector<LinaGX::TextureBuffer> m_allLevels;
		uint32						  m_gpuHandle			 = 0;
		uint32						  m_bytesPerPixel		 = 0;
		Metadata					  m_meta				 = {};
		Vector2ui					  m_size				 = Vector2ui::Zero;
		bool						  m_useGlobalDelete		 = false;
		size_t						  m_totalSize			 = 0;
		LinaGX::MipmapFilter		  m_generatedMipFilter	 = LinaGX::MipmapFilter::Default;
		bool						  m_generatedMipPreAlpha = false;
		bool						  m_isMSAA				 = false;
		uint16						  m_creationFlags		 = 0;
	};

	LINA_RESOURCE_BEGIN(Texture);
	LINA_FIELD(Texture, m_meta, "Metadata", FieldType::UserClass, GetTypeID<Texture::Metadata>())
	LINA_CLASS_END(Texture);

	LINA_CLASS_BEGIN(TextureMeta)
	LINA_FIELD(Texture::Metadata, isLinear, "Is Linear", FieldType::Boolean, 0)
	LINA_FIELD(Texture::Metadata, force8Bit, "Force 8 Bits", FieldType::Boolean, 0)
	LINA_FIELD(Texture::Metadata, generateMipmaps, "Generate Mipmaps", FieldType::Boolean, 0)
	LINA_FIELD(Texture::Metadata, isPremultipliedAlpha, "Pre-multiplied Alpha", FieldType::Boolean, 0)
	LINA_FIELD(Texture::Metadata, mipFilter, "Mipmap Filter", FieldType::Enum, GetTypeID<LinaGX::MipmapFilter>())
	LINA_FIELD_DEPENDENCY(Texture::Metadata, isPremultipliedAlpha, "generateMipmaps", "1")
	LINA_FIELD_DEPENDENCY(Texture::Metadata, mipFilter, "generateMipmaps", "1")
	LINA_CLASS_END(TextureMeta)

	/*
	 LINA_FIELD_PROPERTY(Texture::Metadata,
						 format,
						 Vector<uint32>,
						 "OnlyShow",
						 (Vector<uint32>{(uint32)LinaGX::Format::R8_UNORM, (uint32)LinaGX::Format::R8G8_SNORM, (uint32)LinaGX::Format::R16G16B16A16_SFLOAT, (uint32)LinaGX::Format::R8G8B8A8_UNORM, (uint32)LinaGX::Format::R8G8B8A8_SRGB}))
	 */

} // namespace Lina
