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

#include "Core/GUI/Widgets/Widget.hpp"
#include "Editor/CommonEditor.hpp"

namespace Lina::Editor
{
	class Tab;

	struct PanelPayloadData
	{
		PanelType type		= PanelType::Entities;
		StringID  subData	= 0;
		Vector2	  panelSize = Vector2::Zero;
		String	  panelName = "";
	};

	enum PanelFlags
	{
		PF_NONE			  = 1 << 0,
		PF_FLOATING_POPUP = 1 << 1,
	};

	class Panel : public Widget
	{
	public:
		Panel() = default;
		Panel(PanelType type, uint32 flags = 0) : m_panelType(type), m_panelFlags(flags), Widget(){};
		virtual ~Panel() = default;

		virtual void SaveLayoutToStream(OStream& stream){};
		virtual void LoadLayoutFromStream(IStream& stream){};
		virtual void Destruct() override;
		void		 RefreshTab();

		inline PanelType GetType() const
		{
			return m_panelType;
		}

		inline ResourceID GetSubData() const
		{
			return m_subData;
		}

		inline const Bitmask32& GetPanelFlags() const
		{
			return m_panelFlags;
		}

		inline void SetSubdata(ResourceID data)
		{
			m_subData = data;
		}

		inline void SetTab(Tab* tab)
		{
			m_tab = tab;
		}

	protected:
		PanelType  m_panelType	= PanelType::ResourceBrowser;
		ResourceID m_subData	= 0;
		Bitmask32  m_panelFlags = 0;
		Tab*	   m_tab		= nullptr;
	};

	LINA_WIDGET_BEGIN(Panel, Hidden)
	LINA_CLASS_END(Panel)

} // namespace Lina::Editor
