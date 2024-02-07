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

#ifndef LinaFont_HPP
#define LinaFont_HPP

#include "Core/Resources/Resource.hpp"
#include "Common/Data/HashMap.hpp"
#include "Common/Data/Vector.hpp"
#include "Core/Resources/Data/ResourceMetadata.hpp"
#include "Common/Data/Mutex.hpp"
#include "Common/Data/CommonData.hpp"

namespace LinaVG
{
	class LinaVGFont;
}

namespace Lina
{
#define FONT_META_SIZE			"Size"_hs
#define FONT_META_ISSDF			"IsSDF"_hs
#define FONT_META_CUSTOM_GLPYHS "CustomGlyphRanges"_hs

	class Font : public Resource
	{

	public:
		Font(ResourceManager* rm, bool isUserManaged, const String& path, StringID sid) : Resource(rm, isUserManaged, path, sid, GetTypeID<Font>()){};
		virtual ~Font();

		struct Metadata
		{
			uint32						 pointSize = 16;
			bool						 isSDF	   = false;
			Vector<Pair<uint32, uint32>> glyphRanges;

			void SaveToStream(OStream& out) const;
			void LoadFromStream(IStream& in);
		};

		inline LinaVG::LinaVGFont* GetLinaVGFont()
		{
			return m_lvgFont;
		}

	protected:
		virtual void BatchLoaded() override;
		virtual void LoadFromFile(const char* path) override;
		virtual void LoadFromStream(IStream& stream) override;
		virtual void SaveToStream(OStream& stream) override;
		virtual void SetCustomMeta(IStream& stream) override;

	private:
		LinaVG::LinaVGFont* m_lvgFont = nullptr;
		Vector<char>		m_file;

		Metadata m_meta = {};
	};
} // namespace Lina

#endif
