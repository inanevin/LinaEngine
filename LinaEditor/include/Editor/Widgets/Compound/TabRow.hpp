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
#include "Common/Common.hpp"

namespace Lina::Editor
{
	class Tab;

	class TabRow : public Widget
	{
	public:
		TabRow()		  = default;
		virtual ~TabRow() = default;

		struct Properties
		{
			Delegate<void(void* userData)> onTabClosed;
			Delegate<void(void* userData)> onTabDockedOut;
			Delegate<void(void* userData)> onTabSelected;

			void SaveToStream(OStream& stream) const
			{
			}

			void LoadFromStream(IStream& stream)
			{
			}
		};

		virtual void PreTick() override;
		virtual void Tick(float delta) override;

		void AddTab(void* userData, const String& title, bool isFixed = false);
		void RemoveTab(void* userData);
		void SetSelected(void* userData);
		void SetCanCloseTabs(bool canCloseTabs);

		inline Properties& GetProps()
		{
			return m_props;
		}

		inline bool GetAnyPressed() const
		{
			return m_anyPressed;
		}

		inline bool GetCanCloseTabs() const
		{
			return m_canCloseTabs;
		}

		virtual void SaveToStream(OStream& stream) const override
		{
			Widget::SaveToStream(stream);
			stream << m_props;
		}

		virtual void LoadFromStream(IStream& stream) override
		{
			Widget::LoadFromStream(stream);
			stream >> m_props;
		}

	private:
		friend class Tab;

		void SelectionChanged(Tab* tab);

	private:
		bool		 m_isSelected = false;
		bool		 m_anyPressed = false;
		Properties	 m_props	  = {};
		Vector<Tab*> m_tabs;
		bool		 m_canCloseTabs = true;
	};

	LINA_WIDGET_BEGIN(TabRow, Compound)
	LINA_CLASS_END(TabRow)

} // namespace Lina::Editor
