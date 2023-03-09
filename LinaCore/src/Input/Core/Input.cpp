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

#include "Input/Core/Input.hpp"
#include "Math/Rect.hpp"
#ifdef LINA_PLATFORM_WINDOWS
#include "Platform/Win32/Win32WindowsInclude.hpp"
#endif

namespace Lina
{
	void Input::Initialize(const SystemInitializationInfo& initInfo)
	{
		LINA_TRACE("[Input] -> Initialization.");
	}
	void Input::Shutdown()
	{
		LINA_TRACE("[Input] -> Shutdown.");
	}

	void Input::OnKey(void* windowPtr, uint32 key, int scanCode, InputAction action)
	{
		m_currentStates[key] = action == InputAction::Pressed ? 1 : 0;
	}

	void Input::OnMouseWheel(void* windowPtr, int delta)
	{
		m_mouseScrollPrev.x = delta;
		m_mouseScrollPrev.y = delta;
	}

	void Input::OnMouseButton(void* windowPtr, int button, InputAction action)
	{
		m_currentStates[button] = action == InputAction::Pressed ? 1 : 0;

		if (action == InputAction::Repeated)
			m_doubleClicks[button] = true;
	}

	void Input::OnMouseMove(void* windowPtr, int xPosRel, int yPosRel)
	{
		m_mouseDeltaRawPrev.x = xPosRel;
		m_mouseDeltaRawPrev.y = yPosRel;
	}

	void Input::OnActiveAppChanged(bool isActive)
	{
		m_windowActive = isActive;
	}

	bool Input::GetKey(int keycode)
	{
		if (!m_windowActive)
			return false;
		return m_currentStates[keycode] == 1;
	}

	bool Input::GetKeyDown(int keyCode)
	{
		if (!m_windowActive)
			return false;

		return m_currentStates[keyCode] == 1 && m_previousStates[keyCode] == 0;
	}
	bool Input::GetKeyUp(int keyCode)
	{
		if (!m_windowActive)
			return false;

		return m_currentStates[keyCode] == 0 && m_previousStates[keyCode] == 1;
	}
	bool Input::GetMouseButton(int button)
	{
		if (!m_windowActive)
			return false;

		return m_currentStates[button] == 1;
	}

	bool Input::GetMouseButtonDown(int button)
	{
		if (!m_windowActive)
			return false;

		return m_currentStates[button] == 1 && m_previousStates[button] == 0;
	}
	bool Input::GetMouseButtonUp(int button)
	{
		if (!m_windowActive)
			return false;

		return m_currentStates[button] == 0 && m_previousStates[button] == 1;
	}

	bool Input::GetMouseButtonDoubleClick(int button)
	{
		return m_doubleClicks[button];
	}

	bool Input::GetMouseButtonClicked(int button)
	{
		bool result = false;

		if (GetMouseButtonDown(button))
			m_mousePosTrackingClick = GetMousePositionAbs();

		if (GetMouseButtonUp(button) && GetMousePositionAbs().Equals(m_mousePosTrackingClick, 5))
			result = true;

		return result;
	}

	bool Input::IsPointInRect(const Vector2i& point, const Recti& rect)
	{
#ifdef LINA_PLATFORM_WINDOWS
		RECT r;
		r.left	 = rect.pos.x;
		r.top	 = rect.pos.y;
		r.right	 = rect.pos.x + rect.size.x;
		r.bottom = rect.pos.y + rect.size.y;
		POINT pt;
		pt.x = point.x;
		pt.y = point.y;
		return PtInRect(&r, pt);
#else
		return false;
#endif
	}

	void Input::SetCursorMode(CursorMode mode)
	{
		if (mode == m_cursorMode)
			return;

		m_cursorMode = mode;

		switch (mode)
		{
		case CursorMode::Visible:
#ifdef LINA_PLATFORM_WINDOWS
			ShowCursor(true);
#else
			glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
#endif
			break;

		case CursorMode::Hidden:
#ifdef LINA_PLATFORM_WINDOWS
			ShowCursor(false);
#else
			glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
#endif
			break;

		case CursorMode::Disabled:
#ifdef LINA_PLATFORM_WINDOWS
			ShowCursor(false);
#else
			glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
#endif
			break;
		}
	}

	void Input::SetMousePosition(const Vector2& v) const
	{
#ifdef LINA_PLATFORM_WINDOWS
		SetCursorPos(static_cast<int>(v.x), static_cast<int>(v.y));
#else
		glfwSetCursorPos(glfwWindow, v.x, v.y);
#endif
	}

	void Input::PreTick()
	{
		for (int i = 0; i < 256; i++)
			m_previousStates[i] = m_currentStates[i];
	}

	void Input::Tick(float dt)
	{
		m_mouseDeltaRaw		= m_mouseDeltaRawPrev;
		m_mouseScroll		= m_mouseScrollPrev;
		m_mouseScrollPrev	= Vector2i::Zero;
		m_mouseDeltaRawPrev = Vector2i::Zero;
		m_doubleClicks.clear();
		m_singleClickStates.clear();

#ifdef LINA_PLATFORM_WINDOWS
		POINT point;
		GetCursorPos(&point);
#else
		glfwGetCursorPos(glfwWindow, &xpos, &ypos);
#endif
		m_previousMousePosition	  = m_currentMousePositionAbs;
		m_currentMousePositionAbs = Vector2i(point.x, point.y);
		m_mouseDelta			  = m_currentMousePositionAbs - m_previousMousePosition;

#ifdef LINA_PLATFORM_WINDOWS

#else
		glfwPollEvents();
#endif
	}

} // namespace Lina
