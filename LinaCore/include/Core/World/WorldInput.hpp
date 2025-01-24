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

#include "Common/Math/Vector.hpp"

namespace LinaGX
{
	class Window;
	class Input;
	enum class InputAction;
} // namespace LinaGX

namespace Lina
{
	class Screen;
	class EntityWorld;

	class WorldInput
	{
	public:
		WorldInput() = delete;
		WorldInput(LinaGX::Input* inp, Screen* sc, EntityWorld* world) : m_lgxInput(inp), m_screen(sc), m_world(world){};

		bool	GetKey(int32 button);
		bool	GetKeyDown(int32 button);
		bool	GetKeyUp(int32 button);
		bool	GetMouseButton(int32 button);
		bool	GetMouseButtonDown(int button);
		bool	GetMouseButtonUp(int32 button);
		Vector2 GetMousePosition();
		Vector2 GetMousePositionRatio();
		Vector2 GetMouseDelta();
		Vector2 GetMouseDeltaRelative();
		float	GetMouseScroll();
		void	ConfineMouse();
		void	ReleaseMouse();

		void OnKey(uint32 keycode, int32 scancode, LinaGX::InputAction inputAction);
		void OnMouse(uint32 button, LinaGX::InputAction inputAction);
		void OnMouseWheel(float amt);
		void OnMouseMove(const LinaGX::LGXVector2&);

		inline LinaGX::Input* GetRawLGXInput()
		{
			return m_lgxInput;
		}

	private:
		Vector2		   m_lastMousePosition = Vector2::Zero;
		LinaGX::Input* m_lgxInput		   = nullptr;
		Screen*		   m_screen			   = nullptr;
		EntityWorld*   m_world			   = nullptr;
	};
} // namespace Lina
