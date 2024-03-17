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

#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Core/GUI/Widgets/Widget.hpp"
#include "Core/GUI/Widgets/Compound/Popup.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Graphics/Resource/Font.hpp"
#include "Core/CommonCore.hpp"
#include "Common/Data/CommonData.hpp"
#include "Common/System/System.hpp"
#include "Common/Platform/LinaGXIncl.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Common/Math/Math.hpp"
#include "LinaGX/Core/InputMappings.hpp"

namespace Lina
{

	void WidgetManager::Initialize(System* system, LinaGX::Window* window)
	{
		m_window = window;
		m_system = system;
		m_window->AddListener(this);
		m_rootWidget = Allocate<Widget>();
		m_rootWidget->SetDebugName("Root");

		m_foregroundRoot = Allocate<Widget>();
		m_foregroundRoot->SetDebugName("ForegroundRoot");

		m_resourceManager = m_system->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager);
		m_defaultFont	  = m_resourceManager->GetResource<Font>(Theme::GetDef().defaultFont);
	}

	void WidgetManager::PreTick()
	{
	}

	void WidgetManager::Tick(float delta, const Vector2ui& size)
	{
		if (m_foregroundRoot->GetChildren().empty())
			m_window->SetCursorType(FindCursorType(m_rootWidget));
		else
			m_window->SetCursorType(FindCursorType(m_foregroundRoot));

		m_foregroundRoot->SetDrawOrder(FOREGROUND_DRAW_ORDER);
		PreTickWidget(m_foregroundRoot);
		PreTickWidget(m_rootWidget);

		// actual tick.
		m_debugDrawYOffset = 0.0f;
		m_foregroundRoot->SetPos(Vector2::Zero);
		m_foregroundRoot->SetSize(Vector2(static_cast<float>(size.x), static_cast<float>(size.y)));
		m_rootWidget->SetPos(Vector2::Zero);
		m_rootWidget->SetSize(Vector2(static_cast<float>(size.x), static_cast<float>(size.y)));

		SizePassWidget(m_foregroundRoot, delta);
		TickWidget(m_foregroundRoot, delta);

		SizePassWidget(m_rootWidget, delta);
		TickWidget(m_rootWidget, delta);
	}

	void WidgetManager::AddToForeground(Widget* w)
	{
		m_foregroundRoot->AddChild(w);
	}

	void WidgetManager::RemoveFromForeground(Widget* w)
	{
		m_foregroundRoot->RemoveChild(w);
	}

	void WidgetManager::Draw(int32 threadIndex)
	{

		m_rootWidget->Draw(threadIndex);

		if (!m_foregroundRoot->GetChildren().empty())
		{
			LinaVG::StyleOptions opts;
			opts.color = LinaVG::Vec4(0.0f, 0.0f, 0.0f, m_foregroundDim);
			LinaVG::DrawRect(threadIndex, Vector2::Zero.AsLVG(), Vector2(static_cast<float>(m_window->GetSize().x), static_cast<float>(m_window->GetSize().y)).AsLVG(), opts, 0.0f, FOREGROUND_DRAW_ORDER - 1);
			m_foregroundRoot->Draw(threadIndex);
		}

		if (!m_foregroundRoot->GetChildren().empty())
			DebugDraw(threadIndex, m_foregroundRoot);
		else
			DebugDraw(threadIndex, m_rootWidget);
	}

	void WidgetManager::Deallocate(Widget* widget)
	{
		for (auto* c : widget->m_children)
			Deallocate(c);

		const TypeID tid = widget->m_tid;
		widget->Destruct();

		PoolAllocator* alloc = m_allocators.at(tid);
		alloc->Free(widget);
	}

	void WidgetManager::Shutdown()
	{
		Deallocate(m_rootWidget);
		Deallocate(m_foregroundRoot);
		m_rootWidget	 = nullptr;
		m_foregroundRoot = nullptr;

		m_window->RemoveListener(this);

		linatl::for_each(m_allocators.begin(), m_allocators.end(), [](auto& pair) -> void { delete pair.second; });
		m_allocators.clear();
	}

	void WidgetManager::OnWindowKey(uint32 keycode, int32 scancode, LinaGX::InputAction inputAction)
	{
		if (keycode == LINAGX_KEY_TAB && inputAction != LinaGX::InputAction::Released)
		{
			if (m_window->GetInput()->GetKey(LINAGX_KEY_LSHIFT))
			{
				if (m_controlsOwner)
				{
					Widget* previous = FindPreviousSelectable(m_controlsOwner);
					if (previous)
					{
						GrabControls(previous);
					}
				}
			}
			else
			{
				Widget* next = FindNextSelectable(m_controlsOwner ? m_controlsOwner : m_rootWidget);
				if (next)
				{
					GrabControls(next);
				}
			}
			return;
		}

		if (m_foregroundRoot->OnKey(keycode, scancode, inputAction))
			return;

		m_rootWidget->OnKey(keycode, scancode, inputAction);
	}

	void WidgetManager::OnWindowMouse(uint32 button, LinaGX::InputAction inputAction)
	{
		// Left click presses to anywhere outside the control owner
		// releases controls from that owner.
		if (button == LINAGX_MOUSE_0 && inputAction == LinaGX::InputAction::Pressed && m_controlsOwner != nullptr)
		{
			if (!m_controlsOwner->GetIsHovered())
				ReleaseControls(m_controlsOwner);
		}

		if (m_foregroundRoot->OnMouse(button, inputAction))
			return;

		m_rootWidget->OnMouse(button, inputAction);
	}

	void WidgetManager::OnWindowMouseWheel(int32 delta)
	{
		// m_rootWidget->OnMouseWheel(static_cast<float>(delta));
	}

	void WidgetManager::OnWindowMouseMove(const LinaGX::LGXVector2& pos)
	{
	}

	void WidgetManager::OnWindowFocus(bool gainedFocus)
	{
	}

	void WidgetManager::OnWindowHoverBegin()
	{
	}

	void WidgetManager::OnWindowHoverEnd()
	{
	}

	void WidgetManager::DebugDraw(int32 threadIndex, Widget* w)
	{
		const bool drawRects = (m_window->GetInput()->GetMouseButton(LINAGX_MOUSE_1));

		for (auto* c : w->m_children)
			DebugDraw(threadIndex, c);

		LinaVG::StyleOptions opts;
		opts.isFilled  = false;
		opts.thickness = 2.0f;

		const Rect			rect	= w->GetRect();
		const Vector2&		mp		= m_window->GetMousePosition();
		auto*				lvgFont = m_resourceManager->GetResource<Font>(DEFAULT_FONT_SID)->GetLinaVGFont(m_window->GetDPIScale());
		LinaVG::TextOptions textOpts;
		textOpts.font = lvgFont;

		if (drawRects)
			LinaVG::DrawRect(threadIndex, rect.pos.AsLVG(), (rect.pos + rect.size).AsLVG(), opts, 0.0f, 1000.0f);

		if (w->m_isHovered)
		{
			const Vector2 sz = LinaVG::CalculateTextSize(w->GetDebugName().c_str(), textOpts);
			LinaVG::DrawTextNormal(threadIndex, w->GetDebugName().c_str(), (mp + Vector2(15, 15 + m_debugDrawYOffset)).AsLVG(), textOpts, 0.0f, DEBUG_DRAW_ORDER);

			const String rectStr = "Pos: (" + UtilStr::FloatToString(w->GetPos().x, 1) + ", " + UtilStr::FloatToString(w->GetPos().y, 1) + ") Size: (" + UtilStr::FloatToString(w->GetSize().x, 1) + ", " + UtilStr::FloatToString(w->GetSize().y, 1) + ")";
			LinaVG::DrawTextNormal(threadIndex, rectStr.c_str(), (mp + Vector2(15 + 15 + sz.x, 15 + m_debugDrawYOffset)).AsLVG(), textOpts, 0.0f, DEBUG_DRAW_ORDER);

			m_debugDrawYOffset += lvgFont->m_size * 1.5f;
		}

		if (drawRects)
			w->DebugDraw(threadIndex, DEBUG_DRAW_ORDER);
	}

	void WidgetManager::SetClip(int32 threadIndex, const Rect& r, const TBLR& margins)
	{
		const ClipData cd = {
			.rect	 = r,
			.margins = margins,
		};

		m_clipStack.push_back(cd);

		LinaVG::SetClipPosX(static_cast<uint32>(r.pos.x + margins.left), threadIndex);
		LinaVG::SetClipPosY(static_cast<uint32>(r.pos.y + margins.top), threadIndex);
		LinaVG::SetClipSizeX(static_cast<uint32>(r.size.x - (margins.left + margins.right)), threadIndex);
		LinaVG::SetClipSizeY(static_cast<uint32>(r.size.y - (margins.top + margins.bottom)), threadIndex);
	}

	void WidgetManager::UnsetClip(int32 threadIndex)
	{
		m_clipStack.pop_back();

		if (m_clipStack.empty())
		{
			LinaVG::SetClipPosX(0, threadIndex);
			LinaVG::SetClipPosY(0, threadIndex);
			LinaVG::SetClipSizeX(0, threadIndex);
			LinaVG::SetClipSizeY(0, threadIndex);
		}
		else
		{
			const ClipData& cd = m_clipStack[m_clipStack.size() - 1];
			LinaVG::SetClipPosX(static_cast<uint32>(cd.rect.pos.x + cd.margins.left), threadIndex);
			LinaVG::SetClipPosY(static_cast<uint32>(cd.rect.pos.y + cd.margins.top), threadIndex);
			LinaVG::SetClipSizeX(static_cast<uint32>(cd.rect.size.x - (cd.margins.left + cd.margins.right)), threadIndex);
			LinaVG::SetClipSizeY(static_cast<uint32>(cd.rect.size.y - (cd.margins.top + cd.margins.bottom)), threadIndex);
		}
	}

	Widget* WidgetManager::FindNextSelectable(Widget* start)
	{
		if (!start)
			return nullptr;

		Widget* current = start;
		do
		{
			// Depth-first search for the next selectable widget
			if (!current->m_children.empty())
			{
				current = current->m_children[0];
			}
			else
			{
				while (current != nullptr && current->m_next == nullptr)
				{
					current = current->m_parent;
				}
				if (current != nullptr)
				{
					current = current->m_next;
				}
			}

			if (current && current->GetFlags().IsSet(WF_SELECTABLE) && !current->GetIsDisabled())
			{
				return current;
			}
		} while (current != nullptr && current != start);

		return nullptr;
	}

	Widget* WidgetManager::FindPreviousSelectable(Widget* start)
	{
		if (!start)
			return nullptr;

		Widget* current = start;
		do
		{
			// Reverse depth-first search for the previous selectable widget
			if (current->m_prev)
			{
				current = current->m_prev;
				while (!current->m_children.empty())
				{
					current = current->m_children.back();
				}
			}
			else
			{
				current = current->m_parent;
			}

			if (current && current->GetFlags().IsSet(WF_SELECTABLE) && !current->GetIsDisabled())
			{
				return current;
			}
		} while (current != nullptr && current != start);

		return nullptr;
	}

	LinaGX::CursorType WidgetManager::FindCursorType(Widget* w)
	{
		const LinaGX::CursorType cursorType = w->GetCursorOverride();
		if (cursorType != LinaGX::CursorType::Default)
		{
			return cursorType;
		}

		const Vector<Widget*> children = w->GetChildren();
		for (Widget* c : children)
		{
			LinaGX::CursorType childCursorType = FindCursorType(c);
			if (childCursorType != LinaGX::CursorType::Default)
			{
				return childCursorType;
			}
		}
		return LinaGX::CursorType::Default;
	}

	void WidgetManager::PreTickWidget(Widget* w)
	{
		if (!w->GetFlags().IsSet(WF_CONTROLS_DRAW_ORDER) && w->GetParent())
			w->SetDrawOrder(w->GetParent()->GetDrawOrder());

		w->SetIsHovered();
		w->PreTick();
		for (auto* c : w->GetChildren())
			PreTickWidget(c);
	}

	namespace
	{
		float CalculateAlignedPosX(Widget* w)
		{
			const Vector2			 alignedPos = w->GetAlignedPos();
			const PosAlignmentSource src		= w->GetPosAlignmentSourceX();
			const Vector2			 start		= w->GetParent()->GetStartFromMargins();
			const Vector2			 end		= w->GetParent()->GetEndFromMargins();
			const Vector2			 sz			= end - start;
			if (src == PosAlignmentSource::Center)
				return start.x + sz.x * alignedPos.x - w->GetHalfSizeX();
			else if (src == PosAlignmentSource::End)
				return start.x + sz.x * alignedPos.x - w->GetSizeX();

			return start.x + sz.x * alignedPos.x;
		}

		float CalculateAlignedPosY(Widget* w)
		{
			const Vector2			 alignedPos = w->GetAlignedPos();
			const PosAlignmentSource src		= w->GetPosAlignmentSourceY();
			const Vector2			 start		= w->GetParent()->GetStartFromMargins();
			const Vector2			 end		= w->GetParent()->GetEndFromMargins();
			const Vector2			 sz			= end - start;
			if (src == PosAlignmentSource::Center)
				return start.y + sz.y * alignedPos.y - w->GetHalfSizeY();
			else if (src == PosAlignmentSource::End)
				return start.y + sz.y * alignedPos.y - w->GetSizeY();

			return start.y + sz.y * alignedPos.y;
		}
	} // namespace

	void WidgetManager::SizePassWidget(Widget* w, float delta)
	{
		w->CalculateSize(delta);

		Vector<Widget*> expandingChildren;
		Vector2			totalNonExpandingSize = Vector2::Zero;

		for (auto* c : w->GetChildren())
		{

			if (c->GetFlags().IsSet(WF_USE_FIXED_SIZE_X))
				c->SetSizeX(c->GetFixedSizeX() * m_window->GetDPIScale());

			if (c->GetFlags().IsSet(WF_USE_FIXED_SIZE_Y))
				c->SetSizeY(c->GetFixedSizeY() * m_window->GetDPIScale());

			c->CalculateSize(delta);

			const Vector2 alignedSize = c->GetAlignedSize();

			bool isExpandingX = false;
			bool isExpandingY = false;

			if (c->GetFlags().IsSet(WF_SIZE_ALIGN_X))
			{
				if (!Math::Equals(alignedSize.x, 0.0f, 0.0001f))
					c->SetSizeX((w->GetSizeX() - (w->GetChildMargins().left + w->GetChildMargins().right)) * alignedSize.x);
				else
					isExpandingX = true;
			}

			if (c->GetFlags().IsSet(WF_SIZE_ALIGN_Y))
			{
				if (!Math::Equals(alignedSize.y, 0.0f, 0.0001f))
					c->SetSizeY((w->GetSizeY() - (w->GetChildMargins().top + w->GetChildMargins().bottom)) * alignedSize.y);
				else
					isExpandingY = true;
			}

			if (c->GetFlags().IsSet(WF_SIZE_X_COPY_Y))
				c->SetSizeX(c->GetSizeY());

			if (c->GetFlags().IsSet(WF_SIZE_Y_COPY_X))
				c->SetSizeY(c->GetSizeX());

			if (!isExpandingX)
				totalNonExpandingSize.x += c->GetSizeX();

			if (!isExpandingY)
				totalNonExpandingSize.y += c->GetSizeY();

			if (isExpandingX || isExpandingY)
				expandingChildren.push_back(c);
		}

		if (!expandingChildren.empty())
		{
			const Vector2 start						 = w->GetStartFromMargins();
			const Vector2 end						 = w->GetEndFromMargins();
			const Vector2 size						 = end - start;
			const Vector2 totalAvailableSpace		 = size - totalNonExpandingSize;
			const float	  totalPad					 = w->GetChildPadding() * static_cast<float>(w->GetChildren().size() - 1);
			const Vector2 totalAvailableAfterPadding = totalAvailableSpace - Vector2(totalPad, totalPad);
			const Vector2 targetSize				 = totalAvailableAfterPadding / (static_cast<float>(expandingChildren.size()));

			for (auto* c : expandingChildren)
			{
				if (c->GetFlags().IsSet(WF_SIZE_ALIGN_X) && Math::Equals(c->GetAlignedSizeX(), 0.0f, 0.0001f))
					c->SetSizeX(targetSize.x);

				if (c->GetFlags().IsSet(WF_SIZE_ALIGN_Y) && Math::Equals(c->GetAlignedSizeY(), 0.0f, 0.0001f))
					c->SetSizeY(targetSize.y);
			}
		}

		for (auto* c : w->GetChildren())
			SizePassWidget(c, delta);
	}

	void WidgetManager::TickWidget(Widget* w, float delta)
	{
		if (w->GetFlags().IsSet(WF_POS_ALIGN_X) && w->GetParent())
			w->SetPosX(CalculateAlignedPosX(w));

		if (w->GetFlags().IsSet(WF_POS_ALIGN_Y) && w->GetParent())
			w->SetPosY(CalculateAlignedPosY(w));

		w->Tick(delta);

		for (auto* c : w->GetChildren())
			TickWidget(c, delta);
	}

} // namespace Lina
