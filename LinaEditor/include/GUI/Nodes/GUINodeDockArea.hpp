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

#ifndef GUINodeDockArea_HPP
#define GUINodeDockArea_HPP

#include "GUINode.hpp"
#include "Data/Vector.hpp"
#include "Math/Rect.hpp"
#include "Core/EditorCommon.hpp"

namespace Lina::Editor
{
	class GUINodePanel;
	class EditorGUIDrawer;

	class GUINodeDockArea : public GUINode
	{
	public:
		GUINodeDockArea(EditorGUIDrawer* drawer, Editor* editor, ISwapchain* swapchain, int drawOrder) : m_drawer(drawer), GUINode(editor, swapchain, drawOrder){};
		virtual ~GUINodeDockArea() = default;

		virtual void Draw(int threadID) override;
		virtual bool OnMouse(uint32 button, InputAction act) override;
		void		 AddPanel(GUINodePanel* panel);
		void		 AddNewPanel(EditorPanel panel);
		void		 OnPanelClosed(GUINodePanel* panel);

		inline void SetSplitRect(const Rect& rect)
		{
			m_splitPercentages = rect;
		}

		inline const Rect& GetSplitRect() const
		{
			return m_splitPercentages;
		}

		inline void SetSplittedArea(GUINodeDockArea* area)
		{
			m_splittedArea = area;
		}

		inline GUINodeDockArea* GetSplittedArea()
		{
			return m_splittedArea;
		}

	private:
		void DrawTabs(int threadID);

	protected:
		EditorGUIDrawer*	  m_drawer = nullptr;
		Rect				  m_splitPercentages;
		Rect				  m_tabRect;
		Vector<GUINodePanel*> m_panels;
		GUINodePanel*		  m_focusedPanel = nullptr;
		GUINodeDockArea*	  m_splittedArea = nullptr;
	};
} // namespace Lina::Editor

#endif
