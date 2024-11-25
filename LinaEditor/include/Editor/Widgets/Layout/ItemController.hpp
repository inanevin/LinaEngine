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
			Delegate<void()>			   onCreatePayload;
			Delegate<bool(void* userdata)> onCheckCanCreatePayload;
			Delegate<void(void* userdata)> onPayloadAccepted;
			Color						   colorDeadItem  = Color(0.0f, 0.0f, 0.0f, 0.0f);
			ColorGrad					   colorSelected  = {Theme::GetDef().accentPrimary0, Theme::GetDef().accentPrimary1};
			ColorGrad					   colorUnfocused = {Theme::GetDef().silent0, Theme::GetDef().silent1};
			PayloadType					   payloadType	  = PayloadType::None;

			void SaveToStream(OStream& stream) const
			{
				stream << colorDeadItem << colorSelected << colorUnfocused;
				stream << payloadType;
			}

			void LoadFromStream(IStream& stream)
			{
				stream >> colorDeadItem >> colorSelected >> colorUnfocused;
				stream >> payloadType;
			}
		};

		void	ClearItems();
		void	GatherItems(Widget* parent);
		void	SetFocus(bool isFocused);
		void	SelectItem(Widget* item, bool clearSelected, bool callEvent, bool makeVisible = false);
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

	protected:
		virtual void	Construct() override;
		virtual void	Destruct() override;
		virtual void	Initialize() override;
		virtual void	PreTick() override;
		virtual void	OnGrabbedControls(bool isForward, Widget* prevControls) override;
		virtual Widget* GetNextControls() override;
		virtual Widget* GetPrevControls() override;
		virtual bool	OnKey(uint32 key, int32 scancode, LinaGX::InputAction act) override;
		virtual bool	OnMouse(uint32 button, LinaGX::InputAction act) override;
		virtual void	OnPayloadStarted(PayloadType type, Widget* payload) override;
		virtual void	OnPayloadEnded(PayloadType type, Widget* payload) override;
		virtual bool	OnPayloadDropped(PayloadType type, Widget* payload) override;

		virtual LinaGX::Window* OnPayloadGetWindow() override
		{
			return m_lgxWindow;
		}

		virtual void SaveToStream(OStream& stream) const override
		{
			Widget::SaveToStream(stream);
			stream << m_props;
		}

		virtual void LoadFromStream(IStream& stream) override
		{
			Widget::LoadFromStream(stream);
			stream >> m_props;
		}

	private:
		void*			m_lastSelected = nullptr;
		FileMenu*		m_contextMenu  = nullptr;
		Properties		m_props		   = {};
		Vector<Widget*> m_allItems;
		Vector<Widget*> m_selectedItems;
		bool			m_isFocused		  = false;
		bool			m_payloadActive	  = false;
		bool			m_createdPayload  = false;
		float			m_pressedItemPosY = 0.0f;
		Widget*			m_pressedItem	  = nullptr;
	};

	LINA_WIDGET_BEGIN(ItemController, Editor)
	LINA_CLASS_END(ItemController)
} // namespace Lina::Editor
