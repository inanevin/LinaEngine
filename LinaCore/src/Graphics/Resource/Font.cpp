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
#include "Common/Serialization/VectorSerialization.hpp"
#include "Common/FileSystem/FileSystem.hpp"
#include "Common/Data/Streams.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Common/System/System.hpp"
#include "Core/Graphics/GfxManager.hpp"
#include "Core/Resources/ResourceManager.hpp"

namespace Lina
{
	void Font::Metadata::SaveToStream(OStream& out) const
	{
		out << static_cast<int32>(points.size());
		for (auto p : points)
			out << p.size << p.dpiLimit;
		out << isSDF;
		out << static_cast<int32>(glyphRanges.size());

		for (const auto& rangePair : glyphRanges)
		{
			out << rangePair.first;
			out << rangePair.second;
		}
	}

	void Font::Metadata::LoadFromStream(IStream& in)
	{
		int32 pointSizeSz = 0;
		in >> pointSizeSz;
		points.resize(pointSizeSz);
		for (int32 i = 0; i < pointSizeSz; i++)
			in >> points[i].size >> points[i].dpiLimit;

		in >> isSDF;

		int32 glyphRangeSize = 0;

		in >> glyphRangeSize;

		if (glyphRangeSize != 0)
		{
			glyphRanges.resize(static_cast<size_t>(glyphRangeSize));
			for (int32 i = 0; i < glyphRangeSize; i++)
			{
				in >> glyphRanges[i].first;
				in >> glyphRanges[i].second;
			}
		}
	}

	Font::~Font()
	{
		for (auto* font : m_lvgFonts)
			delete font;
		m_lvgFonts.clear();
	}

	void Font::BatchLoaded()
	{
		Vector<LinaVG::GlyphEncoding> customRangeVec;
		for (const auto& rng : m_meta.glyphRanges)
		{
			customRangeVec.push_back(rng.first);
			customRangeVec.push_back(rng.second);
		}

		const int32 sz = static_cast<int32>(m_meta.points.size());
		m_lvgFonts.resize(sz);
		for (int32 i = 0; i < sz; i++)
		{
			LinaVG::Text* lvgText = GfxManager::GetLVGText();

			if (customRangeVec.empty())
				m_lvgFonts[i] = lvgText->LoadFontFromMemory(m_file.data(), m_file.size(), m_meta.isSDF, m_meta.points[i].size);
			else
				m_lvgFonts[i] = lvgText->LoadFontFromMemory(m_file.data(), m_file.size(), m_meta.isSDF, m_meta.points[i].size, customRangeVec.data(), static_cast<int32>(m_meta.glyphRanges.size()) * 2);
		}
	}

	void Font::LoadFromFile(const char* path)
	{
		// Populate if not existing.
		FileSystem::ReadFileContentsToVector(path, m_file);
	}

	void Font::LoadFromStream(IStream& stream)
	{
		m_meta.LoadFromStream(stream);
		VectorSerialization::LoadFromStream_PT(stream, m_file);
	}

	void Font::SaveToStream(OStream& stream) const
	{
		m_meta.SaveToStream(stream);
		VectorSerialization::SaveToStream_PT(stream, m_file);
	}

	LinaVG::LinaVGFont* Font::GetLinaVGFont(float dpiScale)
	{
		const int32 sz = static_cast<int32>(m_meta.points.size());

		for (int32 i = 0; i < sz; i++)
		{
			if (dpiScale < m_meta.points[i].dpiLimit)
				return m_lvgFonts[i];
		}

		return m_lvgFonts.back();
	}

} // namespace Lina
