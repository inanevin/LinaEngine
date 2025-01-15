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
#include "Common/Data/Vector.hpp"
#include "Common/Data/Mutex.hpp"
#include "Common/Data/CommonData.hpp"

namespace LinaVG
{
	class Font;
	class Text;
} // namespace LinaVG

namespace Lina
{
	class Font : public Resource
	{
	public:
		struct FontPoint
		{
			uint32 size		  = 12;
			float  scaleLimit = 10.0f;
		};

		struct GlyphRange
		{
			uint32 start = 0;
			uint32 end	 = 0;
		};

		struct Metadata
		{
			Vector<FontPoint>  points = {FontPoint()};
			Vector<GlyphRange> glyphRanges;
			bool			   isSDF = false;

			void SaveToStream(OStream& out) const;
			void LoadFromStream(IStream& in);
		};

		static constexpr uint32 VERSION = 0;

		Font(ResourceID id, const String& name) : Resource(id, GetTypeID<Font>(), name){};
		virtual ~Font();

		LinaVG::Font*  GetFont(float scaleFactor);
		virtual bool   LoadFromFile(const String& path) override;
		virtual void   LoadFromStream(IStream& stream) override;
		virtual void   SaveToStream(OStream& stream) const override;
		virtual void   GenerateHW() override;
		virtual void   DestroyHW() override;
		virtual size_t GetSize() const override;

		void Upload(LinaVG::Text& lvgText);

		inline Metadata& GetMeta()
		{
			return m_meta;
		}

		inline const Metadata& GetMeta() const
		{
			return m_meta;
		}

		virtual void SetCustomMeta(IStream& stream) override
		{
			m_meta.LoadFromStream(stream);
		}

		inline size_t GetFileSize() const
		{
			return m_file.size();
		}

		inline const Vector<char>& GetFileData() const
		{
			return m_file;
		}

	private:
		void SaveLVGFontToStream(OStream& stream, LinaVG::Font*) const;
		void LoadLVGFontToStream(IStream& stream, LinaVG::Font*);

	private:
		ALLOCATOR_BUCKET_MEM;
		LINA_REFLECTION_ACCESS(Font);
		Vector<char>		  m_file;
		Vector<LinaVG::Font*> m_hwLvgFonts = {};
		Metadata			  m_meta	   = {};
		LinaVG::Text*		  m_lvgText	   = nullptr;
	};

	LINA_RESOURCE_BEGIN(Font);
	LINA_FIELD(Font, m_meta, "Metadata", FieldType::UserClass, GetTypeID<Font::Metadata>())
	LINA_CLASS_END(Font);

	LINA_CLASS_BEGIN(FontPointRef)
	LINA_FIELD(Font::FontPoint, size, "Size", FieldType::UInt32, 0)
	LINA_FIELD(Font::FontPoint, scaleLimit, "Scale Limit", FieldType::Float, 0)
	LINA_FIELD_LIMITS(Font::FontPoint, size, 4, 96, 0)
	LINA_CLASS_END(FontPointRef)

	LINA_CLASS_BEGIN(FontGlyphRange)
	LINA_FIELD(Font::GlyphRange, start, "Start", FieldType::UInt32, 0)
	LINA_FIELD(Font::GlyphRange, end, "End", FieldType::UInt32, 0)
	LINA_CLASS_END(FontGlyphRange)

	LINA_CLASS_BEGIN(FontMeta)
	LINA_FIELD(Font::Metadata, isSDF, "Is SDF", FieldType::Boolean, 0);
	LINA_FIELD_VEC(Font::Metadata, points, "Points", FieldType::UserClass, Font::FontPoint, GetTypeID<Font::FontPoint>());
	LINA_FIELD_VEC(Font::Metadata, glyphRanges, "Extra Ranges", FieldType::UserClass, Font::GlyphRange, GetTypeID<Font::GlyphRange>());
	LINA_CLASS_END(FontMeta)

} // namespace Lina
