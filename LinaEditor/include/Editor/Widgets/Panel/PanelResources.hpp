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

namespace Lina
{
	class LayoutBorder;
	class Text;
	class DirectionalLayout;
	class GridLayout;
	class Selectable;
} // namespace Lina

namespace Lina::Editor
{
	class DirectoryItem;
	class Editor;

	class PanelResources : public Panel
	{
	public:
		static constexpr float MAX_CONTENTS_SIZE = 4.0f;

		PanelResources() : Panel(PanelType::Resources, 0){};
		virtual ~PanelResources() = default;

		virtual void Construct() override;
		virtual void Draw(int32 threadIndex) override;

		virtual PanelLayoutExtra GetExtraLayoutData() override;
		virtual void			 SetExtraLayoutData(const PanelLayoutExtra& data) override;

	private:
		void	RefreshBrowserHierarchy();
		void	RefreshContents();
		Widget* CreateSelectable(DirectoryItem* item, uint8 level);
		void	UpdateWidgetSizeFromContentsSize(Widget* w);
		Widget* BuildThumbnailForItem(DirectoryItem* item);
		Widget* BuildTooltipForItem(void* userData);
		Widget* BuildTitleForItem(DirectoryItem* item);
		Widget* BuildFolderIconForItem(DirectoryItem* item);
		Widget* BuildTopContents();
		Widget* BuildBottomContents();
		Widget* BuildBrowser();

	private:
		static constexpr float MIN_CONTENTS_SIZE = 2.0f;

		LayoutBorder*	   m_border			   = nullptr;
		Editor*			   m_editor			   = nullptr;
		DirectionalLayout* m_browserItems	   = nullptr;
		Text*			   m_path			   = nullptr;
		Text*			   m_itemCount		   = nullptr;
		Text*			   m_selectedItemCount = nullptr;
		GridLayout*		   m_contentsGrid	   = nullptr;

		Vector<DirectoryItem*> m_currentBrowserSelection  = {};
		Vector<DirectoryItem*> m_currentContentsSelection = {};
		float				   m_contentsSize			  = MAX_CONTENTS_SIZE;
		bool				   m_showListContents		  = false;
	};

} // namespace Lina::Editor
