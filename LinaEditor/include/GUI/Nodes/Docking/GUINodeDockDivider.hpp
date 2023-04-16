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

#ifndef GUINodeDockDivider_HPP
#define GUINodeDockDivider_HPP

#include "GUI/Nodes/GUINode.hpp"
#include "Data/String.hpp"
#include "Math/Color.hpp"
#include "Data/Functional.hpp"
#include "Core/EditorCommon.hpp"

namespace Lina::Editor
{
	class GUINodeDockArea;

	class GUINodeDockDivider : public GUINode
	{
	public:
		GUINodeDockDivider(Editor* editor, ISwapchain* swapchain, int drawOrder) : GUINode(editor, swapchain, drawOrder){};
		virtual ~GUINodeDockDivider() = default;

		virtual void Draw(int threadID) override;

		inline GUINodeDockDivider* SetDefaultColor(const Color& color)
		{
			m_defaultColor = color;
			return this;
		}

		inline GUINodeDockDivider* SetDragDirection(DragDirection dragDir)
		{
			m_dragDirection = dragDir;
			return this;
		}

		inline GUINodeDockDivider* AddDockAreaPositiveSide(GUINodeDockArea* area);
		inline GUINodeDockDivider* AddDockAreaNegativeSide(GUINodeDockArea* area);
		inline GUINodeDockDivider* RemoveDockAreaPositiveSide(GUINodeDockArea* area);
		inline GUINodeDockDivider* RemoveDockAreaNegativeSide(GUINodeDockArea* area);

	private:
		DragDirection			 m_dragDirection	= DragDirection::None;
		Color					 m_defaultColor;
		bool					 m_pressRecorded = false;
		Vector<GUINodeDockArea*> m_positiveSideDockAreas;
		Vector<GUINodeDockArea*> m_negativeSideDockAreas;
	};

} // namespace Lina::Editor

#endif
