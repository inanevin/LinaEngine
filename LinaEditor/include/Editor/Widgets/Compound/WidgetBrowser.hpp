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
#include "Core/GUI/Widgets/Compound/FileMenu.hpp"

namespace Lina
{
	class DirectionalLayout;
} // namespace Lina

namespace Lina::Editor
{
	class Editor;
	class ItemController;
	class PanelWidgetEditor;

	class WidgetBrowser : public Widget, public FileMenuListener
	{

	public:
		WidgetBrowser() : Widget() {};
		virtual ~WidgetBrowser() = default;

		virtual void Construct() override;

		void RefreshWidgets();
		void DropPayload(Widget* w);
		void SetRoot(Widget* w);

		inline void SetWidgetEditor(PanelWidgetEditor* editor)
		{
			m_panelWidgetEditor = editor;
		}

		inline ItemController* GetController() const
		{
			return m_controller;
		}

	private:
		void AddItem(Widget* parent, Widget* w, float margin);
		void RequestRename(Widget* w);
		bool ContainsSearchStrRecursive(Widget* w);

	protected:
		virtual bool OnFileMenuItemClicked(FileMenu* filemenu, StringID sid, void* userData) override;
		virtual void OnFileMenuGetItems(FileMenu* filemenu, StringID sid, Vector<FileMenuItem::Data>& outData, void* userData) override;

	private:
		PanelWidgetEditor* m_panelWidgetEditor = nullptr;
		Widget*			   m_root			   = nullptr;
		Vector<Widget*>	   m_payloadItems;
		Editor*			   m_editor		= nullptr;
		ItemController*	   m_controller = nullptr;
		DirectionalLayout* m_layout		= nullptr;
		String			   m_searchStr	= "";
		uint32			   m_itemCtr	= 0;
	};

	LINA_WIDGET_BEGIN(WidgetBrowser, Hidden)
	LINA_CLASS_END(WidgetBrowser)
} // namespace Lina::Editor
