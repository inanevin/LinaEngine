
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

#include "Editor/Widgets/Panel/PanelResourceViewer.hpp"
#include "Core/Graphics/Resource/GUIWidget.hpp"

namespace Lina
{
	class Text;
} // namespace Lina

namespace Lina::Editor
{
	class Editor;
	class WidgetBrowser;

	class PanelWidgetEditor : public PanelResourceViewer
	{
	public:
		PanelWidgetEditor() : PanelResourceViewer(PanelType::WidgetEditor, GetTypeID<GUIWidget>(), GetTypeID<PanelWidgetEditor>()) {};
		virtual ~PanelWidgetEditor() = default;

		virtual void Construct() override;
		virtual void Initialize() override;
		virtual void Draw() override;

		virtual void StoreEditorActionBuffer() override;
		virtual void RebuildContents() override;

		void SetSelectedWidgets(const Vector<Widget*>& selection);
		void ChangeSelection(const Vector<Widget*>& selection);

		void Duplicate(const Vector<Widget*>& widgets);
		void Delete(const Vector<Widget*>& widgets);
		void Parent(const Vector<Widget*>& widgets, Widget* parent);
		void RefreshBrowser();
		void AddNew(Widget* w, Widget* parent);
		void Rename(Widget* w, const String& name);
		void FetchRootFromResource();

		inline Widget* GetRoot() const
		{
			return m_root;
		}

		inline const Vector<Widget*>& GetSelection() const
		{
			return m_selectedWidgets;
		}

	private:
		void RefreshResources();

	private:
		LINA_REFLECTION_ACCESS(PanelWidgetEditor);
		WidgetBrowser*	m_browser		  = nullptr;
		Vector<Widget*> m_selectedWidgets = {};
		OStream			m_storedStream	  = {};
		uint32			m_uniqueIDCounter = 0;
		Widget*			m_root			  = nullptr;
	};

	LINA_WIDGET_BEGIN(PanelWidgetEditor, Hidden)
	LINA_CLASS_END(PanelWidgetEditor)

} // namespace Lina::Editor
