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

#include "Graphics/Resource/Font.hpp"
#include "Common/Serialization/VectorSerialization.hpp"
#include "Common/FileSystem/FileSystem.hpp"
#include "Common/Data/Streams.hpp"
#include "Common/Platform/LinaVGIncl.hpp"

namespace Lina
{
	Font::~Font()
	{
		if (m_lvgFont)
			delete m_lvgFont;
	}

	void Font::BatchLoaded()
	{
		const bool					  isSdf		   = m_metadata.GetBool(FONT_META_ISSDF, false);
		const int					  size		   = m_metadata.GetInt(FONT_META_SIZE);
		const int					  customRanges = m_metadata.GetInt(FONT_META_CUSTOM_GLPYHS, 0);
		Vector<LinaVG::GlyphEncoding> customRangeVec;
		for (int i = 0; i < customRanges; i += 2)
		{
			const String   range0	 = "Range_" + TO_STRING(i);
			const String   range1	 = "Range_" + TO_STRING(i + 1);
			const StringID range0Sid = TO_SID(range0);
			const StringID range1Sid = TO_SID(range1);
			customRangeVec.push_back(m_metadata.GetInt(range0Sid));
			customRangeVec.push_back(m_metadata.GetInt(range1Sid));
		}

		if (customRangeVec.empty())
			m_lvgFont = LinaVG::LoadFontFromMemory(m_file.data(), m_file.size(), isSdf, size);
		else
			m_lvgFont = LinaVG::LoadFontFromMemory(m_file.data(), m_file.size(), isSdf, size, customRangeVec.data(), customRanges);
	}

	void Font::LoadFromFile(const char* path)
	{
		// Popuplate if not existing.
		m_metadata.GetBool(FONT_META_ISSDF, false);
		m_metadata.GetInt(FONT_META_SIZE, 12);
		FileSystem::ReadFileContentsToVector(path, m_file);
	}

	void Font::LoadFromStream(IStream& stream)
	{
		m_metadata.LoadFromStream(stream);
		VectorSerialization::LoadFromStream_PT(stream, m_file);
	}

	void Font::SaveToStream(OStream& stream)
	{
		m_metadata.SaveToStream(stream);
		VectorSerialization::SaveToStream_PT(stream, m_file);
	}

} // namespace Lina
