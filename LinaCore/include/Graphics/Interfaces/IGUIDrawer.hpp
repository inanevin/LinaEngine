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

#ifndef IGUIDrawer_HPP
#define IGUIDrawer_HPP

#include "Platform/LinaGXIncl.hpp"

namespace Lina
{
	class SurfaceRenderer;

	class IGUIDrawer
	{
	public:
		IGUIDrawer();
		virtual ~IGUIDrawer() = default;

		virtual void DrawGUI(int threadID) = 0;
		virtual void OnKey(uint32 key, LinaGX::InputAction action){};
		virtual void OnMouse(uint32 button, LinaGX::InputAction action){};
		virtual void OnMousePos(const Vector2i& pos){};
		virtual void OnMouseMove(const Vector2i& pos){};
		virtual void OnMouseWheel(uint32 delta){};
		virtual void OnFocus(bool hasFocus){};
		virtual void OnMouseHoverEnd(){};
		virtual void OnWindowDrag(bool isDragging){};

		inline void SetSurfaceRenderer(SurfaceRenderer* rend)
		{
			m_surfaceRenderer = rend;
		}

		inline SurfaceRenderer* GetSurfaceRenderer() const
		{
			return m_surfaceRenderer;
		}

	protected:
		SurfaceRenderer* m_surfaceRenderer = nullptr;
	};
} // namespace Lina

#endif
