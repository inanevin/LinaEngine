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

#include "Core/GUI/Widgets/Widget.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Common/Platform/LinaGXIncl.hpp"
#include "Common/Data/CommonData.hpp"
#include "Common/Math/Math.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Common/Serialization/StringSerialization.hpp"
#include "Core/Graphics/Resource/Font.hpp"
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina
{

	void Widget::AddChild(Widget* w)
	{
		w->m_parent		 = this;
		w->m_lgxWindow	 = m_lgxWindow;
		w->m_manager	 = m_manager;
		const size_t cur = m_children.size();

		Widget* last = m_children.empty() ? nullptr : m_children[m_children.size() - 1];
		if (last != nullptr)
		{
			last->m_next = w;
			w->m_prev	 = last;
		}

		w->m_next = nullptr;
		m_children.push_back(w);
	}

	void Widget::ExecuteNextFrame(Delegate<void()>&& cb)
	{
		m_executeNextFrame.push_back(cb);
	}

	void Widget::RemoveChild(Widget* w)
	{
		if (w->m_prev != nullptr)
		{
			w->m_prev->m_next = w->m_next;
		}
		if (w->m_next != nullptr)
		{
			w->m_next->m_prev = w->m_prev;
		}

		w->m_controlsManager = nullptr;
		w->m_controlsOwner	 = nullptr;

		auto it = linatl::find_if(m_children.begin(), m_children.end(), [w](Widget* child) -> bool { return w == child; });

		LINA_ASSERT(it != m_children.end(), "");
		m_children.erase(it);

		w->m_next = w->m_prev = nullptr;
	}

	void Widget::DeallocAllChildren()
	{
		for (auto* c : m_children)
			m_manager->Deallocate(c);
	}

	void Widget::RemoveAllChildren()
	{
		m_children.clear();
	}

	void Widget::Initialize()
	{
		linatl::for_each(m_children.begin(), m_children.end(), [this](Widget* child) -> void {
			child->Initialize();
			if (GetIsDisabled())
				child->SetIsDisabled(true);
		});
	}

	void Widget::Draw(int32 threadIndex)
	{
		linatl::for_each(m_children.begin(), m_children.end(), [threadIndex](Widget* child) -> void {
			if (child->GetIsVisible())
				child->Draw(threadIndex);
		});
	}

	void Widget::DrawBorders(int32 threadIndex)
	{
		LinaVG::StyleOptions border;
		border.color = m_colorBorders.AsLVG4();

		if (!Math::Equals(m_borderThickness.left, 0.0f, 0.5f))
		{
			const Vector2 start = m_rect.pos;
			const Vector2 end	= start + Vector2(0, m_rect.size.y);
			border.thickness	= m_borderThickness.left;
			LinaVG::DrawLine(threadIndex, start.AsLVG(), end.AsLVG(), border, LinaVG::LineCapDirection::None, 0.0f, m_drawOrder);
		}

		if (!Math::Equals(m_borderThickness.right, 0.0f, 0.5f))
		{
			const Vector2 start = m_rect.pos + Vector2(m_rect.size.x, 0.0f);
			const Vector2 end	= start + Vector2(0, m_rect.size.y);
			border.thickness	= m_borderThickness.right;
			LinaVG::DrawLine(threadIndex, start.AsLVG(), end.AsLVG(), border, LinaVG::LineCapDirection::None, 0.0f, m_drawOrder);
		}

		if (!Math::Equals(m_borderThickness.top, 0.0f, 0.5f))
		{
			const Vector2 start = m_rect.pos;
			const Vector2 end	= start + Vector2(m_rect.size.x, 0.0f);
			border.thickness	= m_borderThickness.top;
			LinaVG::DrawLine(threadIndex, start.AsLVG(), end.AsLVG(), border, LinaVG::LineCapDirection::None, 0.0f, m_drawOrder);
		}

		if (!Math::Equals(m_borderThickness.bottom, 0.0f, 0.5f))
		{
			const Vector2 start = m_rect.pos + Vector2(0.0f, m_rect.size.y);
			const Vector2 end	= start + Vector2(m_rect.size.x, 0.0f);
			border.thickness	= m_borderThickness.bottom;
			LinaVG::DrawLine(threadIndex, start.AsLVG(), end.AsLVG(), border, LinaVG::LineCapDirection::None, 0.0f, m_drawOrder);
		}
	}

	void Widget::DrawTooltip(int32 threadIndex)
	{
		if (!m_isHovered)
		{
			if (m_customTooltip != nullptr)
			{
				RemoveChild(m_customTooltip);
				m_manager->Deallocate(m_customTooltip);
				m_customTooltip = nullptr;
			}
			return;
		}

		const Vector2 mp = Vector2(Math::FloorToFloat(m_lgxWindow->GetMousePosition().x), Math::FloorToFloat(m_lgxWindow->GetMousePosition().y));

		if (m_buildCustomTooltip != nullptr)
		{
			if (m_customTooltip == nullptr)
			{
				m_customTooltip = m_buildCustomTooltip(m_customTooltipUserData);

				m_customTooltip->GetFlags().Set(WF_CONTROLS_DRAW_ORDER);
				m_customTooltip->SetDrawOrder(TOOLTIP_DRAW_ORDER);

				for (auto* c : m_customTooltip->GetChildren())
				{
					c->GetFlags().Set(WF_CONTROLS_DRAW_ORDER);
					c->SetDrawOrder(TOOLTIP_DRAW_ORDER);
				}
				AddChild(m_customTooltip);
			}

			m_customTooltip->SetPos(mp + Vector2(10, 10));
			// m_customTooltip->Draw(threadIndex);
			return;
		}

		const String& tooltip = GetTooltip();

		if (tooltip.empty())
			return;

		LinaVG::TextOptions textOpts;
		textOpts.font		   = m_manager->GetDefaultFont()->GetLinaVGFont(m_lgxWindow->GetDPIScale());
		const Vector2 textSize = LinaVG::CalculateTextSize(tooltip.c_str(), textOpts);

		const Rect tooltipRect = Rect(mp + Vector2(10, 10), textSize + Vector2(Theme::GetDef().baseIndent * 2.0f, Theme::GetDef().baseIndent));

		LinaVG::StyleOptions bg;
		bg.color					= Theme::GetDef().background1.AsLVG4();
		bg.outlineOptions.thickness = Theme::GetDef().baseOutlineThickness;
		bg.outlineOptions.color		= Theme::GetDef().black.AsLVG4();
		LinaVG::DrawRect(threadIndex, tooltipRect.pos.AsLVG(), tooltipRect.GetEnd().AsLVG(), bg, 0.0f, TOOLTIP_DRAW_ORDER);

		LinaVG::DrawTextNormal(threadIndex, tooltip.c_str(), Vector2(tooltipRect.pos.x + Theme::GetDef().baseIndent, tooltipRect.GetCenter().y + textSize.y * 0.5f).AsLVG(), textOpts, 0.0f, TOOLTIP_DRAW_ORDER);
	}

	void Widget::DebugDraw(int32 threadIndex, int32 drawOrder)
	{
		linatl::for_each(m_children.begin(), m_children.end(), [threadIndex, drawOrder](Widget* child) -> void { child->DebugDraw(threadIndex, drawOrder); });
	}

	void Widget::SetIsHovered()
	{
		if (m_isDisabled)
		{
			m_isHovered = false;
			return;
		}

		auto*		foregroundRoot	= m_manager->GetForegroundRoot();
		const auto& foregroundItems = foregroundRoot->GetChildren();
		if (!foregroundItems.empty() && m_drawOrder < FOREGROUND_DRAW_ORDER)
		{
			m_isHovered = false;
			return;
		}

		const Vector2& pos = m_lgxWindow->GetMousePosition();
		m_isHovered		   = m_rect.IsPointInside(pos);

		if (m_parent && m_parent != m_manager->GetRoot() && m_parent != m_manager->GetForegroundRoot() && !m_parent->GetIsHovered())
			m_isHovered = false;
	}

	Vector2 Widget::GetStartFromMargins()
	{
		return m_rect.pos + Vector2(m_childMargins.left, m_childMargins.top);
	}

	Vector2 Widget::GetEndFromMargins()
	{
		return m_rect.GetEnd() - Vector2(m_childMargins.right, m_childMargins.bottom);
	}

	void Widget::SetIsDisabled(bool isDisabled)
	{
		m_isDisabled = isDisabled;

		for (auto* c : m_children)
			c->SetIsDisabled(isDisabled);
	}

	Vector2 Widget::GetMonitorSize()
	{
		return Vector2(static_cast<float>(m_lgxWindow->GetMonitorSize().x), static_cast<float>(m_lgxWindow->GetMonitorSize().y));
	}

	Vector2 Widget::GetWindowSize()
	{
		return Vector2(static_cast<float>(m_lgxWindow->GetSize().x), static_cast<float>(m_lgxWindow->GetSize().y));
	}

	Vector2 Widget::GetWindowPos()
	{
		return Vector2(static_cast<float>(m_lgxWindow->GetPosition().x), static_cast<float>(m_lgxWindow->GetPosition().y));
	}

	Rect Widget::GetTemporaryAlignedRect()
	{
		// When we haven't calculated our original rect from alignment just yet, but we still need to know the results immediately.
		const float posx  = GetParent()->GetPosX() + GetParent()->GetSizeX() * GetAlignedPosX();
		const float posy  = GetParent()->GetPosY() + GetParent()->GetSizeY() * GetAlignedPosY();
		const float sizex = GetFlags().IsSet(WF_USE_FIXED_SIZE_X) ? GetFixedSizeX() : GetParent()->GetSizeX() * GetAlignedSizeX();
		const float sizey = GetFlags().IsSet(WF_USE_FIXED_SIZE_Y) ? GetFixedSizeY() : GetParent()->GetSizeY() * GetAlignedSizeY();
		return Rect(Vector2(posx, posy), Vector2(sizex, sizey));
	}

	bool Widget::IsWidgetInHierarchy(Widget* widget)
	{
		if (UtilVector::Contains(m_children, widget))
			return true;

		for (auto* c : m_children)
		{
			if (c->IsWidgetInHierarchy(widget))
				return true;
		}

		return false;
	}

} // namespace Lina
