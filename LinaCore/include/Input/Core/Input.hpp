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

#ifndef Input_HPP
#define Input_HPP

#include "System/ISubsystem.hpp"
#include "CommonInput.hpp"
#include "Math/Vector.hpp"

namespace Lina
{
#define NUM_KEY_STATES	 380
#define NUM_MOUSE_STATES 8

	class Recti;

	class Input final : public ISubsystem
	{
	public:
		Input(ISystem* sys) : ISubsystem(sys, SubsystemType::Input){};
		~Input() = default;

		virtual void Initialize(const SystemInitializationInfo& initInfo) override;
		virtual void Shutdown() override;
		void		 PreTick();
		void		 Tick();

		void OnKey(void* windowPtr, uint32 key, int scanCode, InputAction action);
		void OnMouseWheel(void* windowPtr, int delta);
		void OnMouseButton(void* windowPtr, int button, InputAction action);
		void OnMouseMove(void* windowPtr, int xPosRel, int yPosRel);
		void OnActiveAppChanged(bool isActive);

		bool GetKey(int button);
		bool GetKeyDown(int button);
		bool GetKeyUp(int button);
		bool GetMouseButton(int button);
		bool GetMouseButtonDown(int button);
		bool GetMouseButtonUp(int button);
		bool GetMouseButtonDoubleClick(int button);
		bool GetMouseButtonClicked(int button);
		bool IsPointInRect(const Vector2i& point, const Recti& rect);
		void SetCursorMode(CursorMode mode);
		void SetMousePosition(const Vector2& v) const;

		inline Vector2i GetMousePositionAbs()
		{
			return m_currentMousePositionAbs;
		}

		inline Vector2i GetMouseDelta()
		{
			return m_mouseDelta;
		}

		inline Vector2i GetMouseDeltaRaw()
		{
			return m_mouseDeltaRaw;
		}

		inline Vector2i GetMouseScroll()
		{
			return m_mouseScroll;
		}

		inline CursorMode GetCursorMode()
		{
			return m_cursorMode;
		}

	private:
		int				   m_keyStatesDown[NUM_KEY_STATES];
		int				   m_keyStatesUp[NUM_KEY_STATES]	   = {0};
		int				   m_mouseStatesDown[NUM_MOUSE_STATES] = {0};
		int				   m_mouseStatesUp[NUM_MOUSE_STATES]   = {0};
		HashMap<int, bool> m_doubleClicks;
		HashMap<int, int>  m_singleClickStates;
		CursorMode		   m_cursorMode				 = CursorMode::Visible;
		Vector2i		   m_mouseScroll			 = Vector2i::Zero;
		Vector2i		   m_mouseScrollPrev		 = Vector2i::Zero;
		Vector2i		   m_mousePosTrackingClick	 = Vector2i::Zero;
		Vector2i		   m_mouseDelta				 = Vector2i::Zero;
		Vector2i		   m_mouseDeltaRaw			 = Vector2i::Zero;
		Vector2i		   m_mouseDeltaRawPrev		 = Vector2i::Zero;
		Vector2i		   m_previousMousePosition	 = Vector2i::Zero;
		Vector2i		   m_currentMousePositionAbs = Vector2i::Zero;
		bool			   m_appActive			 = false;
		void*			   m_lastFocusedWindowHandle = nullptr;
		bool			   m_currentStates[256]		 = {0};
		bool			   m_previousStates[256]	 = {0};
	};
} // namespace Lina

#endif
