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

#include "GUI/Nodes/GUINode.hpp"
#include "Data/CommonData.hpp"
#include "GUI/Utility/GUIUtility.hpp"
#include "Input/Core/InputMappings.hpp"
#include "Graphics/Platform/LinaVGIncl.hpp"
#include "Graphics/Interfaces/IWindow.hpp"
#include "Graphics/Interfaces/ISwapchain.hpp"
#include "GUI/Drawers/GUIDrawerBase.hpp"
#include "Serialization/StringSerialization.hpp"
#include "Math/Math.hpp"
#include "Serialization/HashMapSerialization.hpp"
#include "Core/SystemInfo.hpp"

namespace Lina::Editor
{
	GUINode::GUINode(GUIDrawerBase* drawer, int drawOrder) : m_drawOrder(drawOrder)
	{
		SetDrawer(drawer);
	}

	void GUINode::SetDrawer(GUIDrawerBase* drawer)
	{
		m_drawer	= drawer;
		m_swapchain = m_drawer->GetSwapchain();
		m_editor	= m_drawer->GetEditor();
		m_window	= m_swapchain->GetWindow();

		for (auto c : m_children)
			c->SetDrawer(drawer);
	}

	GUINode::~GUINode()
	{
		m_drawer->OnNodeDeleted(this);

		for (auto c : m_children)
			delete c;

		m_children.clear();
	}

	void GUINode::Draw(int threadID)
	{
		if (!GetIsVisible())
			return;

		const uint32 sz = static_cast<uint32>(m_children.size());
		for (uint32 i = 0; i < sz; i++)
			m_children[i]->Draw(threadID);
	}

	void GUINode::OnKey(uint32 key, InputAction action)
	{
		const uint32 sz = static_cast<uint32>(m_children.size());
		for (uint32 i = 0; i < sz; i++)
			m_children[i]->OnKey(key, action);
	}

	bool GUINode::OnMouse(uint32 button, InputAction action)
	{
		static GUINode* lastPressedNode = nullptr;

		if (!GetIsVisible() || m_disabled)
			return false;

		if (button == LINA_MOUSE_0)
		{
			if (action == InputAction::Pressed)
			{
				if (GetIsHovered())
				{
					lastPressedNode = this;
					m_isPressed		= true;
					OnPressed(button);
					m_dragStartMousePos	  = m_window->GetMousePosition();
					m_dragStartMouseDelta = Vector2(m_dragStartMousePos) - m_rect.pos;
					m_lastPressTime		  = SystemInfo::GetAppTimeF();
				}
			}
			else if (action == InputAction::Released)
			{
				if (m_isDragging)
					OnDragEnd();

				const Vector2 delta = (m_window->GetMousePosition() - m_dragStartMousePos);

				const float diff	= SystemInfo::GetAppTimeF() - m_lastPressTime;
				const bool	isClick = diff < 0.15f;

				if (isClick || delta < 15.0f || !m_isDragging)
				{
					if (lastPressedNode == this && GetIsHovered())
					{
						lastPressedNode = nullptr;
						OnClicked(LINA_MOUSE_0);
					}
					else if (!GetIsHovered())
						OnClickedOutside(LINA_MOUSE_0);
				}

				m_isPressed	 = false;
				m_isDragging = false;
			}
			else if (action == InputAction::Repeated)
			{
				if (GetIsHovered())
					OnDoubleClicked();
			}
		}
		else
		{
			if (action == InputAction::Released)
			{
				if (GetIsHovered())
					OnClicked(button);
				else
					OnClickedOutside(button);
			}
		}

		const uint32 sz = static_cast<uint32>(m_children.size());
		for (uint32 i = 0; i < sz; i++)
			m_children[i]->OnMouse(button, action);

		return false;
	}

	bool GUINode::OnMouseWheel(uint32 delta)
	{
		const uint32 sz = static_cast<uint32>(m_children.size());
		for (uint32 i = 0; i < sz; i++)
		{
			if (m_children[i]->OnMouseWheel(delta))
				return true;
		}

		return false;
	}

	void GUINode::OnMousePos()
	{
		if (m_isPressed && !m_isDragging)
		{
			m_isDragging = true;
			OnDragBegin();
		}
	}

	void GUINode::OnLostFocus()
	{
		if (m_isHovered)
			OnHoverEnd();

		if (m_isDragging)
			OnDragEnd();

		m_isPressed = m_isHovered = m_isDragging = false;

		const uint32 sz = static_cast<uint32>(m_children.size());
		for (uint32 i = 0; i < sz; i++)
			m_children[i]->OnLostFocus();
	}

	bool GUINode::OnShortcut(Shortcut sc)
	{
		const uint32 sz = static_cast<uint32>(m_children.size());
		for (uint32 i = 0; i < sz; i++)
		{
			if (m_children[i]->OnShortcut(sc))
				return true;
		}
		return false;
	}

	void GUINode::OnPayloadCreated(PayloadType type, void* userData)
	{
		m_payloadAvailable = m_payloadMask.IsSet(type);

		const uint32 sz = static_cast<uint32>(m_children.size());
		for (uint32 i = 0; i < sz; i++)
			m_children[i]->OnPayloadCreated(type, userData);
	}

	void GUINode::OnPayloadEnded(PayloadType type)
	{
		m_payloadAvailable = false;

		const uint32 sz = static_cast<uint32>(m_children.size());
		for (uint32 i = 0; i < sz; i++)
			m_children[i]->OnPayloadEnded(type);
	}

	void GUINode::SaveToStream(OStream& stream)
	{
		stream << m_sid << m_visible << m_drawOrder << m_lastDpi;
		m_rect.pos.SaveToStream(stream);
		m_rect.size.SaveToStream(stream);
		HashMapSerialization::SaveToStream_OBJ(stream, m_storedSizes);
		StringSerialization::SaveToStream(stream, m_title);
	}

	void GUINode::LoadFromStream(IStream& stream)
	{
		stream >> m_sid >> m_visible >> m_drawOrder >> m_lastDpi;
		m_rect.pos.LoadFromStream(stream);
		m_rect.size.LoadFromStream(stream);
		HashMapSerialization::LoadFromStream_OBJ(stream, m_storedSizes);
		StringSerialization::LoadFromStream(stream, m_title);
	}

	void GUINode::SetTitle(const String& title)
	{
		m_title = title;
		ClearStoredSizes();
	}

	void GUINode::AddChildren(GUINode* node)
	{
		m_children.push_back(node);
		node->m_parent = this;
	}

	void GUINode::RemoveChildren(GUINode* node)
	{
		auto it = linatl::find_if(m_children.begin(), m_children.end(), [node](GUINode* child) { return child == node; });
		m_children.erase(it);
		node->m_parent = nullptr;
	}

	void GUINode::SetVisible(bool visible)
	{
		m_visible		= visible;
		const uint32 sz = static_cast<uint32>(m_children.size());
		for (uint32 i = 0; i < sz; i++)
			m_children[i]->SetParentVisible(visible);
	}

	GUINode* GUINode::FindChildren(StringID sid)
	{
		GUINode* node = nullptr;

		for (auto c : m_children)
		{
			if (c->GetSID() == sid)
				return c;
			else
				node = c->FindChildren(sid);

			if (node)
				break;
		}

		return node;
	}

	bool GUINode::ChildExists(GUINode* node)
	{
		for (auto c : m_children)
		{
			if (c == node)
				return true;

			if (c->ChildExists(node))
				return true;
		}

		return false;
	}

	void GUINode::SetParentVisible(bool parentVisible)
	{
		m_parentVisible = parentVisible;
		const uint32 sz = static_cast<uint32>(m_children.size());
		for (uint32 i = 0; i < sz; i++)
			m_children[i]->SetParentVisible(parentVisible);
	}

	void GUINode::ConsumeAvailableWidth()
	{
		if (m_parent)
		{
			const float padding	   = Theme::GetProperty(ThemeProperty::GeneralItemPadding, m_window->GetDPIScale());
			const Rect& parentRect = m_parent->GetRect();
			m_rect.size.x		   = parentRect.pos.x + parentRect.size.x - m_rect.pos.x - padding;
		}
	}

	void GUINode::SetWidgetHeight(ThemeProperty prop)
	{
		m_rect.size.y = Theme::GetProperty(prop, m_window->GetDPIScale());
	}

	Vector2 GUINode::GetStoreSize(StringID sid, const String& text, FontType ft, float textScale, bool calculateEveryTime)
	{
		const float windowDpi = m_window->GetDPIScale();

		if (Math::Equals(m_lastDpi, windowDpi, 0.0001f) && !calculateEveryTime)
		{
			const Vector2 retSize = m_storedSizes[sid];
			if (!Math::Equals(retSize.x, 0.0f, 0.001f) || !Math::Equals(retSize.y, 0.0f, 0.001f))
				return retSize;
		}

		m_lastDpi = windowDpi;
		LinaVG::TextOptions textOpts;
		textOpts.font	   = Theme::GetFont(ft, m_lastDpi);
		textOpts.textScale = textScale;
		const Vector2 sz   = FL2(LinaVG::CalculateTextSize(text.c_str(), textOpts));
		m_storedSizes[sid] = sz;
		return sz;
	}

	void GUINode::ClearStoredSizes()
	{
		m_storedSizes.clear();
	}

} // namespace Lina::Editor
