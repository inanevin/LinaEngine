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

namespace Lina
{
	class Audio : public Resource
	{
	public:
		struct Metadata
		{
			void SaveToStream(OStream& out) const;
			void LoadFromStream(IStream& in);
		};

		Audio(ResourceID id, const String& name) : Resource(id, GetTypeID<Audio>(), name) {};
		virtual ~Audio();

		static constexpr uint32 VERSION = 0;

		virtual bool   LoadFromFile(const String& path) override;
		virtual void   LoadFromStream(IStream& stream) override;
		virtual void   SaveToStream(OStream& stream) const override;
		virtual size_t GetSize() const override;
		virtual void   GenerateHW() override;
		virtual void   DestroyHW() override;
		virtual void   SetCustomMeta(IStream& stream) override
		{
			m_meta.LoadFromStream(stream);
		}

		inline uint32 GetHW() const
		{
			return m_buffer;
		}

	private:
		ALLOCATOR_BUCKET_MEM;
		Metadata			  m_meta	   = {};
		uint32				  m_buffer	   = 0;
		int32				  m_size	   = 0;
		int32				  m_sampleRate = 0;
		int32				  m_bitDepth   = 0;
		int32				  m_samples	   = 0;
		float				  m_length	   = 0.0f;
		int32				  m_channels   = 0;
		int32				  m_format	   = 0;
		Vector<unsigned char> m_audioBuffer;
	};

	LINA_RESOURCE_BEGIN(Audio);
	LINA_CLASS_END(Audio);
} // namespace Lina
