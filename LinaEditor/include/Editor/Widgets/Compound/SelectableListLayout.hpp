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
} // namespace Lina

namespace Lina::Editor
{

	struct SelectableListItem
	{
		String title		   = "";
		void*  userData		   = nullptr;
		bool   hasChildren	   = false;
		bool   useDropdownIcon = true;
		bool   useFolderIcon   = false;
	};

	class SelectableListLayoutListener
	{
	public:
		virtual void OnSelectableListFillItems(Vector<SelectableListItem>& outItems, void* parentUserData){};
		virtual void OnSelectableListPayloadDropped(void* payloadUserData, void* droppedItemUserData){};
	};

	class SelectableListLayout : public Widget, public EditorPayloadListener
	{
	public:
		SelectableListLayout()			= default;
		virtual ~SelectableListLayout() = default;

		struct Properties
		{
			bool   useGridAsLayout = false;
			String iconFolded	   = "";
			String iconUnfolded	   = "";
			String iconFolder	   = "";
		};

		virtual void Destruct() override;
		virtual void Construct() override;
		virtual void Initialize() override;
		virtual void Tick(float delta) override;
		virtual void OnPayloadStarted(PayloadType type, Widget* payload) override;
		virtual void OnPayloadEnded(PayloadType type, Widget* payload) override;
		virtual bool OnPayloadDropped(PayloadType type, Widget* payload) override;

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

	private:
		Properties					  m_props		  = {};
		SelectableListLayoutListener* m_listener	  = nullptr;
		DirectionalLayout*			  m_layout		  = nullptr;
		Editor*						  m_editor		  = nullptr;
		bool						  m_payloadActive = false;
		Vector<Selectable*>			  m_selectables	  = {};
		FileMenu*					  m_contextMenu	  = nullptr;
		HashMap<void*, bool>		  m_foldStatus;
	};

} // namespace Lina::Editor
