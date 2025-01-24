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
#include "Core/GUI/Widgets/Primitives/Icon.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Graphics/Resource/Font.hpp"
#include "Core/CommonCore.hpp"
#include "Common/Platform/LinaGXIncl.hpp"
#include "Common/Data/CommonData.hpp"
#include "Common/Tween/Tween.hpp"

#include "Common/Platform/LinaGXIncl.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Common/Math/Math.hpp"
#include "LinaGX/Core/InputMappings.hpp"
#include "Common/Profiling/Profiler.hpp"

namespace Lina
{

#define BASE_RESOLUTION_X 3840
#define BASE_RESOLUTION_Y 2160

	void WidgetManager::Initialize(ResourceManagerV2* resourceManager, LinaGX::Window* window, LinaVG::Drawer* drawer)
	{
		m_resourceManagerV2 = resourceManager;
		m_lvg				= drawer;
		m_window			= window;

		if (!m_rootWidget)
		{
			m_rootWidget								 = Allocate<Widget>();
			m_rootWidget->GetWidgetProps().debugName	 = "Root";
			m_foregroundRoot							 = Allocate<Widget>();
			m_foregroundRoot->GetWidgetProps().debugName = "ForegroundRoot";
		}

		m_defaultFont = resourceManager->GetResource<Font>(Theme::GetDef().defaultFont);
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

		m_cursorOverride = static_cast<uint8>(LinaGX::CursorType::Default);

		/*
		if (m_window->HasFocus())
		{
			if (m_foregroundRoot->GetChildren().empty())
				m_window->SetCursorType(FindCursorType(m_rootWidget));
			else
				m_window->SetCursorType(FindCursorType(m_foregroundRoot));
		}
		*/

		PassPreTick(m_foregroundRoot);
		PassPreTick(m_rootWidget);

		if (m_window->HasFocus())
			m_window->SetCursorType(static_cast<LinaGX::CursorType>(m_cursorOverride));
	}

	void WidgetManager::Tick(float delta, const Vector2ui& size)
	{
		m_size = size;

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

	void WidgetManager::AddToForeground(Widget* w)
	{
		w->SetDrawOrder(FOREGROUND_DRAW_ORDER + static_cast<int32>(m_foregroundRoot->GetChildren().size()));
		w->GetFlags().Set(WF_CONTROLS_DRAW_ORDER);
		m_foregroundRoot->AddChild(w);
	}

	void WidgetManager::RemoveFromForeground(Widget* w)
	{
		w->GetFlags().Remove(WF_CONTROLS_DRAW_ORDER);
		m_foregroundRoot->RemoveChild(w);
	}

	Widget* WidgetManager::LockForeground()
	{
		if (m_foregroundLock)
			return m_foregroundLock;

		Widget* lock = Allocate<Widget>();
		lock->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y | WF_CONTROLS_DRAW_ORDER | WF_FOREGROUND_BLOCKER);
		lock->SetAlignedPos(Vector2::Zero);
		lock->SetAlignedSize(Vector2::One);
		lock->SetDrawOrder(FOREGROUND_DRAW_ORDER);
		lock->GetWidgetProps().drawBackground		   = true;
		lock->GetWidgetProps().outlineThickness		   = 0.0f;
		lock->GetWidgetProps().rounding				   = 0.0f;
		lock->GetWidgetProps().colorBackground		   = Theme::GetDef().black;
		lock->GetWidgetProps().colorBackground.start.w = lock->GetWidgetProps().colorBackground.end.w = 0.75f;
		m_foregroundRoot->AddChild(lock);
		m_foregroundLock = lock;

		return lock;
	}

	void WidgetManager::UnlockForeground()
	{
		if (m_foregroundLock == nullptr)
			return;

		m_foregroundRoot->RemoveChild(m_foregroundLock);
		Deallocate(m_foregroundLock);
		m_foregroundLock = nullptr;
	}

	void WidgetManager::SetDisabledRecursively(Widget* root, bool disabled)
	{
		root->GetFlags().Set(WF_DISABLED, disabled);
		for (Widget* w : root->m_children)
			SetDisabledRecursively(w, disabled);
	}

	void WidgetManager::Draw()
	{
		PassDraw(m_rootWidget);

		m_rootWidget->Draw();

		if (!m_foregroundRoot->GetChildren().empty())
			PassDraw(m_foregroundRoot);

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

		widget->PreDestruct();

		for (auto* c : widget->m_children)
			Deallocate(c);

		for (Widget* w : widget->m_addChildRequests)
			Deallocate(w);

		const TypeID tid = widget->m_tid;
		widget->Destruct();

		WidgetCachePair out = {};
		if (!FindCache(tid, out))
		{
			LINA_ASSERT(false, "");
		}

		WidgetCacheBase* cache = out.cache;
		cache->Destroy(widget);
	}

	void WidgetManager::Shutdown()
	{
		Deallocate(m_rootWidget);
		Deallocate(m_foregroundRoot);
		m_rootWidget	 = nullptr;
		m_foregroundRoot = nullptr;

		for (const WidgetCachePair& pair : m_widgetCaches)
			delete pair.cache;

		m_widgetCaches.clear();
	}

	Widget* WidgetManager::Allocate(TypeID tid, const String& debugName)
	{
		WidgetCachePair	 out	   = {};
		WidgetCacheBase* cacheBase = nullptr;
		if (!FindCache(tid, out))
		{
			MetaType* type = ReflectionSystem::Get().Resolve(tid);
			cacheBase	   = static_cast<WidgetCacheBase*>(type->GetFunction<void*()>("CreateWidgetCache"_hs)());
			m_widgetCaches.push_back({tid, cacheBase});
		}
		else
			cacheBase = out.cache;

		uint32	cacheIndex = 0;
		Widget* t		   = static_cast<Widget*>(cacheBase->Create(cacheIndex));
		t->m_cacheIndex	   = cacheIndex;
		LINA_ASSERT(t != nullptr, "");
		t->GetWidgetProps().debugName = debugName;
		t->m_lgxWindow				  = m_window;
		t->m_manager				  = this;
		t->m_resourceManager		  = m_resourceManagerV2;
		t->m_tid					  = tid;
		t->m_lvg					  = m_lvg;
		t->Construct();
		return t;
	}

	void WidgetManager::OnKey(uint32 keycode, int32 scancode, LinaGX::InputAction inputAction)
	{

		if (keycode == LINAGX_KEY_ESCAPE && inputAction != LinaGX::InputAction::Released)
		{
			Vector<Widget*> removeList;
			for (auto* c : m_foregroundRoot->GetChildren())
			{
				if (!c->GetFlags().IsSet(WF_FOREGROUND_BLOCKER) && !c->GetFlags().IsSet(WF_TOOLTIP))
					removeList.push_back(c);
			}

			for (auto* w : removeList)
			{
				RemoveFromForeground(w);
				Deallocate(w);
			}

			if (!removeList.empty())
				return;
		}

		const bool foregroundExists		= !m_foregroundRoot->GetChildren().empty();
		const bool controlsInForeground = m_foregroundRoot->IsWidgetInHierarchy(GetControlsOwner());

		if (keycode == LINAGX_KEY_TAB && inputAction != LinaGX::InputAction::Released && GetControlsOwner() != nullptr)
		{
			if (!foregroundExists || (foregroundExists && controlsInForeground))
			{
				if (m_window->GetInput()->GetKey(LINAGX_KEY_LSHIFT))
					MoveControlsToPrev();
				else
					MoveControlsToNext();
				return;
			}
		}

		if (PassKey(m_foregroundRoot, keycode, scancode, inputAction))
			return;

		PassKey(m_rootWidget, keycode, scancode, inputAction);
	}

	void WidgetManager::OnMouse(uint32 button, LinaGX::InputAction inputAction)
	{
		Vector<Widget*> sortedChildren = m_foregroundRoot->GetChildren();
		linatl::sort(sortedChildren.begin(), sortedChildren.end(), [](Widget* w, Widget* other) -> bool { return w->GetDrawOrder() > other->GetDrawOrder(); });
		for (Widget* child : sortedChildren)
		{
			if (PassMouse(child, button, inputAction))
				return;
		}

		if (button == LINAGX_MOUSE_0 && inputAction == LinaGX::InputAction::Pressed && m_controlOwner != nullptr && !m_controlOwner->GetIsHovered())
			ReleaseControls(m_controlOwner);

		Widget* co = GetControlsOwner();

		//if (co)
		//{
		//	if (PassMouse(co, button, inputAction))
		//		return;
		//}

		PassMouse(m_rootWidget, button, inputAction);
	}

	void WidgetManager::OnMouseWheel(float amt)
	{
		if (PassMouseWheel(m_foregroundRoot, amt))
			return;
		PassMouseWheel(m_rootWidget, amt);
	}

	void WidgetManager::OnMouseMove(const LinaGX::LGXVector2& pos)
	{
		if (PassMousePos(m_foregroundRoot, pos))
			return;
		PassMousePos(m_rootWidget, Vector2(pos.x * m_window->GetDPIScale(), pos.y * m_window->GetDPIScale()));
	}

	void WidgetManager::OnFocus(bool gainedFocus)
	{
	}

	void WidgetManager::OnHoverBegin()
	{
		m_isMouseHoveringWindow = true;
	}

	void WidgetManager::OnHoverEnd()
	{
		m_isMouseHoveringWindow = false;
	}

	float WidgetManager::GetScalingFactor() const
	{
		const Vector2 sz	 = m_window->GetSize();
		const Vector2 mSize	 = m_window->GetMonitorSize();
		const float	  factor = Math::Remap(mSize.y * m_window->GetDPIScale(), 1080.0f, 2160.0f, 0.8f, 1.0f);
		return factor * (Math::Equals(m_window->GetDPIScale(), 1.0f, 0.1f) ? 1.0f : m_window->GetDPIScale() * 0.75f);
	}

	Vector2 WidgetManager::GetMousePosition() const
	{
		const Vector2 mp = m_window->GetMousePosition();
		return mp * m_window->GetDPIScale();
	}

	void WidgetManager::DebugDraw(Widget* w)
	{
		/*
		const bool drawRects = (m_window->GetInput()->GetKey(LINAGX_KEY_K));

		if (!drawRects)
			return;

		if (w->GetFlags().IsSet(WF_HIDE))
			return;

		for (auto* c : w->m_children)
			DebugDraw(c);

		LinaVG::StyleOptions opts;
		opts.isFilled  = false;
		opts.thickness = 2.0f;

		const Rect			rect	= w->GetRect();
		const Vector2&		mp		= GetMousePosition();
		const float			scaling = GetScalingFactor();
		auto*				lvgFont = m_resourceManagerV2->GetResource<Font>(Theme::GetDef().defaultFont)->GetFont(scaling);
		LinaVG::TextOptions textOpts;
		textOpts.font = lvgFont;

		if (drawRects)
			m_lvg->DrawRect(rect.pos.AsLVG(), (rect.pos + rect.size).AsLVG(), opts, 0.0f, 1000);

		if (w->m_isHovered)
		{
			const Vector2 sz = m_lvg->CalculateTextSize(w->GetWidgetProps().debugName.c_str(), textOpts);
			m_lvg->DrawTextDefault(w->GetWidgetProps().debugName.c_str(), (mp + Vector2(15, 15 + m_debugDrawYOffset)).AsLVG(), textOpts, 0.0f, DEBUG_DRAW_ORDER);

			const String  rectStr = "Pos: (" + UtilStr::FloatToString(w->GetPos().x, 1) + ", " + UtilStr::FloatToString(w->GetPos().y, 1) + ") Size: (" + UtilStr::FloatToString(w->GetSize().x, 1) + ", " + UtilStr::FloatToString(w->GetSize().y, 1) + ")";
			const Vector2 sz2	  = m_lvg->CalculateTextSize(rectStr.c_str(), textOpts);
			m_lvg->DrawTextDefault(rectStr.c_str(), (mp + Vector2(15 + sz.x + 15, 15 + m_debugDrawYOffset)).AsLVG(), textOpts, 0.0f, DEBUG_DRAW_ORDER);

			const String drawOrder = "DO: " + TO_STRING(w->GetDrawOrder());
			m_lvg->DrawTextDefault(drawOrder.c_str(), (mp + Vector2(15 + sz.x + 15 + sz2.x + 15, 15 + m_debugDrawYOffset)).AsLVG(), textOpts, 0.0f, DEBUG_DRAW_ORDER);
			m_debugDrawYOffset += lvgFont->size * 1.5f;
		}

		if (drawRects)
			w->DebugDraw(DEBUG_DRAW_ORDER);
			*/
	}

	void WidgetManager::SetClip(const Rect& r)
	{
		const ClipData cd = {
			.rect = r,
		};

		m_clipStack.push_back(cd);

		LinaVG::Vec4i rect = {};
		rect.x			   = static_cast<int32>(r.pos.x);
		rect.y			   = static_cast<int32>(r.pos.y);

		if (r.size.x < 0.0f)
			rect.z = 0;
		else
			rect.z = static_cast<uint32>(r.size.x);

		if (r.size.y < 0.0f)
			rect.w = 0;
		else
			rect.w = static_cast<uint32>(r.size.y);

		m_lvg->SetClipRect(rect);
	}

	void WidgetManager::UnsetClip()
	{
		m_clipStack.pop_back();

		if (m_clipStack.empty())
		{
			m_lvg->SetClipRect({0, 0, 0, 0});
		}
		else
		{
			const ClipData& cd	 = m_clipStack[m_clipStack.size() - 1];
			LinaVG::Vec4i	rect = {};
			rect.x				 = static_cast<int32>(cd.rect.pos.x);
			rect.y				 = static_cast<int32>(cd.rect.pos.y);

			if (cd.rect.size.x < 0.0f)
				rect.z = 0;
			else
				rect.z = static_cast<uint32>(cd.rect.size.x);

			if (cd.rect.size.y < 0.0f)
				rect.w = 0;
			else
				rect.w = static_cast<uint32>(cd.rect.size.y);

			m_lvg->SetClipRect(rect);
		}
	}

	void WidgetManager::AddToKillList(Widget* w)
	{
		auto it = linatl::find_if(m_killList.begin(), m_killList.end(), [w](Widget* wid) -> bool { return w == wid; });
		if (it != m_killList.end())
			return;
		m_killList.push_back(w);
	}

	float WidgetManager::CalculateAlignedPosX(Widget* w)
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

	float WidgetManager::CalculateAlignedPosY(Widget* w)
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

	bool WidgetManager::PassKey(Widget* widget, uint32 keycode, int32 scancode, LinaGX::InputAction inputAction)
	{
		if (widget->GetFlags().IsSet(WF_HIDE))
			return false;

		if (!widget->GetFlags().IsSet(WF_DISABLED) && widget->OnKey(keycode, scancode, inputAction) && !widget->GetFlags().IsSet(WF_KEY_PASSTHRU))
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
		if (widget->GetFlags().IsSet(WF_HIDE))
			return false;

		if (widget->GetFlags().IsSet(WF_DISABLED))
			return false;

		const bool mouseResult = widget->OnMouse(button, inputAction);
		if (mouseResult && !widget->GetFlags().IsSet(WF_MOUSE_PASSTHRU))
			return true;

		for (auto* c : widget->GetChildren())
		{
			if (PassMouse(c, button, inputAction))
				return true;
		}

		return mouseResult;
	}

	bool WidgetManager::PassMouseWheel(Widget* widget, float amt)
	{
		if (widget->GetFlags().IsSet(WF_HIDE))
			return false;

		if (!widget->GetFlags().IsSet(WF_DISABLED) && widget->OnMouseWheel(amt) && !widget->GetFlags().IsSet(WF_MOUSE_PASSTHRU))
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
		if (widget->GetFlags().IsSet(WF_HIDE))
			return false;

		if (!widget->GetFlags().IsSet(WF_DISABLED) && widget->OnMousePos(pos) && !widget->GetFlags().IsSet(WF_MOUSE_PASSTHRU))
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
		for (auto cb : w->m_preTickHooks)
			cb();

		if (w->GetFlags().IsSet(WF_HIDE))
			return;

		if (!w->GetFlags().IsSet(WF_CONTROLS_DRAW_ORDER) && w->GetParent())
			w->SetDrawOrder(w->GetParent()->GetDrawOrder() + w->GetWidgetProps().drawOrderIncrement);

		w->SetIsHovered();
		w->CheckCustomTooltip();
		w->PreTick();

		const Vector2 mp = Vector2(Math::FloorToFloat(GetMousePosition().x), Math::FloorToFloat(GetMousePosition().y));

		if (w->m_buildCustomTooltip)
		{
			if (!w->GetIsHovered() && w->m_customTooltip != nullptr)
			{
				RemoveFromForeground(w->m_customTooltip);
				Deallocate(w->m_customTooltip);
				w->m_customTooltip = nullptr;
			}
			else if (w->GetIsHovered() && w->m_customTooltip == nullptr)
			{
				w->m_customTooltip = w->m_buildCustomTooltip(w->m_customTooltipUserData);
				w->m_customTooltip->GetFlags().Set(WF_TOOLTIP);
				w->m_customTooltip->SetPos(mp + Vector2(10, 10));
				AddToForeground(w->m_customTooltip);
			}
		}

		for (auto* c : w->GetChildren())
			PassPreTick(c);

		for (Widget* r : w->m_addChildRequests)
			w->AddChild(r);

		w->m_addChildRequests.clear();
	}

	void WidgetManager::PassCalculateSize(Widget* w, float delta)
	{
		if (w->GetFlags().IsSet(WF_HIDE))
			return;

		if (w->GetFlags().IsSet(WF_SIZE_AFTER_CHILDREN))
		{
			for (auto* c : w->GetChildren())
				PassCalculateSize(c, delta);
		}

		Vector<Widget*> expandingChildren;
		Vector2			totalNonExpandingSize = Vector2::Zero;

		if (w->GetFlags().IsSet(WF_USE_FIXED_SIZE_X))
			w->SetSizeX(w->GetFixedSizeX() * GetScalingFactor());

		if (w->GetFlags().IsSet(WF_USE_FIXED_SIZE_Y))
			w->SetSizeY(w->GetFixedSizeY() * GetScalingFactor());

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
				c->SetSizeX(c->GetSizeY() * c->GetAlignedSizeX());

			if (c->GetFlags().IsSet(WF_SIZE_Y_COPY_X))
				c->SetSizeY(c->GetSizeX() * c->GetAlignedSizeY());

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

		if (w->GetFlags().IsSet(WF_SIZE_X_TOTAL_CHILDREN))
		{
			float total = 0.0f;
			int32 idx	= 0;
			for (auto* c : w->GetChildren())
			{
				if (c->GetFlags().IsSet(WF_SIZE_ALIGN_X))
					continue;

				if (idx != 0)
					total += w->GetWidgetProps().childPadding * GetScalingFactor();

				total += c->GetSizeX();
				idx++;
			}

			total *= w->GetAlignedSizeX();
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
					total += w->GetWidgetProps().childPadding * GetScalingFactor();

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

		if (w->GetWidgetProps().useSizeTween)
		{
			Tween& tw = w->GetWidgetProps().sizeTween;
			tw.Tick(delta);

			w->SetSizeX(w->GetSizeX() * tw.GetValue());
			w->SetSizeY(w->GetSizeY() * tw.GetValue());
		}
		w->CalculateSize(delta);

		if (!expandingChildren.empty())
		{
			const Vector2 start						 = w->GetStartFromMargins();
			const Vector2 end						 = w->GetEndFromMargins();
			const Vector2 size						 = end - start;
			const Vector2 totalAvailableSpace		 = size - totalNonExpandingSize;
			const float	  totalPad					 = w->GetWidgetProps().childPadding * GetScalingFactor() * static_cast<float>(w->GetChildren().size() - 1);
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

		if (!w->GetFlags().IsSet(WF_SIZE_AFTER_CHILDREN))
		{
			for (auto* c : w->GetChildren())
				PassCalculateSize(c, delta);
		}
	}

	void WidgetManager::PassTick(Widget* w, float delta)
	{
		if (w->GetFlags().IsSet(WF_HIDE))
			return;

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

	void WidgetManager::PassDraw(Widget* w)
	{
		if (w->ShouldSkipDrawOutsideWindow())
			return;

		if (w->GetSizeX() < 0.5f || w->GetSizeY() < 0.5f)
			return;

		if (w->GetFlags().IsSet(WF_HIDE))
			return;

		w->DrawBackground();
		w->DrawDropshadow();
		w->Draw();
		w->DrawBorders();
		w->DrawTooltip();

		w->CheckClipChildren();

		for (Widget* child : w->GetChildren())
			PassDraw(child);

		w->CheckClipChildrenEnd();
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

			if (current && current->GetFlags().IsSet(WF_CONTROLLABLE) && !current->GetFlags().IsSet(WF_DISABLED) && !current->GetFlags().IsSet(WF_HIDE))
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

			if (current && current->GetFlags().IsSet(WF_CONTROLLABLE) && !current->GetFlags().IsSet(WF_DISABLED) && !current->GetFlags().IsSet(WF_HIDE))
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
			owner->OnLostControls(previous);
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
			owner->OnLostControls(next);
			next->OnGrabbedControls(true, owner);
		}
	}

} // namespace Lina
