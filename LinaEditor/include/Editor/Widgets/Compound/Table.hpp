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
	class DirectionalLayout;
	class Text;
	class ScrollArea;
} // namespace Lina
namespace Lina::Editor
{
	class ItemController;

	class Table : public Widget
	{
	public:
		struct HeaderDefinition
		{
			String text		 = "";
			bool   clickable = false;
		};

		struct Column
		{
			Text*			   columnText	  = nullptr;
			DirectionalLayout* verticalLayout = nullptr;
			DirectionalLayout* contents		  = nullptr;
		};

		Table()			 = default;
		virtual ~Table() = default;

		struct Properties
		{
			bool				   clipChildren	  = true;
			bool				   displayHeaders = true;
			bool				   useRowWrapper  = true;
			Color				   colorRow		  = Theme::GetDef().background1;
			Color				   colorRowAlt	  = Theme::GetDef().background2;
			Delegate<void(uint32)> onColumnClicked;
		};

		virtual void Construct() override;

		void BuildHeaders(const Vector<HeaderDefinition>& headerDef);
		void ClearRows();
		void AddRow(const Vector<Widget*>& columns);

		inline DirectionalLayout* GetHorizontalLayout() const
		{
			return m_layout;
		}

		inline Properties& GetProps()
		{
			return m_props;
		}

	private:
		Properties		   m_props		= {};
		DirectionalLayout* m_layout		= nullptr;
		ItemController*	   m_controller = nullptr;
		Vector<Column>	   m_columns;
		ScrollArea*		   m_scroll = nullptr;
	};

	LINA_WIDGET_BEGIN(Table, Hidden)
	LINA_CLASS_END(Table)

} // namespace Lina::Editor
