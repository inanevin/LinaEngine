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
#include "Editor/WindowPanelManager.hpp"

namespace Lina
{
	class FileMenu;
	class Text;
} // namespace Lina

namespace Lina::Editor
{

	class ItemController : public Widget, public EditorPayloadListener
	{

	public:
		ItemController() : Widget(WF_CONTROLLABLE){};
		virtual ~ItemController() = default;

		struct Properties
		{
			Delegate<void(void* userData)> onItemSelected;
			Delegate<void()>			   onDuplicate;
			Delegate<void()>			   onDelete;
			Delegate<void()>			   onInteract;
			Delegate<void(void* userdata)> onItemRenamed;
			Delegate<void(void* userdata)> onCreatePayload;
			Delegate<bool(void* userdata)> onCheckCanCreatePayload;
			Delegate<void(void* userdata)> onPayloadAccepted;
			Color						   colorDeadItem		  = Color(0.0f, 0.0f, 0.0f, 0.0f);
			ColorGrad					   colorSelected		  = {Theme::GetDef().accentPrimary0, Theme::GetDef().accentPrimary1};
			ColorGrad					   colorUnfocused		  = {Theme::GetDef().silent0, Theme::GetDef().silent1};
			bool						   hoverAcceptItemParents = false;
			PayloadType					   payloadType			  = PayloadType::None;
		};

		virtual void			Construct() override;
		virtual void			Destruct() override;
		virtual void			Initialize() override;
		virtual void			PreTick() override;
		virtual void			OnGrabbedControls(bool isForward, Widget* prevControls) override;
		virtual Widget*			GetNextControls() override;
		virtual Widget*			GetPrevControls() override;
		virtual bool			OnKey(uint32 key, int32 scancode, LinaGX::InputAction act) override;
		virtual bool			OnMouse(uint32 button, LinaGX::InputAction act) override;
		virtual void			OnPayloadStarted(PayloadType type, Widget* payload) override;
		virtual void			OnPayloadEnded(PayloadType type, Widget* payload) override;
		virtual bool			OnPayloadDropped(PayloadType type, Widget* payload) override;
		virtual LinaGX::Window* OnPayloadGetWindow() override
		{
			return m_lgxWindow;
		}

		void	AddItem(Widget* widget);
		void	ClearItems();
		void	SetFocus(bool isFocused);
		void	SelectItem(Widget* item, bool clearSelected, bool callEvent);
		void	UnselectItem(Widget* item);
		bool	IsItemSelected(Widget* item);
		Widget* GetItem(void* userdata);
		void	UnselectAll();
		void	MakeVisibleRecursively(Widget* w);

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

		template <typename T> Vector<T*> GetSelectedUserData()
		{
			Vector<T*> out;
			for (Widget* w : m_selectedItems)
				out.push_back(static_cast<T*>(w->GetUserData()));
			return out;
		}

		inline bool GetIsFocused() const
		{
			return m_isFocused;
		}

	private:
		bool			m_payloadAllowed = false;
		FileMenu*		m_contextMenu	 = nullptr;
		Properties		m_props			 = {};
		Vector<Widget*> m_allItems;
		Vector<Widget*> m_selectedItems;
		bool			m_isFocused		= false;
		bool			m_payloadActive = false;
	};

	LINA_REFLECTWIDGET_BEGIN(ItemController)
	LINA_REFLECTWIDGET_END(ItemController)
} // namespace Lina::Editor
