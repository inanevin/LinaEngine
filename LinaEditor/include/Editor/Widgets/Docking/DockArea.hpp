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

#include "DockWidget.hpp"
#include "Editor/CommonEditor.hpp"
#include "Editor/Editor.hpp"

namespace Lina
{
	class DirectionalLayout;
}

namespace Lina::Editor
{
	class DockPreview;
	class TabRow;
	class Panel;

	class DockArea : public DockWidget, public EditorPayloadListener
	{
	public:
		DockArea() : DockWidget(){};
		virtual ~DockArea() = default;

		virtual void			Construct() override;
		virtual void			Destruct() override;
		virtual void			PreTick() override;
		virtual void			Tick(float delta) override;
		virtual void			Draw() override;
		virtual void			OnPayloadStarted(PayloadType type, Widget* payload) override;
		virtual void			OnPayloadEnded(PayloadType type, Widget* payload) override;
		virtual bool			OnPayloadDropped(PayloadType type, Widget* payload) override;
		virtual LinaGX::Window* OnPayloadGetWindow() override
		{
			return m_lgxWindow;
		}

		void AddPanel(Panel* w);
		void RemovePanel(Panel* w);
		void FixAreaChildMargins();

		DockArea* AddDockArea(Direction direction, Panel* panel);
		void	  RemoveArea();
		void	  SetSelected(Widget* w);

		const Vector<Panel*>& GetPanels() const
		{
			return m_panels;
		}

	private:
		void ExpandWidgetsToMyPlace(const Vector<Widget*>& widgets, Direction directionOfAreas);

	private:
		TabRow*					   m_tabRow	 = nullptr;
		DockPreview*			   m_preview = nullptr;
		DirectionalLayout*		   m_layout	 = nullptr;
		Vector<Panel*>			   m_panels;
		Widget*					   m_selectedPanel = nullptr;
		bool					   m_payloadActive = false;
		Vector<Pair<Panel*, bool>> m_panelKillList;
	};

	LINA_REFLECTWIDGET_BEGIN(DockArea, Editor)
	LINA_REFLECTWIDGET_END(DockArea)

} // namespace Lina::Editor
