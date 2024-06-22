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
#include "Common/Data/HashMap.hpp"
#include "Editor/Editor.hpp"

namespace Lina
{
	class Selectable;
	class DirectionalLayout;
	class FileMenu;
	class GridLayout;
	class FoldLayout;
} // namespace Lina

namespace Lina::Editor
{

	struct SelectableListItem
	{
		String	 title				  = "";
		void*	 userData			  = nullptr;
		bool	 hasChildren		  = false;
		bool	 useDropdownIcon	  = true;
		bool	 useFolderIcon		  = false;
		bool	 useCustomInteraction = false;
		Texture* customTexture		  = nullptr;
	};

	class SelectableListLayout;

	class SelectableListLayoutListener
	{
	public:
		virtual void	OnSelectableListFillItems(SelectableListLayout* list, Vector<SelectableListItem>& outItems, void* parentUserData){};
		virtual void	OnSelectableListPayloadDropped(SelectableListLayout* list, void* payloadUserData, void* droppedItemUserData){};
		virtual void	OnSelectableListItemControl(SelectableListLayout* list, void* userData){};
		virtual void	OnSelectableListItemInteracted(SelectableListLayout* list, void* userData){};
		virtual Widget* OnSelectableListBuildCustomTooltip(SelectableListLayout* list, void* userData)
		{
			return nullptr;
		};
		virtual PayloadType OnSelectableListGetPayloadType(SelectableListLayout* list)
		{
			return PayloadType::None;
		}
	};

	class SelectableListLayout : public Widget, public EditorPayloadListener
	{
	public:
		SelectableListLayout()			= default;
		virtual ~SelectableListLayout() = default;

		struct Properties
		{
			bool useGridAsLayout = false;
		};

		virtual void Destruct() override;
		virtual void Construct() override;
		virtual void Initialize() override;
		virtual void Tick(float delta) override;
		virtual void OnPayloadStarted(PayloadType type, Widget* payload) override;
		virtual void OnPayloadEnded(PayloadType type, Widget* payload) override;
		virtual bool OnPayloadDropped(PayloadType type, Widget* payload) override;
		virtual bool OnMouse(uint32 button, LinaGX::InputAction act) override;
		void		 ChangeFold(void* userData, bool isUnfolded);
		void		 GrabControls(void* userData);
		void		 SetGridLayoutItemSize(const Vector2& size);

		virtual LinaGX::Window* OnPayloadGetWindow() override
		{
			return m_lgxWindow;
		}

		void RefreshItems();

		inline void SetListener(SelectableListLayoutListener* listener)
		{
			m_listener = listener;
		}

		inline Properties& GetProps()
		{
			return m_props;
		}

		inline FileMenu* GetFileMenu() const
		{
			return m_contextMenu;
		}

	private:
		Widget* CreateItem(const SelectableListItem& item, uint8 level);
		Widget* CreateGridItem(const SelectableListItem& item, uint8 level);
		void	SetSelectableCallbacks(const SelectableListItem& item, Selectable* selcetable, FoldLayout* fold);

	private:
		Properties					  m_props		  = {};
		SelectableListLayoutListener* m_listener	  = nullptr;
		Widget*						  m_layout		  = nullptr;
		DirectionalLayout*			  m_dirLayout	  = nullptr;
		GridLayout*					  m_gridLayout	  = nullptr;
		bool						  m_payloadActive = false;
		Vector<Selectable*>			  m_selectables	  = {};
		Vector<DirectionalLayout*>	  m_gridLayoutItems;
		FileMenu*					  m_contextMenu = nullptr;
		HashMap<void*, bool>		  m_foldStatus;
	};

	LINA_REFLECTWIDGET_BEGIN(SelectableListLayout)
	LINA_REFLECTWIDGET_END(SelectableListLayout)

} // namespace Lina::Editor
