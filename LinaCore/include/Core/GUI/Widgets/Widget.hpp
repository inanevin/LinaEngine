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
#include "Common/Math/Rect.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Core/GUI/CommonGUI.hpp"
#include "Core/GUI/Theme.hpp"
#include "Common/Data/Bitmask.hpp"

namespace LinaGX
{
	class Window;
	enum class InputAction;

} // namespace LinaGX

namespace LinaVG
{
	struct StyleOptions;
}

namespace Lina
{
	class System;
	class ResourceManager;

	class Widget
	{
	public:
		virtual void PreTick();
		virtual void Tick(float delta);
		virtual void Draw(int32 threadIndex);

		virtual void AddChild(Widget* w);
		virtual void RemoveChild(Widget* w);
		void		 SetIsHovered();

		virtual void			   Construct(){};
		virtual void			   Destruct(){};
		virtual void			   Initialize();
		virtual bool			   OnMouse(uint32 button, LinaGX::InputAction action);
		virtual bool			   OnKey(uint32 keycode, int32 scancode, LinaGX::InputAction action);
		virtual void			   DebugDraw(int32 threadIndex, int32 drawOrder);
		virtual LinaGX::CursorType GetCursorOverride()
		{
			return LinaGX::CursorType::Default;
		}

		template <typename T> T* Allocate(const String& dbgName = "Widget")
		{
			T* t = m_manager->Allocate<T>();
			t->SetDebugName(dbgName);
			return t;
		}

		void Deallocate(Widget* w)
		{
			m_manager->Deallocate(w);
		}

		template <typename... Args> void AddChild(Args&&... args)
		{
			(AddChild(std::forward<Widget*>(args)), ...);
		}

		inline void SetPos(const Vector2& pos)
		{
			m_rect.pos = pos;
		}

		inline void SetPosX(float x)
		{
			m_rect.pos.x = x;
		}

		inline void SetPosY(float y)
		{
			m_rect.pos.y = y;
		}

		inline void SetSize(const Vector2& size)
		{
			m_rect.size = size;
		}

		inline void SetSizeX(float x)
		{
			m_rect.size.x = x;
		}

		inline void SetSizeY(float y)
		{
			m_rect.size.y = y;
		}

		inline const Rect& GetRect() const
		{
			return m_rect;
		}

		inline const Vector2& GetPos() const
		{
			return m_rect.pos;
		}

		inline float GetPosX() const
		{
			return m_rect.pos.x;
		}

		inline float GetPosY() const
		{
			return m_rect.pos.y;
		}

		inline float GetSizeX() const
		{
			return m_rect.size.x;
		}

		inline float GetSizeY() const
		{
			return m_rect.size.y;
		}

		inline float GetHalfSizeX() const
		{
			return m_rect.size.x * 0.5f;
		}

		inline float GetHalfSizeY() const
		{
			return m_rect.size.y * 0.5f;
		}

		inline const Vector2& GetSize() const
		{
			return m_rect.size;
		}

		inline const Vector2 GetHalfSize() const
		{
			return m_rect.size * 0.5f;
		}

		inline LinaGX::Window* GetWindow()
		{
			return m_lgxWindow;
		}

		inline int32 GetDrawOrder() const
		{
			return m_drawOrder;
		}

		inline void SetDrawOrder(int32 order)
		{
			m_drawOrder = order;
		}

		inline bool GetIsHovered() const
		{
			return m_isHovered;
		}

		inline void SetDebugName(const String& dbgName)
		{
			m_debugName = dbgName;
		}

		inline const String& GetDebugName() const
		{
			return m_debugName;
		}

		inline void SetChildID(uint32 idx)
		{
			m_childID = idx;
		}

		inline const Vector<Widget*>& GetChildren() const
		{
			return m_children;
		}

		inline Bitmask32& GetFlags()
		{
			return m_flags;
		}

		inline Widget* GetParent()
		{
			return m_parent;
		}

		inline TypeID GetTID() const
		{
			return m_tid;
		}

		inline const String& GetDisplayName() const
		{
			return m_displayName;
		}

		inline void SetDisplayName(const String& name)
		{
			m_displayName = name;
		}

		inline bool GetIsPressed() const
		{
			return m_isPressed;
		}

		inline float GetUserDataFloat() const
		{
			return m_userDataFloat;
		}

		inline void SetUserDataFloat(float f)
		{
			m_userDataFloat = f;
		}

	protected:
		friend class WidgetManager;

		Widget(int32 maxChilds = -1, Bitmask32 flags = 0) : m_maxChilds(maxChilds), m_flags(flags){};
		virtual ~Widget() = default;

	protected:
		TypeID			 m_tid		 = 0;
		WidgetManager*	 m_manager	 = nullptr;
		Widget*			 m_parent	 = nullptr;
		LinaGX::Window*	 m_lgxWindow = nullptr;
		int32			 m_drawOrder = 0;
		System*			 m_system	 = nullptr;
		Rect			 m_rect		 = {};
		Vector<Widget*>	 m_children;
		int32			 m_maxChilds	   = 0;
		bool			 m_isHovered	   = false;
		bool			 m_isPressed	   = false;
		ResourceManager* m_resourceManager = nullptr;
		String			 m_debugName	   = "Widget";
		uint32			 m_childID		   = 0;
		Bitmask32		 m_flags		   = 0;
		Widget*			 m_next			   = nullptr;
		Widget*			 m_prev			   = nullptr;
		String			 m_displayName	   = "Widget";
		float			 m_userDataFloat   = 0.0f;
	};

} // namespace Lina
