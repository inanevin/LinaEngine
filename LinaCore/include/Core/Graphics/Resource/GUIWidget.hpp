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
#include "Core/GUI/Widgets/Widget.hpp"

namespace Lina
{
	class GUIWidget : public Resource
	{

	public:
		GUIWidget(const String& path, StringID sid) : Resource(path, sid, GetTypeID<GUIWidget>()){};
		GUIWidget(ResourceID id = 0) : Resource(id){};

		virtual ~GUIWidget();
		virtual void LoadFromFile(const char* path) override;
		virtual void SaveToStream(OStream& stream) const override;
		virtual void LoadFromStream(IStream& stream) override;
		void		 UpdateBlob();
		void		 ClearRoot();

		inline Widget& GetRoot()
		{
			return m_root;
		}

		inline void CopyRootBlob(IStream& outStream)
		{
			if (m_loadedStream.Empty())
			{
				OStream stream;
				m_root.SaveToStream(stream);
				m_loadedStream.Create(stream.GetDataRaw(), stream.GetCurrentSize());
				stream.Destroy();
			}

			outStream.Destroy();
			outStream.Create(m_loadedStream.GetDataRaw(), m_loadedStream.GetSize());
		}

	private:
	private:
		ALLOCATOR_BUCKET_MEM;
		Widget	m_root;
		IStream m_loadedStream;
	};

	LINA_REFLECTRESOURCE_BEGIN(GUIWidget);
	LINA_REFLECTRESOURCE_END(GUIWidget);

} // namespace Lina
