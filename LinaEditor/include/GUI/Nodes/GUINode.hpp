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

#ifndef GUINode_HPP
#define GUINode_HPP

#include "Math/Rect.hpp"
#include "Input/Core/CommonInput.hpp"
#include "Data/Vector.hpp"
#include "Math/Vector.hpp"

namespace Lina
{
	class ISwapchain;
}

namespace Lina::Editor
{
	class Editor;
	class EditorGUIDrawer;

	class GUINode
	{
	public:
		GUINode(Editor* editor, ISwapchain* swapchain, int drawOrder);
		virtual ~GUINode();

		virtual void Draw(int threadID);
		virtual bool OnKey(uint32 key, InputAction action);
		virtual bool OnMouse(uint32 button, InputAction action);
		virtual bool OnMouseWheel(uint32 delta);
		virtual void OnClicked(uint32 button){};

		virtual Vector2 CalculateSize()
		{
			return Vector2::Zero;
		}

		GUINode* AddChildren(GUINode* node);
		GUINode* RemoveChildren(GUINode* node);
		GUINode* SetVisible(bool visible);

		inline Vector<GUINode*>& GetChildren()
		{
			return m_children;
		}

		inline GUINode* SetRect(const Rect& rect)
		{
			m_rect = rect;
			return this;
		}

		inline GUINode* SetPos(const Vector2& pos)
		{
			m_rect.pos = pos;
			return this;
		}

		inline GUINode* SetSize(const Vector2& size)
		{
			m_rect.size = size;
			return this;
		}

		inline GUINode* SetMinPos(const Vector2& minPos)
		{
			m_minPos = minPos;
			return this;
		}

		inline GUINode* SetMaxPos(const Vector2& maxPos)
		{
			m_maxPos = maxPos;
			return this;
		}

		inline const Rect& GetRect() const
		{
			return m_rect;
		}

		inline bool GetIsHovered() const
		{
			return m_isHovered;
		}

		inline int GetDrawOrder() const
		{
			return m_drawOrder;
		}

		inline bool GetIsVisible() const
		{
			return m_visible;
		}

	protected:
		friend class EditorGUIDrawer;

		bool			 m_visible	 = true;
		int				 m_drawOrder = 0;
		bool			 m_isHovered = false;
		bool			 m_isPressed = false;
		Editor*			 m_editor	 = nullptr;
		ISwapchain*		 m_swapchain = nullptr;
		Vector<GUINode*> m_children;
		Rect			 m_rect				  = Rect();
		Vector2			 m_minPos			  = Vector2::Zero;
		Vector2			 m_maxPos			  = Vector2::Zero;
		Vector2			 m_lastCalculatedSize = Vector2::Zero;
		float			 m_lastDpi			  = 0.0f;
	};
} // namespace Lina::Editor

#endif
