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

#include "Core/Graphics/Resource/Font.hpp"

#include "Common/FileSystem/FileSystem.hpp"
#include "Common/Data/Streams.hpp"
#include "Common/Platform/LinaVGIncl.hpp"

#include "Core/Resources/ResourceManager.hpp"

namespace Lina
{
	void Font::Metadata::SaveToStream(OStream& out) const
	{
		out << static_cast<int32>(points.size());
		for (auto p : points)
			out << p.size << p.scaleLimit;
		out << isSDF;
		out << static_cast<int32>(glyphRanges.size());

		for (const auto& range : glyphRanges)
		{
			out << range.start;
			out << range.end;
		}
	}

	void Font::Metadata::LoadFromStream(IStream& in)
	{
		int32 pointSizeSz = 0;
		in >> pointSizeSz;
		points.resize(pointSizeSz);
		for (int32 i = 0; i < pointSizeSz; i++)
			in >> points[i].size >> points[i].scaleLimit;

		in >> isSDF;

		int32 glyphRangeSize = 0;

		in >> glyphRangeSize;

		if (glyphRangeSize != 0)
		{
			glyphRanges.resize(static_cast<size_t>(glyphRangeSize));
			for (int32 i = 0; i < glyphRangeSize; i++)
			{
				in >> glyphRanges[i].start;
				in >> glyphRanges[i].end;
			}
		}
	}

	Font::~Font()
	{
	}

	void Font::DestroyHW()
	{
		LINA_ASSERT(m_hwValid, "");
		m_hwValid		= false;
		m_hwUploadValid = false;

		for (LinaVG::Font* font : m_hwLvgFonts)
		{
			font->atlas->RemoveFont(font->atlasRectPos, font->atlasRectHeight);
			font->atlas = nullptr;
			delete font;
		}

		m_hwLvgFonts.clear();
	}

	size_t Font::GetSize() const
	{
		size_t total = m_file.size();
		for (LinaVG::Font* font : m_hwLvgFonts)
			total += font->structSizeInBytes;
		return total;
	}

	void Font::GenerateHW()
	{
		LINA_ASSERT(m_hwValid == false, "");
		m_hwValid = true;

		Vector<LinaVG::GlyphEncoding> customRangeVec;
		for (const auto& rng : m_meta.glyphRanges)
		{
			customRangeVec.push_back(rng.start);
			customRangeVec.push_back(rng.end);
		}

		const int32 sz = static_cast<int32>(m_meta.points.size());
		m_hwLvgFonts.resize(sz);

		for (int32 i = 0; i < sz; i++)
		{
			if (customRangeVec.empty())
				m_hwLvgFonts[i] = LinaVG::Text::LoadFontFromMemory(m_file.data(), m_file.size(), m_meta.isSDF, m_meta.points[i].size);
			else
				m_hwLvgFonts[i] = LinaVG::Text::LoadFontFromMemory(m_file.data(), m_file.size(), m_meta.isSDF, m_meta.points[i].size, customRangeVec.data(), static_cast<int32>(m_meta.glyphRanges.size()) * 2);
		}
	}

	void Font::Upload(LinaVG::Text& lvgText)
	{
		for (LinaVG::Font* font : m_hwLvgFonts)
			lvgText.AddFontToAtlas(font);

		m_hwUploadValid = true;
	}

	bool Font::LoadFromFile(const String& path)
	{
		if (!FileSystem::FileOrPathExists(path))
			return false;

		m_file.clear();
		FileSystem::ReadFileContentsToVector(path.c_str(), m_file);

		if (m_file.empty())
			return false;

		return true;
	}

	void Font::LoadFromStream(IStream& stream)
	{
		Resource::LoadFromStream(stream);
		uint32 version = 0;
		stream >> version;
		stream >> m_meta;
		stream >> m_file;
	}

	void Font::SaveToStream(OStream& stream) const
	{
		Resource::SaveToStream(stream);
		stream << VERSION;
		stream << m_meta;
		stream << m_file;
	}

	LinaVG::Font* Font::GetFont(float scaleFactor)
	{
		const int32 sz = static_cast<int32>(m_meta.points.size());

		for (int32 i = 0; i < sz; i++)
		{
			if (scaleFactor < m_meta.points[i].scaleLimit)
				return m_hwLvgFonts[i];
		}

		return m_hwLvgFonts.back();
	}

	void Font::SaveLVGFontToStream(OStream& stream, LinaVG::Font* font) const
	{
		stream << font->size;
		stream << font->newLineHeight;
		stream << font->spaceAdvance;
		stream << font->isSDF;
		stream << font->supportsKerning;
		stream << font->atlasRectHeight;

		const uint32 glyphsSize = static_cast<uint32>(font->glyphs.size());
		stream << glyphsSize;

		for (const auto& [glyph, ch] : font->glyphs)
		{
			stream << glyph;
			stream << ch.m_advance.x;
			stream << ch.m_advance.y;
			stream << ch.m_ascent;
			stream << ch.m_bearing.x;
			stream << ch.m_bearing.y;
			stream << ch.m_descent;
			stream << ch.m_size.x;
			stream << ch.m_size.y;
			stream << ch.m_uv12.x;
			stream << ch.m_uv12.y;
			stream << ch.m_uv34.x;
			stream << ch.m_uv34.y;

			if (ch.m_buffer != nullptr)
				stream.WriteRaw(ch.m_buffer, static_cast<size_t>(ch.m_size.x) * static_cast<size_t>(ch.m_size.y));
		}

		const uint32 kerningSize = static_cast<uint32>(font->kerningTable.size());
		stream << kerningSize;

		for (const auto& [glyph, kerning] : font->kerningTable)
		{
			stream << glyph;

			const uint32 xAdvancesSize = static_cast<uint32>(kerning.xAdvances.size());
			stream << xAdvancesSize;

			for (auto [adv1, adv2] : kerning.xAdvances)
			{
				stream << adv1;
				stream << adv2;
			}
		}
	}

	void Font::LoadLVGFontToStream(IStream& stream, LinaVG::Font* font)
	{
		stream >> font->size;
		stream >> font->newLineHeight;
		stream >> font->spaceAdvance;
		stream >> font->isSDF;
		stream >> font->supportsKerning;
		stream >> font->atlasRectHeight;

		uint32 glyphsSize = 0;
		stream >> glyphsSize;

		for (uint32 i = 0; i < glyphsSize; i++)
		{
			LinaVG::GlyphEncoding glpyh = 0;
			stream >> glpyh;
			auto& ch = font->glyphs[glpyh];

			stream >> ch.m_advance.x;
			stream >> ch.m_advance.y;
			stream >> ch.m_ascent;
			stream >> ch.m_bearing.x;
			stream >> ch.m_bearing.y;
			stream >> ch.m_descent;
			stream >> ch.m_size.x;
			stream >> ch.m_size.y;
			stream >> ch.m_uv12.x;
			stream >> ch.m_uv12.y;
			stream >> ch.m_uv34.x;
			stream >> ch.m_uv34.y;

			const size_t sz = static_cast<size_t>(ch.m_size.x * ch.m_size.y);

			if (sz != 0)
			{
				ch.m_buffer = (unsigned char*)malloc(sz);
				stream.ReadToRaw(ch.m_buffer, sz);
			}
		}

		uint32 kerningSize = 0;
		stream >> kerningSize;

		for (uint32 i = 0; i < kerningSize; i++)
		{
			unsigned long glyph = 0;
			stream >> glyph;
			auto& kerning = font->kerningTable[glyph];

			uint32 xAdvSize = 0;
			stream >> xAdvSize;

			for (uint32 j = 0; j < xAdvSize; j++)
			{
				unsigned long adv1 = 0;
				unsigned long adv2 = 0;
				stream >> adv1;
				stream >> adv2;
				kerning.xAdvances[adv1] = adv2;
			}
		}
	}

} // namespace Lina
