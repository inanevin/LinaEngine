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

#include "Editor/Widgets/Layout/ItemController.hpp"
#include "Editor/Editor.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/Primitives/InputField.hpp"
#include "Core/GUI/Widgets/Primitives/Icon.hpp"
#include "Core/GUI/Widgets/Layout/ScrollArea.hpp"
#include "Core/GUI/Widgets/Compound/FileMenu.hpp"
#include "Core/GUI/Widgets/Layout/FoldLayout.hpp"
#include <LinaGX/Core/InputMappings.hpp>
namespace Lina::Editor
{
	void ItemController::Construct()
	{
		// GetFlags().Set(WF_MOUSE_PASSTHRU);
		FileMenu* fm = m_manager->Allocate<FileMenu>("FileMenu");
		AddChild(fm);
		m_contextMenu = fm;
	}

	void ItemController::Destruct()
	{
		Editor::Get()->GetWindowPanelManager().RemovePayloadListener(this);
	}

	void ItemController::Initialize()
	{
		Editor::Get()->GetWindowPanelManager().AddPayloadListener(this);
	}

	void ItemController::PreTick()
	{
		if (m_isPressed && !m_lgxWindow->GetInput()->GetMouseButton(LINAGX_MOUSE_0))
		{
			m_isPressed = false;
		}

		if (!m_isPressed)
			m_payloadAllowed = true;

		if (m_isPressed && m_selectedItems.size() == 1 && m_payloadAllowed && m_props.onCreatePayload)
		{
			Widget* item = m_selectedItems.front();
			if (!item->GetIsHovered() && (!m_props.hoverAcceptItemParents || (m_props.hoverAcceptItemParents && !item->GetParent()->GetIsHovered())))
			{
				bool canCreate = true;
				if (m_props.onCheckCanCreatePayload)
					canCreate = m_props.onCheckCanCreatePayload(item->GetUserData());

				if (canCreate)
				{
					m_props.onCreatePayload(item->GetUserData());
				}

				m_payloadAllowed = false;
			}
		}

		if (m_isFocused && !m_manager->IsControlsOwner(this))
			SetFocus(false);

		for (Widget* item : m_allItems)
		{
			const bool selected = IsItemSelected(item);

			if (selected)
			{
				item->GetWidgetProps().colorBackground.start = m_isFocused ? m_props.colorSelected.start : m_props.colorUnfocused.start;
				item->GetWidgetProps().colorBackground.end	 = m_isFocused ? m_props.colorSelected.end : m_props.colorUnfocused.end;
			}
			else
			{
				item->GetWidgetProps().colorBackground = m_props.colorDeadItem;
			}

			// Maybe create a dedicated payload preview rendering logic instead of modifying widget properties.
			const bool hoverOK = item->GetIsHovered() || (m_props.hoverAcceptItemParents && item->GetParent()->GetIsHovered());
			if (m_payloadActive && hoverOK)
			{
				item->GetWidgetProps().outlineThickness = Theme::GetDef().baseOutlineThickness;
				item->GetWidgetProps().outlineIsInner	= true;
				item->GetWidgetProps().colorOutline		= Theme::GetDef().accentSecondary;
			}
			else
			{
				item->GetWidgetProps().outlineThickness = 0.0f;
			}
		}
	}

	void ItemController::OnGrabbedControls(bool isForward, Widget* prevControls)
	{
		if (prevControls == this)
			return;

		if (!m_selectedItems.empty())
		{
			SetFocus(true);
			return;
		}

		if (m_allItems.empty())
			return;

		Widget* item = nullptr;
		if (isForward)
		{
			for (Widget* r : m_selectedItems)
			{
				if (r->GetIsVisible() && !r->GetIsDisabled())
				{
					item = r;
					break;
				}
			}
		}
		else
		{
			for (Vector<Widget*>::iterator it = m_selectedItems.end() - 1; it >= m_selectedItems.begin(); it--)
			{
				Widget* r = *it;
				if (r->GetIsVisible() && !r->GetIsDisabled())
				{
					item = r;
					break;
				}
			}
		}

		if (item != nullptr)
		{
			SelectItem(item, true, true);
			SetFocus(true);
		}
	}

	Widget* ItemController::GetNextControls()
	{
		if (m_allItems.empty())
			return nullptr;

		if (m_selectedItems.empty())
		{
			SelectItem(m_allItems[0], true, true);
			return this;
		}

		Widget* currentSelected = m_selectedItems.back();

		const int32 sz	  = static_cast<int32>(m_allItems.size());
		bool		found = false;

		for (int32 i = 0; i < sz; i++)
		{
			Widget* item = m_allItems[i];

			if (item == currentSelected)
			{
				found = true;
				continue;
			}

			if (found)
			{
				if (item->GetIsVisible() && !item->GetIsDisabled())
				{
					SelectItem(item, true, true);
					return this;
				}
			}
		}

		SetFocus(false);
		return nullptr;
	}

	Widget* ItemController::GetPrevControls()
	{
		if (m_allItems.empty() || m_selectedItems.empty())
			return nullptr;

		Widget* currentSelected = m_selectedItems.front();

		const int32 sz	  = static_cast<int32>(m_allItems.size());
		bool		found = false;

		for (int32 i = sz - 1; i >= 0; i--)
		{
			Widget* item = m_allItems[i];

			if (item == currentSelected)
			{
				found = true;
				continue;
			}

			if (found)
			{
				if (item->GetIsVisible() && !item->GetIsDisabled())
				{
					SelectItem(item, true, true);
					return this;
				}
			}
		}

		SetFocus(false);
		return nullptr;
	}

	bool ItemController::OnKey(uint32 key, int32 scancode, LinaGX::InputAction act)
	{
		if (act != LinaGX::InputAction::Pressed)
			return false;

		if (m_allItems.empty() || !m_isFocused || m_selectedItems.empty())
			return false;

		const bool controlsHeld = m_lgxWindow->GetInput()->GetKey(LINAGX_KEY_LCTRL);

		if (controlsHeld && key == LINAGX_KEY_D)
		{
			// Duplicate
			if (m_props.onDuplicate)
				m_props.onDuplicate();
			return true;
		}

		if (key == LINAGX_KEY_DELETE)
		{
			// delete
			if (m_props.onDelete)
				m_props.onDelete();
			return true;
		}

		if (key == LINAGX_KEY_LEFT)
		{
			for (Widget* w : m_selectedItems)
			{
				if (w->GetParent() && w->GetParent()->GetTID() == GetTypeID<FoldLayout>() && w->GetParent()->GetChildren().front() == w)
				{
					FoldLayout* f = static_cast<FoldLayout*>(w->GetParent());
					f->SetIsUnfolded(false);
				}
			}
			return true;
		}

		if (key == LINAGX_KEY_R && controlsHeld && m_selectedItems.size() == 1 && m_props.onItemRenamed)
		{
			m_props.onItemRenamed(m_selectedItems.front()->GetUserData());
			return true;
		}

		if (key == LINAGX_KEY_RIGHT)
		{
			for (Widget* w : m_selectedItems)
			{
				if (w->GetParent() && w->GetParent()->GetTID() == GetTypeID<FoldLayout>() && w->GetParent()->GetChildren().front() == w)
				{
					FoldLayout* f = static_cast<FoldLayout*>(w->GetParent());
					f->SetIsUnfolded(true);
				}
			}
			return true;
		}

		if (key == LINAGX_KEY_RETURN)
		{
			for (Widget* w : m_selectedItems)
			{
				if (w->GetParent() && w->GetParent()->GetTID() == GetTypeID<FoldLayout>() && w->GetParent()->GetChildren().front() == w)
				{
                    FoldLayout* f = static_cast<FoldLayout*>(w->GetParent());
                    f->SetIsUnfolded(!f->GetIsUnfolded());
				}
			}
			// interact
			if (m_props.onInteract)
				m_props.onInteract();
			return true;
		}

		return false;
	}

	bool ItemController::OnMouse(uint32 button, LinaGX::InputAction act)
	{
		if (act == LinaGX::InputAction::Released)
			return false;

		if (m_isHovered && button == LINAGX_MOUSE_0 && (act == LinaGX::InputAction::Pressed || act == LinaGX::InputAction::Repeated))
		{
			SetFocus(true);

			for (Widget* item : m_allItems)
			{
				if (item->GetIsHovered() || (m_props.hoverAcceptItemParents && item->GetParent()->GetIsHovered()))
				{
					FoldLayout* fold = nullptr;

					if (item->GetParent()->GetTID() == GetTypeID<FoldLayout>() && item->GetParent()->GetChildren().front() == item)
						fold = static_cast<FoldLayout*>(item->GetParent());

					if (fold != nullptr)
					{
						Widget* front	= fold->GetChildren().front();
						Icon*	chevron = front->GetWidgetOfType<Icon>(front);

						if (chevron && chevron->GetIsHovered())
							fold->SetIsUnfolded(!fold->GetIsUnfolded());
						else if (act == LinaGX::InputAction::Repeated && fold->GetIsHovered())
							fold->SetIsUnfolded(!fold->GetIsUnfolded());
					}

					if (m_lgxWindow->GetInput()->GetKey(LINAGX_KEY_LCTRL))
					{
						if (IsItemSelected(item))
							UnselectItem(item);
						else
							SelectItem(item, false, true);
					}
					else
					{
						const bool wasSelected = IsItemSelected(item);
						SelectItem(item, true, true);

						m_isPressed = true;

						// interact
						if (act == LinaGX::InputAction::Repeated && m_props.onInteract)
							m_props.onInteract();
					}
					return true;
				}
			}

			UnselectAll();

			if (m_props.onItemSelected)
				m_props.onItemSelected(nullptr);

			return true;
		}

		if (m_isHovered && button == LINAGX_MOUSE_1 && (act == LinaGX::InputAction::Pressed || act == LinaGX::InputAction::Repeated))
		{
			SetFocus(true);

			for (Widget* item : m_allItems)
			{
				if (item->GetIsHovered() || (m_props.hoverAcceptItemParents && item->GetParent()->GetIsHovered()))
				{
					if (!IsItemSelected(item))
						SelectItem(item, true, true);

					m_contextMenu->CreateItems(0, m_lgxWindow->GetMousePosition(), nullptr);
					return true;
				}
			}

			UnselectAll();
			m_contextMenu->CreateItems(0, m_lgxWindow->GetMousePosition(), nullptr);
			return true;
		}

		return false;
	}

	void ItemController::OnPayloadStarted(PayloadType type, Widget* payload)
	{
		if (type != m_props.payloadType)
			return;

		m_payloadActive					  = true;
		GetWidgetProps().colorOutline	  = Theme::GetDef().accentPrimary0;
		GetWidgetProps().outlineThickness = Theme::GetDef().baseOutlineThickness;
		GetWidgetProps().outlineIsInner	  = true;
	}

	void ItemController::OnPayloadEnded(PayloadType type, Widget* payload)
	{
		if (type != m_props.payloadType)
			return;

		m_payloadActive					  = false;
		GetWidgetProps().outlineThickness = 0.0f;
	}

	bool ItemController::OnPayloadDropped(PayloadType type, Widget* payload)
	{
		if (type != m_props.payloadType)
			return false;

		m_payloadActive = false;

		for (Widget* w : m_allItems)
		{
			const bool hoverOK = w->GetIsHovered() || (m_props.hoverAcceptItemParents && w->GetParent()->GetIsHovered());
			if (hoverOK)
			{
				if (m_props.onPayloadAccepted)
					m_props.onPayloadAccepted(w->GetUserData());
				return true;
			}
		}

		if (m_isHovered)
		{
			if (m_props.onPayloadAccepted)
				m_props.onPayloadAccepted(nullptr);
			return true;
		}

		return false;
	}

	void ItemController::AddItem(Widget* widget)
	{
		m_allItems.push_back(widget);
        
        if(m_lastSelected == widget->GetUserData())
            SelectItem(widget, true, false);
	}

	void ItemController::ClearItems()
	{
		m_allItems.clear();
		UnselectAll();
	}

	void ItemController::UnselectAll()
	{
		m_selectedItems.clear();
	}

	void ItemController::SetFocus(bool isFocused)
	{
		m_isFocused = isFocused;

		if (m_isFocused && !m_manager->IsControlsOwner(this))
			m_manager->GrabControls(this);
	}

	void ItemController::MakeVisibleRecursively(Widget* w)
	{
		Widget* parent = w->GetParent();
		if (parent == this)
			return;
		parent = parent->GetParent();
		if (parent == this)
			return;

		if (parent->GetTID() != GetTypeID<FoldLayout>())
			return;

		FoldLayout* fold = static_cast<FoldLayout*>(parent);
		fold->SetIsUnfolded(true);
		parent->SetVisible(true);
		MakeVisibleRecursively(parent);
	}

	void ItemController::SelectItem(Widget* item, bool clearSelected, bool callEvent)
	{
		if (item == nullptr)
			return;

		if (m_children.size() > 1)
		{
			Widget* c = m_children[1];
			if (c->GetTID() == GetTypeID<ScrollArea>())
			{
				ScrollArea* sc = static_cast<ScrollArea*>(c);
				sc->ScrollToChild(item);
			}
		}
		if (clearSelected)
			m_selectedItems.clear();

        m_lastSelected = item->GetUserData();
		m_manager->GrabControls(this);
		m_selectedItems.push_back(item);

		if (callEvent && m_props.onItemSelected)
			m_props.onItemSelected(item->GetUserData());

		SetFocus(true);
	}

	void ItemController::UnselectItem(Widget* item)
	{
		auto it = linatl::find_if(m_selectedItems.begin(), m_selectedItems.end(), [item](Widget* wid) -> bool { return item == wid; });
		if (it != m_selectedItems.end())
			m_selectedItems.erase(it);
	}

	bool ItemController::IsItemSelected(Widget* w)
	{
		auto it = linatl::find_if(m_selectedItems.begin(), m_selectedItems.end(), [w](Widget* wid) -> bool { return w == wid; });
		return it != m_selectedItems.end();
	}

	Widget* ItemController::GetItem(void* userdata)
	{
		auto it = linatl::find_if(m_allItems.begin(), m_allItems.end(), [userdata](Widget* wid) -> bool { return wid->GetUserData() == userdata; });
		if (it != m_allItems.end())
			return *it;
		return nullptr;
	}

} // namespace Lina::Editor
