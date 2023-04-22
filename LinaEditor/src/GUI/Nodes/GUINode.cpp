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

namespace Lina::Editor
{
	GUINode::GUINode(GUIDrawerBase* drawer, int drawOrder) : m_drawOrder(drawOrder)
	{
		m_drawer	= drawer;
		m_swapchain = m_drawer->GetSwapchain();
		m_editor	= m_drawer->GetEditor();
		m_window	= m_swapchain->GetWindow();
	}

	GUINode::~GUINode()
	{
		for (auto c : m_children)
			delete c;

		m_children.clear();
	}

	void GUINode::Draw(int threadID)
	{
		if (!m_visible)
			return;

		const uint32 sz = static_cast<uint32>(m_children.size());
		for (uint32 i = 0; i < sz; i++)
			m_children[i]->Draw(threadID);
	}

	bool GUINode::OnKey(uint32 key, InputAction action)
	{

		const uint32 sz = static_cast<uint32>(m_children.size());
		for (uint32 i = 0; i < sz; i++)
		{
			if (m_children[i]->OnKey(key, action))
				return true;
		}

		return false;
	}

	bool GUINode::OnMouse(uint32 button, InputAction action)
	{
		static GUINode* lastPressedNode = nullptr;

		if (button == LINA_MOUSE_0)
		{
			if (action == InputAction::Pressed)
			{
				if (GetIsHovered())
				{
					lastPressedNode = this;
					m_isPressed		= true;
					m_isDragging	= true;
					OnDragBegin();
					m_dragStartMousePos	  = m_window->GetMousePosition();
					m_dragStartMouseDelta = Vector2(m_dragStartMousePos) - m_rect.pos;
				}
			}
			else if (action == InputAction::Released)
			{
				if (lastPressedNode == this && GetIsHovered())
				{
					lastPressedNode = nullptr;
					OnClicked(LINA_MOUSE_0);
				}

				if (m_isDragging)
					OnDragEnd();

				m_isPressed	 = false;
				m_isDragging = false;
			}
		}
		else
		{
			if (action == InputAction::Released)
				OnClicked(button);
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

	void GUINode::OnPayloadCreated(PayloadType type, void* data)
	{
		const uint32 sz = static_cast<uint32>(m_children.size());
		for (uint32 i = 0; i < sz; i++)
			m_children[i]->OnPayloadCreated(type, data);
	}

	bool GUINode::OnPayloadDropped(PayloadType type, void* data)
	{
		const uint32 sz		= static_cast<uint32>(m_children.size());
		bool		 retVal = false;

		for (uint32 i = 0; i < sz; i++)
		{
			const bool ret = m_children[i]->OnPayloadDropped(type, data);

			if (ret)
				retVal = true;
		}

		return retVal;
	}

	void GUINode::SaveToStream(OStream& stream)
	{
		stream << m_sid << m_visible << m_drawOrder << m_lastDpi;
		m_rect.pos.SaveToStream(stream);
		m_rect.size.SaveToStream(stream);
		m_lastCalculatedSize.SaveToStream(stream);
		StringSerialization::SaveToStream(stream, m_title);
	}

	void GUINode::LoadFromStream(IStream& stream)
	{
		stream >> m_sid >> m_visible >> m_drawOrder >> m_lastDpi;
		m_rect.pos.LoadFromStream(stream);
		m_rect.size.LoadFromStream(stream);
		m_lastCalculatedSize.LoadFromStream(stream);
		StringSerialization::LoadFromStream(stream, m_title);
	}

	GUINode* GUINode::AddChildren(GUINode* node)
	{
		m_children.push_back(node);
		return this;
	}

	GUINode* GUINode::RemoveChildren(GUINode* node)
	{
		auto it = linatl::find_if(m_children.begin(), m_children.end(), [node](GUINode* child) { return child == node; });
		m_children.erase(it);
		return this;
	}

	GUINode* GUINode::SetVisible(bool visible)
	{
		m_visible		= visible;
		const uint32 sz = static_cast<uint32>(m_children.size());
		for (uint32 i = 0; i < sz; i++)
			m_children[i]->SetVisible(visible);

		return this;
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

} // namespace Lina::Editor
