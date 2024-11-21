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

#include "Common/Data/String.hpp"
#include "Common/Data/Vector.hpp"
#include "Common/StringID.hpp"
#include "Common/Data/Streams.hpp"

#include "Editor/CommonEditor.hpp"

namespace Lina::Editor
{
	class Editor;

	class EditorLayout
	{
	public:
		static constexpr uint32 VERSION = 0;

		virtual ~EditorLayout();
		void ClearBuffers();

		struct PanelData
		{
			PanelType	panelType  = PanelType::ResourceBrowser;
			StringID	subData	   = 0;
			Span<uint8> layoutData = {};

			inline void SaveToStream(OStream& out) const
			{
				out << static_cast<uint8>(panelType);
				out << subData;
				const uint32 sz = static_cast<uint32>(layoutData.size());
				out << sz;
				if (layoutData.size() > 0)
					out.WriteRaw(layoutData);
			}

			inline void LoadFromStream(IStream& in)
			{
				uint8 pt = 0;
				in >> pt;
				in >> subData;
				panelType = static_cast<PanelType>(pt);

				uint32 dataSize = 0;
				in >> dataSize;

				if (dataSize != 0)
				{
					layoutData = {new uint8[static_cast<size_t>(dataSize)], static_cast<size_t>(dataSize)};
					in.ReadToRaw(layoutData);
				}
			}
		};

		struct DockWidgetData
		{
			Vector2			  alignedPos   = Vector2::Zero;
			Vector2			  alignedSize  = Vector2::Zero;
			bool			  isBorder	   = false;
			bool			  isHorizontal = false;
			Vector<PanelData> panels;

			inline void SaveToStream(OStream& out) const
			{
				out << alignedPos << alignedSize;
				out << isBorder;
				out << isHorizontal;
				out << panels;
			}

			inline void LoadFromStream(IStream& in)
			{
				in >> alignedPos >> alignedSize;
				in >> isBorder;
				in >> isHorizontal;
				in >> panels;
			}
		};

		struct WindowLayout
		{
			StringID			   sid		= 0;
			Vector2i			   position = Vector2i::Zero;
			Vector2ui			   size		= Vector2ui::Zero;
			String				   title	= "";
			Vector<DockWidgetData> dockWidgets;

			inline void SaveToStream(OStream& out) const
			{
				out << sid;
				position.SaveToStream(out);
				size.SaveToStream(out);
				out << title;
				out << dockWidgets;
			}

			inline void LoadFromStream(IStream& in)
			{
				in >> sid;
				position.LoadFromStream(in);
				size.LoadFromStream(in);
				in >> title;
				in >> dockWidgets;
			}
		};

		virtual void SaveToStream(OStream& out);
		virtual void LoadFromStream(IStream& in);

		void			 ApplyStoredLayout();
		void			 StoreLayout();
		void			 StoreDefaultLayout();
		const PanelData& FindPanelData(PanelType type);

	private:
		Vector<WindowLayout> m_windows;
	};

} // namespace Lina::Editor
