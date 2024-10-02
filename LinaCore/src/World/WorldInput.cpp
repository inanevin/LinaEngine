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

#include "Core/World/WorldInput.hpp"
#include "Core/World/Screen.hpp"
#include "Common/Platform/LinaGXIncl.hpp"
#include "Common/Math/Math.hpp"

namespace Lina
{

	bool WorldInput::GetKey(int32 button)
	{
		if (!m_isActive)
			return false;
		return m_lgxInput->GetKey(button);
	}

	bool WorldInput::GetKeyDown(int32 button)
	{
		if (!m_isActive)
			return false;

		return m_lgxInput->GetKeyDown(button);
	}

	bool WorldInput::GetKeyUp(int32 button)
	{
		const bool isUp = m_lgxInput->GetKeyUp(button);

		if (m_isActive)
			return isUp;
		else
			return isUp ? true : false;
	}

	bool WorldInput::GetMouseButton(int32 button)
	{
		if (!m_isActive)
			return false;

		return m_lgxInput->GetMouseButton(button);
	}

	bool WorldInput::GetMouseButtonDown(int button)
	{
		if (!m_isActive)
			return false;

		return m_lgxInput->GetMouseButtonDown(button);
	}

	bool WorldInput::GetMouseButtonUp(int32 button)
	{
		const bool isUp = m_lgxInput->GetMouseButtonUp(button);

		if (m_isActive)
			return isUp;
		else
			return isUp ? true : false;
	}

	Vector2 WorldInput::GetMousePosition()
	{
		if (!m_isActive)
			return m_lastMousePosition;

		const Vector2 mpRelativeWindow = m_screen->GetOwnerWindow()->GetMousePosition();
		const Vector2 displayPos	   = m_screen->GetDisplayPos();
		const Vector2 displaySize	   = m_screen->GetDisplaySize();
		const Vector2 mousePos		   = Vector2(Math::Clamp(mpRelativeWindow.x - displayPos.x, 0.0f, displaySize.x), Math::Clamp(mpRelativeWindow.y - displayPos.y, 0.0f, displaySize.y));
		m_lastMousePosition			   = mousePos;
		return m_lastMousePosition;
	}

	Vector2 WorldInput::GetMousePositionRatio()
	{
		const Vector2 displaySize = m_screen->GetDisplaySize();
		const Vector2 mousePos	  = GetMousePosition();
		const Vector2 ratio		  = Vector2(mousePos.x / displaySize.x, mousePos.y / displaySize.y);
		return ratio;
	}

} // namespace Lina
