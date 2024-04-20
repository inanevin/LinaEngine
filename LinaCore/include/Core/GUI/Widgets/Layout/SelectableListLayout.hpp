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

namespace Lina
{

	struct SelectableListItem
	{
		String title		 = "";
		uint8  level		 = 0;
		void*  userData		 = nullptr;
		bool   hasChildren	 = false;
		bool   startUnfolded = false;
	};

	class SelectableListLayoutListener
	{
	public:
		virtual void SelectableListFillItems(Vector<SelectableListItem>& outItems){};
		virtual void SelectableListFillSubItems(Vector<SelectableListItem>& outItems, void* parentUserData){};

		virtual void SelectableListOnPayload(Widget* payload){};
		virtual void SelectableListContextMenu(void* userData){};
	};

	class SelectableListLayout : public Widget
	{
	public:
		SelectableListLayout()			= default;
		virtual ~SelectableListLayout() = default;

		struct Properties
		{
			bool   useGridAsLayout		= false;
			String dropdownIconFolded	= "";
			String dropdownIconUnfolded = "";
		};

		virtual void Construct();
		virtual void Initialize();

		void RefreshItems();

		inline void SetListener(SelectableListLayoutListener* listener)
		{
			m_listener = listener;
		}

		inline Properties& GetProps()
		{
			return m_props;
		}

	private:
		Widget* CreateItem(const SelectableListItem& item, uint8 level);

	private:
		Properties					  m_props	 = {};
		SelectableListLayoutListener* m_listener = nullptr;
		Widget*						  m_layout	 = nullptr;
	};

} // namespace Lina
