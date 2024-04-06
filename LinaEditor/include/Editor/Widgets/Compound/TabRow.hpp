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
			Delegate<void(Widget* tiedWidget)> onTabClosed;
			Delegate<void(Widget* tiedWidget)> onTabDockedOut;
			Delegate<void(Widget* tiedWidget)> onSelectionChanged;
		};

		virtual void Tick(float delta) override;
		virtual void Draw(int32 threadIndex) override;

		void AddTab(Widget* tiedWidget);
		void RemoveTab(Widget* tiedWidget);
		void SetSelected(Widget* tiedWidget);
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

	private:
		friend class Tab;

		void SelectionChanged(Widget* tiedWidget);
		void Close(Widget* tiedWidget);
		void DockOut(Widget* tiedWidget);

	private:
		bool		 m_isSelected = false;
		bool		 m_anyPressed = false;
		Properties	 m_props	  = {};
		Vector<Tab*> m_tabs;
		bool		 m_canCloseTabs = true;
	};

} // namespace Lina::Editor
