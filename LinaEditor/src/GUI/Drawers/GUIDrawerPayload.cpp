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

#include "GUI/Drawers/GUIDrawerPayload.hpp"
#include "Graphics/Core/SurfaceRenderer.hpp"
#include "Graphics/Interfaces/ISwapchain.hpp"
#include "Graphics/Interfaces/IWindow.hpp"
#include "Graphics/Platform/LinaVGIncl.hpp"
#include "GUI/Nodes/Custom/GUINodeTopPanel.hpp"
#include "GUI/Nodes/Custom/GUINodeTitleSection.hpp"
#include "GUI/Nodes/Docking/GUINodeDockArea.hpp"
#include "Core/Theme.hpp"

namespace Lina::Editor
{
	GUIDrawerPayload::GUIDrawerPayload(Editor* editor, ISwapchain* swap) : GUIDrawerBase(editor, swap)
	{
		m_editor = editor;
	}

	void GUIDrawerPayload::DrawGUI(int threadID)
	{
		const float dragHeight		= m_window->GetMonitorInformation().size.y * 0.02f;
		const float titleAreaHeight = m_window->GetMonitorInformation().size.y * 0.05f;

		const Vector2 swpSize = m_swapchain->GetSize();

		

		LinaVG::StyleOptions opts;
		const float			 thickness = m_window->GetDPIScale();
		opts.thickness				   = thickness * 2;
		opts.color					   = LV4(Theme::TC_Silent0);
		opts.isFilled				   = false;
		LinaVG::DrawRect(threadID, LV2(Vector2(thickness, thickness)), LV2((swpSize - Vector2(thickness, thickness))), opts, 0.0f, FRONT_DRAW_ORDER);
	}
} // namespace Lina::Editor
