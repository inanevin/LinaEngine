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

#include "Editor/Widgets/Panel/Panel.hpp"
#include "Core/GUI/Widgets/Compound/FileMenu.hpp"

namespace Lina
{
	class GUIWidget;
	class DirectionalLayout;
} // namespace Lina

namespace Lina::Editor
{
	class Editor;
	class ItemController;
	class PanelWidgetEditorProperties;

	class PanelWidgetEditor : public Panel, public FileMenuListener
	{
	private:
		struct WidgetInfo
		{
			String title = "";
			TypeID tid	 = 0;
		};

		struct CategoryInfo
		{
			String			   title = "";
			Vector<WidgetInfo> widgets;
		};

	public:
		PanelWidgetEditor() : Panel(PanelType::WidgetEditor, 0){};
		virtual ~PanelWidgetEditor() = default;

		virtual void Construct() override;
		virtual void PreDestruct() override;
		virtual void Initialize() override;
		virtual void Tick(float delta) override;
		virtual void LoadLayoutFromStream(IStream& stream) override;
		virtual void SaveLayoutToStream(OStream& stream) override;
		virtual bool OnFileMenuItemClicked(FileMenu* filemenu, StringID sid, void* userData) override;
		virtual void OnFileMenuGetItems(FileMenu* filemenu, StringID sid, Vector<FileMenuItem::Data>& outData, void* userData) override;

	private:
		void	RefreshWidgets();
		void	RefreshHierarchy();
		void	CheckSaveCurrent(Delegate<void()>&& onAct);
		Widget* BuildDirectorySelector();
		void	AddItemForWidget(Widget* rootInEditor, Widget* sourceWidget, float margin);
		void	RequestDelete(Vector<Widget*> widgets);
		void	RequestDuplicate(Vector<Widget*> widgets);
		void	RequestRename(Widget* w);
		void	OpenWidget(ResourceID id);
		void	CloseCurrent(bool save);

	private:
		ResourceID					 m_lastOpenWidget = 0;
		Widget*						 m_gridParent	  = nullptr;
		Vector<CategoryInfo>		 m_categories;
		TypeID						 m_payloadCarryTID	   = 0;
		Editor*						 m_editor			   = nullptr;
		GUIWidget*					 m_currentWidget	   = nullptr;
		ItemController*				 m_widgetsController   = nullptr;
		ItemController*				 m_hierarchyController = nullptr;
		DirectionalLayout*			 m_widgetsLayout	   = nullptr;
		DirectionalLayout*			 m_hierarchyLayout	   = nullptr;
		PanelWidgetEditorProperties* m_propertiesArea	   = nullptr;

		Widget* m_leftSide	  = nullptr;
		Widget* m_leftSideTop = nullptr;
		Widget* m_leftSideBot = nullptr;
		Widget* m_middle	  = nullptr;
		Widget* m_rightSide	  = nullptr;
	};

	LINA_WIDGET_BEGIN(PanelWidgetEditor, Hidden)
	LINA_CLASS_END(PanelWidgetEditor)

} // namespace Lina::Editor
