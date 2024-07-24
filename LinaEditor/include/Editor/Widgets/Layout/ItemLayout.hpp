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
	class TextureAtlasImage;
	class FileMenu;
	class Text;
} // namespace Lina

namespace Lina::Editor
{
	struct ItemDefinition
	{
		void*				   userData			= nullptr;
		bool				   useOutlineInGrid = false;
		bool				   unfoldOverride	= false;
		bool				   nameEditable		= true;
		String				   icon				= "";
		Color				   color			= Theme::GetDef().foreground0;
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
			bool												  itemsCanHaveChildren = false;
			Delegate<void(Vector<ItemDefinition>& outITems)>	  onGatherItems;
			Delegate<void(void* userData)>						  onItemSelected;
			Delegate<void(void* userData)>						  onItemInteracted;
			Delegate<void(void* userData)>						  onItemDelete;
			Delegate<void(void* userData)>						  onDuplicate;
			Delegate<void(void* userData)>						  onDelete;
			Delegate<void(void* userData)>						  onCreatePayload;
			Delegate<void(void* userData, const String& newName)> onItemRenamed;
		};

		virtual void	Construct() override;
		virtual void	Initialize() override;
		virtual void	PreTick() override;
		virtual void	Draw() override;
		virtual void	OnGrabbedControls(bool isForward, Widget* prevControls) override;
		virtual Widget* GetNextControls() override;
		virtual Widget* GetPrevControls() override;
		virtual bool	OnKey(uint32 key, int32 scancode, LinaGX::InputAction act) override;
		virtual bool	OnMouse(uint32 button, LinaGX::InputAction act) override;

		void SetOutlineThicknessAndColor(float outlineThickness, const Color& outlineColor);
		void SetSelectedItem(void* userData);
		void RefreshItems();
		void SetUseGridLayout(bool useGridLayout);
		void SetGridItemSize(const Vector2& size);
		void SetFocus(bool isFocused);

		inline Properties& GetProps()
		{
			return m_props;
		}

		inline FileMenu* GetContextMenu() const
		{
			return m_contextMenu;
		}

		inline const Vector<Widget*>& GetSelectedItems() const
		{
			return m_selectedItems;
		}

		inline bool GetFocus() const
		{
			return m_isFocused;
		}

		template <typename T> inline void FillSelectedUserData(Vector<T*>& outData)
		{
			for (Widget* r : m_selectedItems)
			{
				void* userData = static_cast<Widget*>(r)->GetUserData();
				outData.push_back(static_cast<T*>(userData));
			}
		}

	private:
		bool IsItemSelected(Widget* r);
		void RemoveItemFromSelected(Widget* r);
		void SelectItem(Widget* r, bool clearSelected, bool callEvent = true);
		void UnfoldRecursively(Widget* w);
		void RenameTitle(Text* txt);

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
		Vector<Widget*>		 m_selectionItems;
		Vector<Widget*>		 m_selectedItems;
		bool				 m_isFocused   = false;
		FileMenu*			 m_contextMenu = nullptr;
		bool				 m_isPressed   = true;
		Widget*				 m_dragTarget  = nullptr;
	};

	LINA_REFLECTWIDGET_BEGIN(ItemLayout)
	LINA_REFLECTWIDGET_END(ItemLayout)
} // namespace Lina::Editor
