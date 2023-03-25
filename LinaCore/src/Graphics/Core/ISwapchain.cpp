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

#include "Graphics/Interfaces/ISwapchain.hpp"
#include "Graphics/Platform/RendererIncl.hpp"
#include "Graphics/Core/GfxManager.hpp"
#include "Graphics/Core/WindowManager.hpp"
#include "Graphics/Interfaces/IWindow.hpp"
#include "System/ISystem.hpp"

namespace Lina
{
	ISwapchain::ISwapchain(Renderer* renderer, const Vector2i& size, IWindow* window, StringID sid) : m_size(size), m_window(window), m_sid(sid)
	{
		m_renderer		= renderer;
		m_windowManager = m_renderer->GetGfxManager()->GetSystem()->CastSubsystem<WindowManager>(SubsystemType::WindowManager);
	}
	uint32 ISwapchain::GetWindowDPI() const
	{
		return m_window->GetDPI();
	}
	float ISwapchain::GetWindowDPIScale() const
	{
		return m_window->GetDPIScale();
	}
} // namespace Lina
