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
#include "Core/StringID.hpp"
#include "Data/Functional.hpp"
#include "Data/String.hpp"
#include "Core/EditorCommon.hpp"
#include "Serialization/ISerializable.hpp"
#include "Data/Bitmask.hpp"

namespace Lina
{
	class ISwapchain;
	class IWindow;
} // namespace Lina

namespace Lina::Editor
{
	class Editor;
	class GUIDrawerBase;

	class GUINode : public ISerializable
	{
	public:
		GUINode(GUIDrawerBase* drawer, int drawOrder);
		virtual ~GUINode();

		virtual void Draw(int threadID);
		virtual bool OnKey(uint32 key, InputAction action);
		virtual bool OnMouse(uint32 button, InputAction action);
		virtual bool OnMouseWheel(uint32 delta);
		virtual void OnLostFocus();
		virtual bool OnShortcut(Shortcut sc);
		virtual void OnPayloadCreated(PayloadType type, void* userData);
		virtual void OnPayloadEnded(PayloadType type);
		virtual void OnPayloadAccepted(){};
		virtual void OnClicked(uint32 button){};
		virtual void OnPressed(uint32 button){};
		virtual void OnDoubleClicked(){};
		virtual void OnHoverBegin(){};
		virtual void OnHoverEnd(){};
		virtual void OnDragBegin(){};
		virtual void OnDragEnd(){};
		virtual void SaveToStream(OStream& stream) override;
		virtual void LoadFromStream(IStream& stream) override;

		void	 SetDrawer(GUIDrawerBase* drawer);
		GUINode* AddChildren(GUINode* node);
		GUINode* RemoveChildren(GUINode* node);
		GUINode* SetVisible(bool visible);
		GUINode* FindChildren(StringID sid);
		bool	 ChildExists(GUINode* node);
		
		virtual Vector2 CalculateSize()
		{
			return Vector2::Zero;
		}

		inline Vector<GUINode*>& GetChildren()
		{
			return m_children;
		}

		inline void SetRect(const Rect& rect)
		{
			m_rect = rect;
		}

		inline void SetPos(const Vector2& pos)
		{
			m_rect.pos = pos;
		}

		inline void SetSize(const Vector2& size)
		{
			m_rect.size = size;
		}

		inline void SetMinPos(const Vector2& minPos)
		{
			m_minRect.pos = minPos;
		}

		inline void SetMaxPos(const Vector2& maxPos)
		{
			m_maxRect.pos = maxPos;
		}

		inline void SetMinSize(const Vector2& size)
		{
			m_minRect.size = size;
		}

		inline void SetMaxSize(const Vector2& size)
		{
			m_maxRect.size = size;
		}

		inline void SetCallbackClicked(Delegate<void(GUINode*)>&& onClicked)
		{
			m_onClicked = onClicked;
		}

		inline void SetCallbackDismissed(Delegate<void(GUINode*)>&& onDismissed)
		{
			m_onDismissed = onDismissed;
		}

		inline void SetCallbackPayloadAccepted(Delegate<void(GUINode*, void*)>&& onPayloadAccepted)
		{
			m_onPayloadAccepted = onPayloadAccepted;
		}

		inline void SetTitle(const char* title)
		{
			m_title = title;
		}

		inline void SetSID(StringID sid)
		{
			m_sid = sid;
		}

		inline const String& GetTitle() const
		{
			return m_title;
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

		inline bool GetIsPressed() const
		{
			return m_isPressed;
		}

		inline bool GetIsDragging() const
		{
			return m_isDragging;
		}

		inline StringID GetSID() const
		{
			return m_sid;
		}

		inline Editor* GetEditor() const
		{
			return m_editor;
		}

		inline ISwapchain* GetSwapchain() const
		{
			return m_swapchain;
		}

		inline GUIDrawerBase* GetDrawer() const
		{
			return m_drawer;
		}

		inline bool GetIsDisabled() const
		{
			return m_disabled;
		}

		void SetDisabled(bool isDisabled)
		{
			m_disabled = isDisabled;
		}

		inline Bitmask16& GetPayloadMask()
		{
			return m_payloadMask;
		}

	protected:
		friend class GUIDrawerBase;

		bool							m_payloadAvailable = false;
		bool							m_disabled		   = false;
		Bitmask16						m_payloadMask	   = 0;
		GUIDrawerBase*					m_drawer;
		Editor*							m_editor			  = nullptr;
		ISwapchain*						m_swapchain			  = nullptr;
		IWindow*						m_window			  = nullptr;
		StringID						m_sid				  = 0;
		bool							m_visible			  = true;
		int								m_drawOrder			  = 0;
		bool							m_isHovered			  = false;
		bool							m_isPressed			  = false;
		bool							m_isDragging		  = false;
		Vector2i						m_dragStartMousePos	  = Vector2i::Zero;
		Vector2i						m_dragStartMouseDelta = Vector2i::Zero;
		Vector<GUINode*>				m_children;
		Rect							m_rect				 = Rect();
		Rect							m_minRect			 = Rect();
		Rect							m_maxRect			 = Rect();
		Vector2							m_lastCalculatedSize = Vector2::Zero;
		float							m_lastDpi			 = 0.0f;
		String							m_title				 = "";
		Delegate<void(GUINode*)>		m_onClicked;
		Delegate<void(GUINode*)>		m_onDismissed;
		Delegate<void(GUINode*, void*)> m_onPayloadAccepted;
	};
} // namespace Lina::Editor

#endif
