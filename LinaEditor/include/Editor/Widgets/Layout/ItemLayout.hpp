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
	class ScrollArea;
	class ShapeRect;
	class TextureAtlasImage;
} // namespace Lina

namespace Lina::Editor
{
	struct ItemDefinition
	{
		void*				   userData			= nullptr;
		bool				   useOutlineInGrid = false;
		bool				   unfoldOverride	= false;
		String				   icon				= "";
		TextureAtlasImage*	   texture			= nullptr;
		String				   name				= "";
		Vector<ItemDefinition> children;
	};

	class ItemLayout : public Widget
	{

	public:
		ItemLayout() : Widget(){};
		virtual ~ItemLayout() = default;

		struct Properties
		{
			bool											 itemsCanHaveChildren = false;
			Delegate<void(Vector<ItemDefinition>& outITems)> onGatherItems;
			Delegate<void(void* userData)>					 onItemSelected;
			Delegate<void(void* userData)>					 onItemInteracted;
			Delegate<void(void* userData)>					 onItemDelete;
		};

		virtual void	Construct() override;
		virtual void	Initialize() override;
		virtual void	PreTick() override;
		virtual void	OnGrabbedControls(bool isForward, Widget* prevControls) override;
		virtual Widget* GetNextControls() override;
		virtual Widget* GetPrevControls() override;
		virtual bool	OnKey(uint32 key, int32 scancode, LinaGX::InputAction act) override;
		virtual bool	OnMouse(uint32 button, LinaGX::InputAction act) override;

		void SetSelectedItem(void* userData);
		void RefreshItems();
		void SetUseGridLayout(bool useGridLayout);
		void SetGridItemSize(const Vector2& size);

		inline void SetFocus(bool isFocused)
		{
			m_isFocused = isFocused;
		}

		inline Properties& GetProps()
		{
			return m_props;
		}

	private:
		bool IsItemSelected(ShapeRect* r);
		void RemoveItemFromSelected(ShapeRect* r);
		void SelectItem(ShapeRect* r);
		void UnfoldRecursively(Widget* w);

	private:
		void	CreateLayout();
		void	ProcessItem(const ItemDefinition& def, Widget* parent, float margin);
		Widget* CreateItem(const ItemDefinition& def, float margin, bool hasParent);
		Widget* CreateListItem(const ItemDefinition& def, float margin, bool hasParent);
		Widget* CreateGridItem(const ItemDefinition& def);
		void	MakeVisibleRecursively(Widget* w);

	private:
		Properties			 m_props		 = {};
		Widget*				 m_layout		 = nullptr;
		ScrollArea*			 m_scroll		 = nullptr;
		bool				 m_useGridLayout = false;
		Vector<Widget*>		 m_gridItems;
		Vector2				 m_gridItemSize = Vector2::Zero;
		HashMap<void*, bool> m_areItemsUnfolded;
		Vector<Widget*>		 m_listItems;
		void*				 m_lastSelectedItem = nullptr;
		Vector<ShapeRect*>	 m_selectionItems;
		Vector<ShapeRect*>	 m_selectedItems;
		bool				 m_isFocused = false;
	};

	LINA_REFLECTWIDGET_BEGIN(ItemLayout)
	LINA_REFLECTWIDGET_END(ItemLayout)
} // namespace Lina::Editor
