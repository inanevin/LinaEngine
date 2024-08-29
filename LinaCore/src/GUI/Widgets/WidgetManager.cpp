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
#include "Core/GUI/Widgets/Layout/ScrollArea.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Graphics/Resource/Font.hpp"
#include "Core/Graphics/GfxManager.hpp"
#include "Core/CommonCore.hpp"
#include "Common/Data/CommonData.hpp"
#include "Common/System/System.hpp"
#include "Common/Platform/LinaGXIncl.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Common/Math/Math.hpp"
#include "LinaGX/Core/InputMappings.hpp"
#include "Common/Profiling/Profiler.hpp"

namespace Lina
{

	void WidgetManager::Initialize(ResourceManagerV2* resourceManager, LinaGX::Window* window, LinaVG::Drawer* drawer)
	{
		m_resourceManagerV2 = resourceManager;
		m_lvg				= drawer;
		m_window			= window;
		m_window->AddListener(this);
		m_rootWidget								 = Allocate<Widget>();
		m_rootWidget->GetWidgetProps().debugName	 = "Root";
		m_foregroundRoot							 = Allocate<Widget>();
		m_foregroundRoot->GetWidgetProps().debugName = "ForegroundRoot";
		m_defaultFont								 = resourceManager->GetResource<Font>(Theme::GetDef().defaultFont);
	}

	void WidgetManager::InitializeWidget(Widget* w)
	{
		PassPreTick(w);
		PassCalculateSize(w, 0.016f);
		PassTick(w, 0.016f);
	}

	void WidgetManager::PreTick()
	{
		for (auto* w : m_killList)
		{
			w->GetParent()->RemoveChild(w);
			Deallocate(w);
		}

		m_killList.clear();

		if (m_window->HasFocus())
		{
			if (m_foregroundRoot->GetChildren().empty())
				m_window->SetCursorType(FindCursorType(m_rootWidget));
			else
				m_window->SetCursorType(FindCursorType(m_foregroundRoot));
		}

		// m_foregroundRoot->SetDrawOrder(FOREGROUND_DRAW_ORDER);
		PassPreTick(m_foregroundRoot);
		PassPreTick(m_rootWidget);
	}

	void WidgetManager::Tick(float delta, const Vector2ui& size)
	{
		// actual tick.
		m_debugDrawYOffset = 0.0f;
		m_foregroundRoot->SetPos(Vector2::Zero);
		m_foregroundRoot->SetSize(Vector2(static_cast<float>(size.x), static_cast<float>(size.y)));
		m_rootWidget->SetPos(Vector2::Zero);
		m_rootWidget->SetSize(Vector2(static_cast<float>(size.x), static_cast<float>(size.y)));

		PassCalculateSize(m_foregroundRoot, delta);
		PassTick(m_foregroundRoot, delta);

		PassCalculateSize(m_rootWidget, delta);
		PassTick(m_rootWidget, delta);
	}

	void WidgetManager::AddToForeground(Widget* w, float foregroundDim)
	{
		if (foregroundDim > -0.1f)
			SetForegroundDim(foregroundDim);

		w->SetDrawOrder(FOREGROUND_DRAW_ORDER + static_cast<int32>(m_foregroundRoot->GetChildren().size()));
		w->GetFlags().Set(WF_CONTROLS_DRAW_ORDER);
		m_foregroundRoot->AddChild(w);
	}

	void WidgetManager::RemoveFromForeground(Widget* w)
	{
		w->GetFlags().Remove(WF_CONTROLS_DRAW_ORDER);
		m_foregroundRoot->RemoveChild(w);
	}

	void WidgetManager::Draw()
	{
		PROFILER_FUNCTION();
		m_rootWidget->Draw();

		if (!m_foregroundRoot->GetChildren().empty())
		{
			LinaVG::StyleOptions opts;
			opts.color = LinaVG::Vec4(0.0f, 0.0f, 0.0f, m_foregroundDim);
			m_lvg->DrawRect(Vector2::Zero.AsLVG(), Vector2(static_cast<float>(m_window->GetSize().x), static_cast<float>(m_window->GetSize().y)).AsLVG(), opts, 0.0f, FOREGROUND_DRAW_ORDER);
			m_foregroundRoot->Draw();
		}

		if (!m_foregroundRoot->GetChildren().empty())
			DebugDraw(m_foregroundRoot);
		else
			DebugDraw(m_rootWidget);
	}

	void WidgetManager::Deallocate(Widget* widget)
	{
		// If the widget is the active controls owner of a manager
		// if (widget->GetLocalControlsManager() && widget->GetLocalControlsManager()->GetLocalControlsOwner() == widget)
		// 	widget->GetLocalControlsManager()->SetLocalControlsOwner(nullptr);

		// If the widget is the manager of someone
		// if (widget->GetLocalControlsOwner())
		// 	widget->GetLocalControlsOwner()->SetLocalControlsManager(nullptr);

		if (IsControlsOwner(widget))
			ReleaseControls(widget);

		// if (m_lastControlsManager == widget)
		// 	m_lastControlsManager = nullptr;

		widget->m_customTooltip = nullptr;

		widget->PreDestruct();
		for (auto* c : widget->m_children)
			Deallocate(c);

		const TypeID tid = widget->m_tid;
		widget->Destruct();

		WidgetCacheBase* cache = m_widgetCaches.at(tid);
		cache->Destroy(widget);
	}

	void WidgetManager::Shutdown()
	{
		Deallocate(m_rootWidget);
		Deallocate(m_foregroundRoot);
		m_rootWidget	 = nullptr;
		m_foregroundRoot = nullptr;

		m_window->RemoveListener(this);

		for (auto [tid, cache] : m_widgetCaches)
			delete cache;

		m_widgetCaches.clear();
	}

	void WidgetManager::OnWindowKey(LinaGX::Window* window, uint32 keycode, int32 scancode, LinaGX::InputAction inputAction)
	{
		if (keycode == LINAGX_KEY_TAB && inputAction != LinaGX::InputAction::Released && GetControlsOwner() != nullptr)
		{
			if (m_window->GetInput()->GetKey(LINAGX_KEY_LSHIFT))
				MoveControlsToPrev();
			else
				MoveControlsToNext();
			return;
		}

		if (PassKey(m_foregroundRoot, keycode, scancode, inputAction))
			return;

		PassKey(m_rootWidget, keycode, scancode, inputAction);
	}

	void WidgetManager::OnWindowMouse(LinaGX::Window* window, uint32 button, LinaGX::InputAction inputAction)
	{
		// If we have some items in the foreground
		// check if any was clicked, if not, then remove the non-blocker ones
		// this is used for removing popups mostly.
		if (inputAction == LinaGX::InputAction::Pressed && !m_foregroundRoot->GetChildren().empty())
		{
			Vector<Widget*> removeList;
			for (auto* c : m_foregroundRoot->GetChildren())
			{
				if (!c->GetIsHovered())
				{
					if (!c->GetFlags().IsSet(WF_FOREGROUND_BLOCKER))
						removeList.push_back(c);
				}
			}

			for (auto* w : removeList)
			{
				RemoveFromForeground(w);
				Deallocate(w);
			}

			// if(!removeList.empty())
			//     return;
		}

		if (button == LINAGX_MOUSE_0 && inputAction == LinaGX::InputAction::Pressed && m_controlOwner != nullptr && !m_controlOwner->GetIsHovered())
		{
			ReleaseControls(m_controlOwner);
			// for (Widget* w : m_controlsOwners)
			// {
			// 	if (w->GetIsHovered())
			// 		continue;
			//
			// 	if (m_window->GetInput()->GetKey(LINAGX_KEY_LCTRL) && w->GetFlags().IsSet(WF_ALLOW_MULTICONTROL))
			// 		continue;
			//
			// 	ReleaseControls(w);
			// }
		}

		Vector<Widget*> sortedChildren = m_foregroundRoot->GetChildren();
		linatl::sort(sortedChildren.begin(), sortedChildren.end(), [](Widget* w, Widget* other) -> bool { return w->GetDrawOrder() > other->GetDrawOrder(); });
		for (Widget* child : sortedChildren)
		{
			if (PassMouse(child, button, inputAction))
				return;
		}

		PassMouse(m_rootWidget, button, inputAction);
	}

	void WidgetManager::OnWindowMouseWheel(LinaGX::Window* window, float amt)
	{
		if (PassMouseWheel(m_foregroundRoot, amt))
			return;
		PassMouseWheel(m_rootWidget, amt);
	}

	void WidgetManager::OnWindowMouseMove(LinaGX::Window* window, const LinaGX::LGXVector2& pos)
	{
		if (PassMousePos(m_foregroundRoot, pos))
			return;
		PassMousePos(m_rootWidget, pos);
	}

	void WidgetManager::OnWindowFocus(LinaGX::Window* window, bool gainedFocus)
	{
	}

	void WidgetManager::OnWindowHoverBegin(LinaGX::Window* window)
	{
		m_isMouseHoveringWindow = true;
	}

	void WidgetManager::OnWindowHoverEnd(LinaGX::Window* window)
	{
		m_isMouseHoveringWindow = false;
	}

	void WidgetManager::DebugDraw(Widget* w)
	{
		const bool drawRects = (m_window->GetInput()->GetKey(LINAGX_KEY_K));

		if (!drawRects)
			return;

		for (auto* c : w->m_children)
			DebugDraw(c);

		LinaVG::StyleOptions opts;
		opts.isFilled  = false;
		opts.thickness = 2.0f;

		const Rect			rect	= w->GetRect();
		const Vector2&		mp		= m_window->GetMousePosition();
		auto*				lvgFont = m_resourceManagerV2->GetResource<Font>(Theme::GetDef().defaultFont)->GetLinaVGFont(m_window->GetDPIScale());
		LinaVG::TextOptions textOpts;
		textOpts.font = lvgFont;

		if (drawRects)
			m_lvg->DrawRect(rect.pos.AsLVG(), (rect.pos + rect.size).AsLVG(), opts, 0.0f, 1000);

		if (w->m_isHovered)
		{
			const Vector2 sz = m_lvg->CalculateTextSize(w->GetWidgetProps().debugName.c_str(), textOpts);
			m_lvg->DrawTextNormal(w->GetWidgetProps().debugName.c_str(), (mp + Vector2(15, 15 + m_debugDrawYOffset)).AsLVG(), textOpts, 0.0f, DEBUG_DRAW_ORDER);

			const String  rectStr = "Pos: (" + UtilStr::FloatToString(w->GetPos().x, 1) + ", " + UtilStr::FloatToString(w->GetPos().y, 1) + ") Size: (" + UtilStr::FloatToString(w->GetSize().x, 1) + ", " + UtilStr::FloatToString(w->GetSize().y, 1) + ")";
			const Vector2 sz2	  = m_lvg->CalculateTextSize(rectStr.c_str(), textOpts);
			m_lvg->DrawTextNormal(rectStr.c_str(), (mp + Vector2(15 + sz.x + 15, 15 + m_debugDrawYOffset)).AsLVG(), textOpts, 0.0f, DEBUG_DRAW_ORDER);

			const String drawOrder = "DO: " + TO_STRING(w->GetDrawOrder());
			m_lvg->DrawTextNormal(drawOrder.c_str(), (mp + Vector2(15 + sz.x + 15 + sz2.x + 15, 15 + m_debugDrawYOffset)).AsLVG(), textOpts, 0.0f, DEBUG_DRAW_ORDER);
			m_debugDrawYOffset += lvgFont->m_size * 1.5f;
		}

		if (drawRects)
			w->DebugDraw(DEBUG_DRAW_ORDER);
	}

	void WidgetManager::SetClip(const Rect& r, const TBLR& margins)
	{
		const ClipData cd = {
			.rect	 = r,
			.margins = margins,
		};

		m_clipStack.push_back(cd);

		m_lvg->SetClipPosX(static_cast<uint32>(r.pos.x + margins.left));
		m_lvg->SetClipPosY(static_cast<uint32>(r.pos.y + margins.top));
		m_lvg->SetClipSizeX(static_cast<uint32>(r.size.x - (margins.left + margins.right)));
		m_lvg->SetClipSizeY(static_cast<uint32>(r.size.y - (margins.top + margins.bottom)));
	}

	void WidgetManager::UnsetClip()
	{
		m_clipStack.pop_back();

		if (m_clipStack.empty())
		{
			m_lvg->SetClipPosX(0);
			m_lvg->SetClipPosY(0);
			m_lvg->SetClipSizeX(0);
			m_lvg->SetClipSizeY(0);
		}
		else
		{
			const ClipData& cd = m_clipStack[m_clipStack.size() - 1];
			m_lvg->SetClipPosX(static_cast<uint32>(cd.rect.pos.x + cd.margins.left));
			m_lvg->SetClipPosY(static_cast<uint32>(cd.rect.pos.y + cd.margins.top));
			m_lvg->SetClipSizeX(static_cast<uint32>(cd.rect.size.x - (cd.margins.left + cd.margins.right)));
			m_lvg->SetClipSizeY(static_cast<uint32>(cd.rect.size.y - (cd.margins.top + cd.margins.bottom)));
		}
	}

	void WidgetManager::AddToKillList(Widget* w)
	{
		m_killList.push_back(w);
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

	namespace
	{
		float CalculateAlignedPosX(Widget* w)
		{
			const Vector2 alignedPos = w->GetAlignedPos();
			const Anchor  src		 = w->GetPosAlignmentSourceX();
			const Vector2 start		 = w->GetParent()->GetStartFromMargins();
			const Vector2 end		 = w->GetParent()->GetEndFromMargins();
			const Vector2 sz		 = end - start;
			if (src == Anchor::Center)
				return start.x + sz.x * alignedPos.x - w->GetHalfSizeX();
			else if (src == Anchor::End)
				return start.x + sz.x * alignedPos.x - w->GetSizeX();

			return start.x + sz.x * alignedPos.x;
		}

		float CalculateAlignedPosY(Widget* w)
		{
			const Vector2 alignedPos = w->GetAlignedPos();
			const Anchor  src		 = w->GetPosAlignmentSourceY();
			const Vector2 start		 = w->GetParent()->GetStartFromMargins();
			const Vector2 end		 = w->GetParent()->GetEndFromMargins();
			const Vector2 sz		 = end - start;
			if (src == Anchor::Center)
				return start.y + sz.y * alignedPos.y - w->GetHalfSizeY();
			else if (src == Anchor::End)
				return start.y + sz.y * alignedPos.y - w->GetSizeY();

			return start.y + sz.y * alignedPos.y;
		}
	} // namespace

	bool WidgetManager::PassKey(Widget* widget, uint32 keycode, int32 scancode, LinaGX::InputAction inputAction)
	{
		if (!widget->GetIsDisabled() && widget->GetIsVisible() && widget->OnKey(keycode, scancode, inputAction) && !widget->GetFlags().IsSet(WF_KEY_PASSTHRU))
			return true;

		for (auto* c : widget->GetChildren())
		{
			if (PassKey(c, keycode, scancode, inputAction))
				return true;
		}

		return false;
	}

	bool WidgetManager::PassMouse(Widget* widget, uint32 button, LinaGX::InputAction inputAction)
	{

		if (!widget->GetIsDisabled() && widget->GetIsVisible() && widget->OnMouse(button, inputAction) && !widget->GetFlags().IsSet(WF_MOUSE_PASSTHRU))
			return true;

		for (auto* c : widget->GetChildren())
		{
			if (PassMouse(c, button, inputAction))
				return true;
		}

		return false;
	}

	bool WidgetManager::PassMouseWheel(Widget* widget, float amt)
	{
		if (!widget->GetIsDisabled() && widget->GetIsVisible() && widget->OnMouseWheel(amt) && !widget->GetFlags().IsSet(WF_MOUSE_PASSTHRU))
			return true;

		for (auto* c : widget->GetChildren())
		{
			if (PassMouseWheel(c, amt))
				return true;
		}

		return false;
	}

	bool WidgetManager::PassMousePos(Widget* widget, const Vector2& pos)
	{
		if (!widget->GetIsDisabled() && widget->GetIsVisible() && widget->OnMousePos(pos) && !widget->GetFlags().IsSet(WF_MOUSE_PASSTHRU))
			return true;

		for (auto* c : widget->GetChildren())
		{
			if (PassMousePos(c, pos))
				return true;
		}

		return false;
	}

	void WidgetManager::PassPreTick(Widget* w)
	{
		for (auto cb : w->m_executeNextFrame)
			cb();

		w->m_executeNextFrame.clear();

		if (!w->GetFlags().IsSet(WF_CONTROLS_DRAW_ORDER) && w->GetParent())
			w->SetDrawOrder(w->GetParent()->GetDrawOrder() + w->GetWidgetProps().drawOrderIncrement);

		w->SetIsHovered();
		w->CheckCustomTooltip();
		w->PreTick();
		for (auto* c : w->GetChildren())
			PassPreTick(c);
	}

	void WidgetManager::PassCalculateSize(Widget* w, float delta)
	{
		if (!w->GetFlags().IsSet(WF_SIZE_AFTER_CHILDREN))
			w->CalculateSize(delta);

		Vector<Widget*> expandingChildren;
		Vector2			totalNonExpandingSize = Vector2::Zero;

		if (w->GetFlags().IsSet(WF_USE_FIXED_SIZE_X))
			w->SetSizeX(w->GetFixedSizeX() * m_window->GetDPIScale());

		if (w->GetFlags().IsSet(WF_USE_FIXED_SIZE_Y))
			w->SetSizeY(w->GetFixedSizeY() * m_window->GetDPIScale());

		for (auto* c : w->GetChildren())
		{

			c->CalculateSize(delta);

			const Vector2 alignedSize = c->GetAlignedSize();

			bool isExpandingX = false;
			bool isExpandingY = false;

			if (c->GetFlags().IsSet(WF_SIZE_ALIGN_X))
			{
				if (!Math::Equals(alignedSize.x, 0.0f, 0.0001f))
					c->SetSizeX((w->GetSizeX() - (w->GetWidgetProps().childMargins.left + w->GetWidgetProps().childMargins.right)) * alignedSize.x);
				else
					isExpandingX = true;
			}

			if (c->GetFlags().IsSet(WF_SIZE_ALIGN_Y))
			{
				if (!Math::Equals(alignedSize.y, 0.0f, 0.0001f))
					c->SetSizeY((w->GetSizeY() - (w->GetWidgetProps().childMargins.top + w->GetWidgetProps().childMargins.bottom)) * alignedSize.y);
				else
					isExpandingY = true;
			}

			if (c->GetFlags().IsSet(WF_SIZE_X_COPY_Y))
				c->SetSizeX(c->GetSizeY());

			if (c->GetFlags().IsSet(WF_SIZE_Y_COPY_X))
				c->SetSizeY(c->GetSizeX());

			if (!c->GetFlags().IsSet(WF_SKIP_FLOORING))
			{
				// c->SetSizeX(Math::FloorToFloatEven(c->GetSizeX()));
				// c->SetSizeY(Math::FloorToFloatEven(c->GetSizeY()));
			}

			if (!isExpandingX)
				totalNonExpandingSize.x += c->GetSizeX();

			if (!isExpandingY)
				totalNonExpandingSize.y += c->GetSizeY();

			if (isExpandingX || isExpandingY)
				expandingChildren.push_back(c);
		}

		if (w->GetFlags().IsSet(WF_SIZE_AFTER_CHILDREN))
			w->CalculateSize(delta);

		if (w->GetFlags().IsSet(WF_SIZE_X_TOTAL_CHILDREN))
		{
			float total = 0.0f;
			int32 idx	= 0;
			for (auto* c : w->GetChildren())
			{
				if (c->GetFlags().IsSet(WF_SIZE_ALIGN_X))
					continue;

				if (idx != 0)
					total += w->GetWidgetProps().childPadding;

				total += c->GetSizeX();
				idx++;
			}

			// total *= w->GetAlignedSizeX();
			w->SetSizeX(total + w->GetWidgetProps().childMargins.left + w->GetWidgetProps().childMargins.right);
		}

		if (w->GetFlags().IsSet(WF_SIZE_Y_TOTAL_CHILDREN))
		{
			float total = 0.0f;
			int32 idx	= 0;
			for (auto* c : w->GetChildren())
			{
				if (c->GetFlags().IsSet(WF_SIZE_ALIGN_Y))
					continue;

				if (idx != 0)
					total += w->GetWidgetProps().childPadding;

				total += c->GetSizeY();
				idx++;
			}

			total *= w->GetAlignedSizeY();
			w->SetSizeY(total + w->GetWidgetProps().childMargins.top + w->GetWidgetProps().childMargins.bottom);
		}

		if (w->GetFlags().IsSet(WF_SIZE_X_MAX_CHILDREN))
		{
			float max = 0.0f;
			for (auto* c : w->GetChildren())
			{
				if (c->GetFlags().IsSet(WF_SIZE_ALIGN_X))
					continue;

				max = Math::Max(max, c->GetSizeX());
			}

			w->SetSizeX(max * w->GetAlignedSizeX() + w->GetWidgetProps().childMargins.left + w->GetWidgetProps().childMargins.right);
		}

		if (w->GetFlags().IsSet(WF_SIZE_Y_MAX_CHILDREN))
		{
			float max = 0.0f;
			for (auto* c : w->GetChildren())
			{
				if (c->GetFlags().IsSet(WF_SIZE_ALIGN_Y))
					continue;

				max = Math::Max(max, c->GetSizeY());
			}
			w->SetSizeY(max * w->GetAlignedSizeY() + w->GetWidgetProps().childMargins.top + w->GetWidgetProps().childMargins.bottom);
		}

		if (!expandingChildren.empty())
		{
			const Vector2 start						 = w->GetStartFromMargins();
			const Vector2 end						 = w->GetEndFromMargins();
			const Vector2 size						 = end - start;
			const Vector2 totalAvailableSpace		 = size - totalNonExpandingSize;
			const float	  totalPad					 = w->GetWidgetProps().childPadding * static_cast<float>(w->GetChildren().size() - 1);
			const Vector2 totalAvailableAfterPadding = totalAvailableSpace - Vector2(totalPad, totalPad);
			const Vector2 targetSize				 = totalAvailableAfterPadding / (static_cast<float>(expandingChildren.size()));

			for (auto* c : expandingChildren)
			{
				const bool skipFlooring = true || c->GetFlags().IsSet(WF_SKIP_FLOORING);

				if (c->GetFlags().IsSet(WF_SIZE_ALIGN_X) && Math::Equals(c->GetAlignedSizeX(), 0.0f, 0.0001f))
				{
					if (skipFlooring)
						c->SetSizeX(targetSize.x);
					else
						c->SetSizeX(Math::FloorToFloatEven(targetSize.x));
				}

				if (c->GetFlags().IsSet(WF_SIZE_ALIGN_Y) && Math::Equals(c->GetAlignedSizeY(), 0.0f, 0.0001f))
				{
					if (skipFlooring)
						c->SetSizeY(targetSize.y);
					else
						c->SetSizeY(Math::FloorToFloatEven(targetSize.y));
				}
			}
		}

		// if (!w->GetFlags().IsSet(WF_SKIP_FLOORING))
		//{
		//	w->SetSizeX(Math::FloorToFloatEven(w->GetSizeX()));
		//	w->SetSizeY(Math::FloorToFloatEven(w->GetSizeY()));
		// }

		for (auto* c : w->GetChildren())
			PassCalculateSize(c, delta);
	}

	void WidgetManager::PassTick(Widget* w, float delta)
	{
		if (w->GetFlags().IsSet(WF_POS_ALIGN_X) && w->GetParent())
			w->SetPosX(CalculateAlignedPosX(w));

		if (w->GetFlags().IsSet(WF_POS_ALIGN_Y) && w->GetParent())
			w->SetPosY(CalculateAlignedPosY(w));

		if (!w->GetFlags().IsSet(WF_TICK_AFTER_CHILDREN))
			w->Tick(delta);

		if (w->m_tickHook)
			w->m_tickHook(delta);

		// if (!w->GetFlags().IsSet(WF_SKIP_FLOORING))
		// {
		// 	w->SetPosX(Math::FloorToFloatEven(w->GetPosX()));
		// 	w->SetPosY(Math::FloorToFloatEven(w->GetPosY()));
		// }

		for (auto* c : w->GetChildren())
			PassTick(c, delta);

		if (w->GetFlags().IsSet(WF_TICK_AFTER_CHILDREN))
			w->Tick(delta);
	}

	ScrollArea* WidgetManager::FindScrollAreaAbove(Widget* w)
	{
		auto* parent = w->GetParent();
		if (parent)
		{

			if (parent->GetTID() == GetTypeID<ScrollArea>())
				return static_cast<ScrollArea*>(parent);

			return FindScrollAreaAbove(parent);
		}
		return nullptr;
	}

	void WidgetManager::GrabControls(Widget* widget)
	{
		m_controlOwner = widget;
	}

	void WidgetManager::ReleaseControls(Widget* widget)
	{
		if (m_controlOwner == widget)
			m_controlOwner = nullptr;
	}

	Widget* WidgetManager::GetControlsOwner()
	{
		return m_controlOwner;
	}

	bool WidgetManager::IsControlsOwner(Widget* w)
	{
		return m_controlOwner == w;
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
				current = current->m_children[0];
			else
			{
				while (current != nullptr && current->m_next == nullptr)
					current = current->m_parent;

				if (current != nullptr)
					current = current->m_next;
			}

			if (current && current->GetFlags().IsSet(WF_CONTROLLABLE) && !current->GetIsDisabled() && current->GetIsVisible())
				return current;
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
					current = current->m_children.back();
			}
			else
				current = current->m_parent;

			if (current && current->GetFlags().IsSet(WF_CONTROLLABLE) && !current->GetIsDisabled() && current->GetIsVisible())
				return current;

		} while (current != nullptr && current != start);

		return nullptr;
	}

	void WidgetManager::MoveControlsToPrev()
	{
		Widget* owner = m_controlOwner;
		if (!owner)
			return;

		Widget* prevControls = owner->GetPrevControls();
		Widget* previous	 = prevControls ? prevControls : FindPreviousSelectable(owner);
		if (previous)
		{
			GrabControls(previous);
			previous->OnGrabbedControls(false, owner);
		}
	}

	void WidgetManager::MoveControlsToNext()
	{
		Widget* owner = m_controlOwner;
		if (!owner)
			return;

		Widget* nextControls = owner->GetNextControls();
		Widget* next		 = nextControls ? nextControls : FindNextSelectable(owner);
		if (next)
		{
			GrabControls(next);
			next->OnGrabbedControls(true, owner);
		}
	}

} // namespace Lina
