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
#include "Common/Math/Math.hpp"
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
		GetFlags().Set(WF_MOUSE_PASSTHRU);
		FileMenu* fm = m_manager->Allocate<FileMenu>("FileMenu");
		AddChild(fm);
		m_contextMenu = fm;

		Editor::Get()->GetWindowPanelManager().AddPayloadListener(this);
	}

	void ItemController::Destruct()
	{
		Editor::Get()->GetWindowPanelManager().RemovePayloadListener(this);
	}

	void ItemController::Initialize()
	{
	}

	void ItemController::PreTick()
	{
		if (m_isPressed)
		{
			// Mouse up or item moved out of our control
			if (!m_lgxWindow->GetInput()->GetMouseButton(LINAGX_MOUSE_0) || m_pressedItem->GetPosY() != m_pressedItemPosY)
			{
				m_isPressed		 = false;
				m_createdPayload = false;
			}
		}

		if (!m_createdPayload && m_props.onCreatePayload && m_isPressed && !m_selectedItems.empty())
		{
			bool anyHovered = false;
			for (Widget* w : m_selectedItems)
			{
				if (w->GetIsHovered())
				{
					anyHovered = true;
					break;
				}
			}

			// We moved out.
			if (!anyHovered)
			{
				bool canCreate = true;
				for (Widget* it : m_selectedItems)
				{
					if (!m_props.onCheckCanCreatePayload(it->GetUserData()))
					{
						canCreate = false;
						break;
					}
				}

				if (canCreate)
				{
					m_props.onCreatePayload();
					m_createdPayload = true;
				}
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
			if (m_payloadActive && item->GetIsHovered())
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

		auto skipItem = [](Widget* w) -> bool {
			if (w->GetFlags().IsSet(WF_HIDE) || w->GetFlags().IsSet(WF_DISABLED))
				return true;

			if (w->GetParent() != nullptr)
			{
				Widget* parent = w->GetParent();

				if (parent->GetFlags().IsSet(WF_HIDE) || parent->GetFlags().IsSet(WF_DISABLED))
					return true;
			}
			return false;
		};

		Widget* item = nullptr;
		if (isForward)
		{
			for (Widget* r : m_selectedItems)
			{
				if (!skipItem(r))
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
				if (!skipItem(r))
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

		const int32 idx = UtilVector::IndexOf(m_allItems, currentSelected);

		if (idx == -1)
		{
			SetFocus(false);
			return nullptr;
		}

		const int32 sz = static_cast<int32>(m_allItems.size());

		auto skipItem = [](Widget* w) -> bool {
			if (w->GetFlags().IsSet(WF_HIDE) || w->GetFlags().IsSet(WF_DISABLED))
				return true;

			if (w->GetParent() != nullptr)
			{
				Widget* parent = w->GetParent();

				if (parent->GetFlags().IsSet(WF_HIDE) || parent->GetFlags().IsSet(WF_DISABLED))
					return true;
			}
			return false;
		};

		for (int32 i = idx + 1; i < sz; i++)
		{
			Widget* item = m_allItems[i];

			if (!skipItem(item))
			{
				SelectItem(item, true, true);
				return this;
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

		const int32 sz	= static_cast<int32>(m_allItems.size());
		const int32 idx = UtilVector::IndexOf(m_allItems, currentSelected);

		if (idx == -1)
		{
			SetFocus(false);
			return nullptr;
		}

		auto skipItem = [](Widget* w) -> bool {
			if (w->GetFlags().IsSet(WF_HIDE) || w->GetFlags().IsSet(WF_DISABLED))
				return true;

			if (w->GetParent() != nullptr)
			{
				Widget* parent = w->GetParent();

				if (parent->GetFlags().IsSet(WF_HIDE) || parent->GetFlags().IsSet(WF_DISABLED))
					return true;
			}
			return false;
		};

		for (int32 i = idx - 1; i >= 0; i--)
		{
			Widget* item = m_allItems[i];

			if (!skipItem)
			{
				SelectItem(item, true, true);
				return this;
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

		if (controlsHeld && key == LINAGX_KEY_A)
		{
			m_selectedItems.clear();
			for (Widget* item : m_allItems)
				m_selectedItems.push_back(item);
			if (m_props.onItemSelected)
				m_props.onItemSelected(m_selectedItems.back());
			return true;
		}

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

		if (key == LINAGX_KEY_F2 && m_selectedItems.size() == 1 && m_props.onItemRenamed)
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
		if (!m_isHovered)
			return false;

		const bool wasFocused = m_isFocused;

		if (act == LinaGX::InputAction::Pressed)
			SetFocus(true);

		const bool lshift = m_lgxWindow->GetInput()->GetKey(LINAGX_KEY_LSHIFT);
		const bool lctrl  = m_lgxWindow->GetInput()->GetKey(LINAGX_KEY_LCTRL);

		// To allow payload drag and drop on multiple items.
		if (button == LINAGX_MOUSE_0 && act == LinaGX::InputAction::Released && m_selectedItems.size() > 1 && !lshift && !lctrl)
		{
			for (Widget* item : m_allItems)
			{
				int32 idx = 0;

				if (!item->GetIsHovered())
				{
					idx++;
					continue;
				}

				SelectItem(item, true, true);
			}

			return true;
		}

		if (button == LINAGX_MOUSE_0 && (act == LinaGX::InputAction::Pressed || act == LinaGX::InputAction::Repeated))
		{
			int32 idx = 0;

			for (Widget* item : m_allItems)
			{
				if (!item->GetIsHovered())
				{
					idx++;
					continue;
				}

				if (m_lgxWindow->GetInput()->GetKey(LINAGX_KEY_LCTRL))
				{
					if (!IsItemSelected(item))
						SelectItem(item, false, true);
					else
						UnselectItem(item);

					return true;
				}

				if (IsItemSelected(item) && act != LinaGX::InputAction::Repeated)
				{
					m_pressedItem	  = item;
					m_pressedItemPosY = item->GetPosY();
					m_isPressed		  = true;
					return true;
				}

				if (m_lgxWindow->GetInput()->GetKey(LINAGX_KEY_LSHIFT))
				{
					Widget*		lastSelection = m_selectedItems.empty() ? m_allItems.front() : m_selectedItems.back();
					const int32 start		  = UtilVector::IndexOf(m_allItems, lastSelection);
					const int32 end			  = idx;
					const int32 iStart		  = Math::Min(start, end);
					const int32 iEnd		  = Math::Max(start, end);

					for (int32 i = iStart; i <= iEnd; i++)
					{
						SelectItem(m_allItems[i], false, true);
					}

					return true;
				}

				if (m_lgxWindow->GetInput()->GetKey(LINAGX_KEY_LCTRL))
				{
					if (!IsItemSelected(item))
						SelectItem(item, false, true);
				}
				else
				{
					const bool wasSelected = IsItemSelected(item);
					SelectItem(item, true, true);

					m_isPressed		  = true;
					m_pressedItemPosY = item->GetPosY();
					m_pressedItem	  = item;

					// interact
					if (act == LinaGX::InputAction::Repeated && m_props.onInteract)
						m_props.onInteract();
				}

				return true;
			}

			if (wasFocused)
			{
				UnselectAll();

				if (m_props.onItemSelected)
					m_props.onItemSelected(nullptr);
			}

			return true;
		}

		if (m_isHovered && button == LINAGX_MOUSE_1 && (act == LinaGX::InputAction::Pressed || act == LinaGX::InputAction::Repeated))
		{
			SetFocus(true);

			for (Widget* item : m_allItems)
			{
				if (item->GetIsHovered())
				{
					if (!IsItemSelected(item))
						SelectItem(item, true, true);

					m_contextMenu->CreateItems(0, m_manager->GetMousePosition(), nullptr);
					return true;
				}
			}

			UnselectAll();
			m_contextMenu->CreateItems(0, m_manager->GetMousePosition(), nullptr);
			return true;
		}

		return false;
	}

	void ItemController::OnPayloadStarted(PayloadType type, Widget* payload)
	{
		auto it = linatl::find_if(m_props.payloadTypes.begin(), m_props.payloadTypes.end(), [type](PayloadType pt) -> bool { return pt == type; });
		if (it == m_props.payloadTypes.end())
			return;

		m_payloadActive					  = true;
		GetWidgetProps().colorOutline	  = Theme::GetDef().accentPrimary0;
		GetWidgetProps().outlineThickness = Theme::GetDef().baseOutlineThickness;
		GetWidgetProps().outlineIsInner	  = true;
	}

	void ItemController::OnPayloadEnded(PayloadType type, Widget* payload)
	{
		auto it = linatl::find_if(m_props.payloadTypes.begin(), m_props.payloadTypes.end(), [type](PayloadType pt) -> bool { return pt == type; });
		if (it == m_props.payloadTypes.end())
			return;

		m_payloadActive					  = false;
		GetWidgetProps().outlineThickness = 0.0f;
	}

	bool ItemController::OnPayloadDropped(PayloadType type, Widget* payload)
	{
		auto it = linatl::find_if(m_props.payloadTypes.begin(), m_props.payloadTypes.end(), [type](PayloadType pt) -> bool { return pt == type; });
		if (it == m_props.payloadTypes.end())
			return false;

		m_payloadActive = false;

		for (Widget* w : m_allItems)
		{
			if (w->GetIsHovered())
			{
				if (m_props.onPayloadAccepted)
					m_props.onPayloadAccepted(w->GetUserData(), payload->GetUserData(), type);
				return true;
			}
		}

		if (m_isHovered)
		{
			if (m_props.onPayloadAccepted)
				m_props.onPayloadAccepted(nullptr, payload->GetUserData(), type);
			return true;
		}

		return false;
	}

	void ItemController::ClearItems()
	{
		m_allItems.clear();
		UnselectAll();
	}

	void ItemController::GatherItems(Widget* parent)
	{
		const Vector<Widget*> children = parent->GetChildren();
		for (Widget* c : children)
		{
			if (c->GetFlags().IsSet(WF_TREEITEM))
			{
				m_allItems.push_back(c);
				if (m_lastSelected == c->GetUserData())
					SelectItem(c, true, false);
			}

			GatherItems(c);
		}
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
		if (parent->GetParent() == this)
			return;

		if (parent->GetTID() != GetTypeID<FoldLayout>())
			return;

		FoldLayout* fold = static_cast<FoldLayout*>(parent);
		fold->SetIsUnfolded(true);
		MakeVisibleRecursively(parent);
	}

	void ItemController::SelectItem(Widget* item, bool clearSelected, bool callEvent, bool makeVisible)
	{
		if (item == nullptr)
			return;

		if (makeVisible)
			MakeVisibleRecursively(item);

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

		auto exists = linatl::find_if(m_selectedItems.begin(), m_selectedItems.end(), [item](Widget* it) -> bool { return item == it; });
		if (exists == m_selectedItems.end())
			m_selectedItems.push_back(item);

		if (callEvent && m_props.onItemSelected)
			m_props.onItemSelected(item->GetUserData());

		// SetFocus(true);
	}

	void ItemController::UnselectItem(Widget* item)
	{
		auto it = linatl::find_if(m_selectedItems.begin(), m_selectedItems.end(), [item](Widget* wid) -> bool { return item == wid; });
		if (it != m_selectedItems.end())
		{
			if (m_props.onItemUnselected)
				m_props.onItemUnselected(*it);
			m_selectedItems.erase(it);
		}
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
