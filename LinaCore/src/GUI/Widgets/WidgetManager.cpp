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
#include "Common/Data/CommonData.hpp"
#include "Common/System/System.hpp"
#include "Common/Platform/LinaGXIncl.hpp"
#include "LinaGX/Core/InputMappings.hpp"

#include "Core/Resources/ResourceManager.hpp"
#include "Core/Graphics/Resource/Font.hpp"
#include "Core/CommonCore.hpp"
#include "Common/Platform/LinaVGIncl.hpp"

namespace Lina
{
	void WidgetManager::Initialize(System* system, LinaGX::Window* window)
	{
		m_window = window;
		m_system = system;
		m_window->AddListener(this);
		m_rootWidget = Allocate<Widget>();
		m_rootWidget->SetDebugName("Root");
		m_resourceManager = m_system->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager);
	}

	void WidgetManager::Tick(float delta, const Vector2ui& size)
	{
		m_rootWidget->SetPos(Vector2::Zero);
		m_rootWidget->SetSize(Vector2(static_cast<float>(size.x), static_cast<float>(size.y)));
		m_rootWidget->Tick(delta);
	}

	Popup* WidgetManager::AddPopup()
	{
		Popup* popup = m_rootWidget->Allocate<Popup>();
		popup->SetDrawOrder(POPUP_DRAW_ORDER);
		m_rootWidget->AddChild(popup);
		return popup;
	}

	void WidgetManager::RemovePopup(Popup* popup)
	{
		m_rootWidget->RemoveChild(popup);
		popup->Destroy();
	}

	void WidgetManager::RenderSync()
	{
		m_rootWidget->RenderSync();
	}
	void WidgetManager::Draw(int32 threadIndex)
	{
		m_rootWidget->Draw(threadIndex);

		// DebugDraw(threadIndex, m_rootWidget);
	}

	void WidgetManager::Deallocate(Widget* widget)
	{
		const TypeID tid = widget->m_tid;
		m_allocators[tid]->Free(widget);
	}

	void WidgetManager::Shutdown()
	{
		m_rootWidget->Destroy();
		Deallocate(m_rootWidget);
		m_rootWidget = nullptr;
		m_window->RemoveListener(this);

		linatl::for_each(m_allocators.begin(), m_allocators.end(), [](auto& pair) -> void { delete pair.second; });
		m_allocators.clear();
	}

	void WidgetManager::OnWindowKey(uint32 keycode, int32 scancode, LinaGX::InputAction inputAction)
	{
		// m_rootWidget->OnKey(keycode, scancode, inputAction);
	}

	void WidgetManager::OnWindowMouse(uint32 button, LinaGX::InputAction inputAction)
	{
		// m_rootWidget->OnMouse(button, inputAction);
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

	void WidgetManager::FindHoveredRecursive(const Vector2& pos, Widget* w)
	{
	}

	void WidgetManager::ClearHoveredRecursive(Widget* w)
	{
	}

	void WidgetManager::ClearHoverStatus(Widget* w)
	{
		if (w == nullptr)
			return;

		w->m_isHovered = false;
	}

	void WidgetManager::DebugDraw(int32 threadIndex, Widget* w)
	{
		LinaVG::StyleOptions opts;
		opts.isFilled = false;

		const Rect	   rect = w->GetRect();
		const Vector2& mp	= m_window->GetMousePosition();

		if (w->GetAlignPoint() == AlignPoint::TopLeft)
		{
			LinaVG::DrawRect(threadIndex, rect.pos.AsLVG(), (rect.pos + rect.size).AsLVG(), opts, 0.0f, 1000.0f);

			if (w->m_isHovered)
			{
				LinaVG::TextOptions opts;
				opts.font = m_resourceManager->GetResource<Font>(DEFAULT_FONT_SID)->GetLinaVGFont(m_window->GetDPIScale());
				LinaVG::DrawTextNormal(threadIndex, w->GetDebugName().c_str(), (mp + Vector2(10, 10)).AsLVG(), opts);
			}
		}
		else
		{
			LinaVG::DrawRect(threadIndex, (rect.pos - rect.size * 0.5f).AsLVG(), (rect.pos + rect.size * 0.5f).AsLVG(), opts, 0.0f, 1000.0f);

			if (w->m_isHovered)
			{
				LinaVG::TextOptions opts;
				opts.font = m_resourceManager->GetResource<Font>(DEFAULT_FONT_SID)->GetLinaVGFont(m_window->GetDPIScale());
				LinaVG::DrawTextNormal(threadIndex, w->GetDebugName().c_str(), (mp + Vector2(10, 10)).AsLVG(), opts);
			}
		}

		for (auto* c : w->m_children)
			DebugDraw(threadIndex, c);
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

} // namespace Lina
