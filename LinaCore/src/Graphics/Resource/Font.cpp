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
#include "Serialization/VectorSerialization.hpp"
#include "FileSystem/FileSystem.hpp"
#include "Data/Streams.hpp"
#include "Graphics/Platform/LinaVGIncl.hpp"

namespace Lina
{
	void Font::Flush()
	{
		
	}

	void Font::BatchLoaded()
	{
		const int vars = m_metadata.GetInt("VariationCount"_hs, 1);

		for (int i = 0; i < vars; i++)
		{
			const bool isSdf = m_metadata.GetBool("IsSDF"_hs);
			const int  size	 = m_metadata.GetInt("Size"_hs);

			LinaVG::LoadFontFromMemory(m_file.data(), m_file.size(), isSdf, m_sid, size);
			m_file.clear();
		}
	}

	void Font::LoadFromFile(const char* path)
	{
		// Popuplate if not existing.
		m_metadata.GetBool("IsSDF"_hs, false);
		m_metadata.GetInt("Size"_hs, 12);
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
