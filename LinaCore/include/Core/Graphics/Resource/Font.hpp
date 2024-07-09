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
#include "Common/Data/HashMap.hpp"
#include "Common/Data/Vector.hpp"
#include "Core/Resources/Data/ResourceMetadata.hpp"
#include "Common/Data/Mutex.hpp"
#include "Common/Data/CommonData.hpp"

namespace LinaVG
{
	class LinaVGFont;
	class Text;
} // namespace LinaVG

namespace Lina
{
	class Font : public Resource
	{
	public:
		struct FontPoint
		{
			uint32 size		= 12;
			float  dpiLimit = 10.0f;
		};

		struct Metadata
		{
			Vector<FontPoint>			 points = {FontPoint()};
			bool						 isSDF	= false;
			Vector<Pair<uint32, uint32>> glyphRanges;

			void SaveToStream(OStream& out) const;
			void LoadFromStream(IStream& in);
		};

		Font(const String& path, StringID sid) : Resource(path, sid, GetTypeID<Font>()){};
		virtual ~Font();

		void				GenerateHW(LinaVG::Text& lvgText);
		LinaVG::LinaVGFont* GetLinaVGFont(float dpiScale);
		virtual void		LoadFromFile(const char* path) override;
		virtual void		LoadFromStream(IStream& stream) override;
		virtual void		SaveToStream(OStream& stream) const override;

		virtual void SetCustomMeta(IStream& stream) override
		{
			m_meta.LoadFromStream(stream);
		}

	private:
		ALLOCATOR_BUCKET_MEM;

		Vector<LinaVG::LinaVGFont*> m_lvgFonts = {};
		Vector<char>				m_file;
		Metadata					m_meta = {};
	};

	LINA_REFLECTRESOURCE_BEGIN(Font);
	LINA_REFLECTRESOURCE_END(Font);
} // namespace Lina
