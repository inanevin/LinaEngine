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

#ifndef EditorGUIDrawer_HPP
#define EditorGUIDrawer_HPP

#include "Graphics/Interfaces/IGUIDrawer.hpp"
#include "Core/StringID.hpp"
#include "Core/EditorCommon.hpp"
#include "Data/HashMap.hpp"
#include "Math/Rect.hpp"

namespace Lina::Editor
{
	class GUINode;
	class Editor;
	class GUINodeTitleSection;
	class GUINodeDockArea;

	class EditorGUIDrawer : public IGUIDrawer
	{
	public:
		EditorGUIDrawer(Editor* editor, ISwapchain* swapchain, EditorPanel panelType, StringID sid);
		virtual ~EditorGUIDrawer();

		virtual void DrawGUI(int threadID) override;
		virtual void OnKey(uint32 key, InputAction action) override;
		virtual void OnMouse(uint32 button, InputAction action) override;
		virtual void OnMousePos(const Vector2i& pos) override;
		virtual void OnMouseMove(const Vector2i& pos) override;
		virtual void OnMouseWheel(uint32 delta) override;

		inline StringID GetSID() const
		{
			return m_sid;
		}

		inline GUINodeDockArea* GetFirstDockArea()
		{
			return m_dockAreas[0];
		}

		void SplitDockArea(GUINodeDockArea* area, DockSplitType type);

		inline bool GetIsDraggingPanelTab() const
		{
			return true;
			return m_isDraggingPanelTab;
		}

	private:
		GUINode* GetHovered(GUINode* parent);

	protected:
		GUINode*				 m_previousHovered	  = nullptr;
		bool					 m_isDraggingPanelTab = false;
		StringID				 m_sid				  = 0;
		GUINode*				 m_hoveredNode		  = nullptr;
		Editor*					 m_editor			  = nullptr;
		GUINode*				 m_root				  = nullptr;
		GUINodeTitleSection*	 m_titleSection		  = nullptr;
		Vector<GUINodeDockArea*> m_dockAreas;
	};
} // namespace Lina::Editor

#endif
