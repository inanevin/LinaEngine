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

#include "Common/Data/HashMap.hpp"
#include "Common/Data/IDList.hpp"
#include "Core/GUI/CommonGUI.hpp"
#include "Core/GUI/Theme.hpp"
#include "WidgetCache.hpp"
#include <LinaGX/Core/WindowListener.hpp>

namespace LinaGX
{
	class Window;
	enum class InputAction;
	enum class CursorType;
} // namespace LinaGX

namespace LinaVG
{
	class Drawer;
}

namespace Lina
{
	class Widget;
	class Popup;
	class System;
	class ResourceManagerV2;
	class Font;
	class ScrollArea;

	class WidgetManager : public LinaGX::WindowListener
	{
	public:
		WidgetManager()			 = default;
		virtual ~WidgetManager() = default;

		void	Initialize(ResourceManagerV2* resourceManager, LinaGX::Window* window, LinaVG::Drawer* drawer);
		void	Draw();
		void	InitializeWidget(Widget* w);
		void	PreTick();
		void	Tick(float delta, const Vector2ui& size);
		void	Shutdown();
		void	DebugDraw(Widget* w);
		void	SetClip(const Rect& r, const TBLR& margin);
		void	UnsetClip();
		void	AddToKillList(Widget* w);
		void	AddToForeground(Widget* widget);
		void	RemoveFromForeground(Widget* widget);
		void	Deallocate(Widget* widget);
		void	GrabControls(Widget* widget);
		void	ReleaseControls(Widget* widget);
		Widget* GetControlsOwner();
		bool	IsControlsOwner(Widget* w);
		void	MoveControlsToNext();
		void	MoveControlsToPrev();
		Widget* FindNextSelectable(Widget* start);
		Widget* FindPreviousSelectable(Widget* start);
		bool	PassKey(Widget* widget, uint32 keycode, int32 scancode, LinaGX::InputAction inputAction);
		bool	PassMouse(Widget* widget, uint32 button, LinaGX::InputAction inputAction);
		bool	PassMouseWheel(Widget* widget, float amt);
		Widget* LockForeground();
		void	UnlockForeground();

		inline Widget* GetRoot() const
		{
			return m_rootWidget;
		}

		inline Widget* GetForegroundRoot()
		{
			return m_foregroundRoot;
		}

		inline Rect* GetClipStackTop()
		{
			if (m_clipStack.empty())
				return nullptr;
			else
				return &m_clipStack[m_clipStack.size() - 1].rect;
		}

		inline Font* GetDefaultFont()
		{
			return m_defaultFont;
		}

		inline Widget* GetLastControlsManager() const
		{
			return m_lastControlsManager;
		}

		Widget* Allocate(TypeID tid, const String& debugName = "Widget");

		template <typename T> T* Allocate(const String& debugName = "Widget")
		{
			const TypeID tid = GetTypeID<T>();
			return static_cast<T*>(Allocate(tid, debugName));
		}

		inline LinaVG::Drawer* GetLVG() const
		{
			return m_lvg;
		}

		inline ResourceManagerV2* GetResourceManagerV2() const
		{
			return m_resourceManagerV2;
		}

		inline bool GetIsMouseHoveringWindow() const
		{
			return m_isMouseHoveringWindow;
		}

	protected:
		virtual void OnWindowKey(LinaGX::Window* window, uint32 keycode, int32 scancode, LinaGX::InputAction inputAction) override;
		virtual void OnWindowMouse(LinaGX::Window* window, uint32 button, LinaGX::InputAction inputAction) override;
		virtual void OnWindowMouseWheel(LinaGX::Window* window, float amt) override;
		virtual void OnWindowMouseMove(LinaGX::Window* window, const LinaGX::LGXVector2&) override;
		virtual void OnWindowFocus(LinaGX::Window* window, bool gainedFocus) override;
		virtual void OnWindowHoverBegin(LinaGX::Window* window) override;
		virtual void OnWindowHoverEnd(LinaGX::Window* window) override;

	private:
		LinaGX::CursorType FindCursorType(Widget* start);

		bool		PassMousePos(Widget* widget, const Vector2& pos);
		void		PassCalculateSize(Widget* w, float delta);
		void		PassPreTick(Widget* w);
		void		PassTick(Widget* w, float delta);
		ScrollArea* FindScrollAreaAbove(Widget* w);

	private:
		friend class Widget;

	private:
		LinaGX::Window*					  m_window = nullptr;
		LinaVG::Drawer*					  m_lvg	   = nullptr;
		Vector<Widget*>					  m_controlsOwners;
		Widget*							  m_controlOwner	  = nullptr;
		Widget*							  m_rootWidget		  = nullptr;
		Widget*							  m_foregroundRoot	  = nullptr;
		Widget*							  m_deepestHovered	  = nullptr;
		ResourceManagerV2*				  m_resourceManagerV2 = nullptr;
		Vector<ClipData>				  m_clipStack;
		float							  m_debugDrawYOffset	= 0.0f;
		Font*							  m_defaultFont			= nullptr;
		HashSet<Widget*>				  m_killList			= {};
		Widget*							  m_lastControlsManager = nullptr;
		HashMap<TypeID, WidgetCacheBase*> m_widgetCaches;
		Widget*							  m_foregroundLock		  = nullptr;
		bool							  m_isMouseHoveringWindow = true;
	};

} // namespace Lina
