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
#include "Common/Memory/CommonMemory.hpp"
#include "Core/GUI/CommonGUI.hpp"
#include "Core/GUI/Theme.hpp"
#include "Common/Data/Bitmask.hpp"
#include "Core/Reflection/ClassReflection.hpp"

namespace LinaGX
{
	class Window;

	enum class InputAction;

} // namespace LinaGX

namespace LinaVG
{
	struct StyleOptions;
	class Drawer;
} // namespace LinaVG

namespace Lina
{
	class System;
	class ResourceManagerV2;
	class IStream;
	class Ostream;
	class WidgetManager;

#define WIDGET_VERSION 0
	/*
		0: Initial
	 */

#define V2_GET_MUTATE(NAME, VAR)                                                                                                                                                                                                                                   \
	inline void Set##NAME(const Vector2& sz)                                                                                                                                                                                                                       \
	{                                                                                                                                                                                                                                                              \
		VAR = sz;                                                                                                                                                                                                                                                  \
	}                                                                                                                                                                                                                                                              \
	inline void Set##NAME##X(float x)                                                                                                                                                                                                                              \
	{                                                                                                                                                                                                                                                              \
		VAR.x = x;                                                                                                                                                                                                                                                 \
	}                                                                                                                                                                                                                                                              \
	inline void Set##NAME##Y(float y)                                                                                                                                                                                                                              \
	{                                                                                                                                                                                                                                                              \
		VAR.y = y;                                                                                                                                                                                                                                                 \
	}                                                                                                                                                                                                                                                              \
	inline const Vector2& Get##NAME() const                                                                                                                                                                                                                        \
	{                                                                                                                                                                                                                                                              \
		return VAR;                                                                                                                                                                                                                                                \
	}                                                                                                                                                                                                                                                              \
	inline float Get##NAME##X() const                                                                                                                                                                                                                              \
	{                                                                                                                                                                                                                                                              \
		return VAR.x;                                                                                                                                                                                                                                              \
	}                                                                                                                                                                                                                                                              \
	inline float Get##NAME##Y() const                                                                                                                                                                                                                              \
	{                                                                                                                                                                                                                                                              \
		return VAR.y;                                                                                                                                                                                                                                              \
	}

#define V2_INCREMENTERS(NAME, VAR)                                                                                                                                                                                                                                 \
	inline void Add##NAME##X(float x)                                                                                                                                                                                                                              \
	{                                                                                                                                                                                                                                                              \
		VAR.x += x;                                                                                                                                                                                                                                                \
	}                                                                                                                                                                                                                                                              \
	inline void Add##NAME##Y(float y)                                                                                                                                                                                                                              \
	{                                                                                                                                                                                                                                                              \
		VAR.y += y;                                                                                                                                                                                                                                                \
	}

	class Widget
	{
	public:
		Widget(Bitmask32 flags = 0) : m_flags(flags), m_tid(GetTypeID<Widget>()){};
		virtual ~Widget() = default;

		virtual void Initialize();
		virtual void Construct(){};
		virtual void Destruct(){};
		virtual bool OnMouse(uint32 button, LinaGX::InputAction action)
		{
			return false;
		};
		virtual bool OnMouseWheel(float amt)
		{
			return false;
		};
		virtual bool OnMousePos(const Vector2& p)
		{
			return false;
		};
		virtual bool OnKey(uint32 keycode, int32 scancode, LinaGX::InputAction action)
		{
			return false;
		};
		virtual void DebugDraw(int32 drawOrder);
		virtual void PreTick(){};
		virtual void CalculateSize(float delta){};
		virtual void Tick(float delta){};
		virtual void Draw();
		virtual void SaveToStream(OStream& stream) const;
		virtual void LoadFromStream(IStream& stream);

		void	AddChild(Widget* w);
		void	ExecuteNextFrame(Delegate<void()>&& cb);
		void	RemoveChild(Widget* w);
		void	DeallocAllChildren();
		void	RemoveAllChildren();
		void	SetIsHovered();
		void	SetIsDisabled(bool isDisabled);
		void	DrawBorders();
		void	DrawTooltip();
		Vector2 GetStartFromMargins();
		Vector2 GetEndFromMargins();
		Rect	GetTemporaryAlignedRect();
		Vector2 GetWindowSize();
		Vector2 GetMonitorSize();
		Vector2 GetWindowPos();

		bool IsWidgetInHierarchy(Widget* widget);

		inline int32 GetDrawOrderIncrement() const
		{
			return m_drawOrderIncrement;
		}

		inline void SetDrawOrderIncrement(int32 incr)
		{
			m_drawOrderIncrement = incr;
		}

		virtual float CalculateChildrenSize()
		{
			return 0.0f;
		}

		inline Widget* GetCustomTooltip()
		{
			return m_customTooltip;
		}

		virtual LinaGX::CursorType GetCursorOverride()
		{
			return LinaGX::CursorType::Default;
		}

		inline const Rect& GetRect()
		{
			return m_rect;
		}

		inline float GetHalfSizeX() const
		{
			return m_rect.size.x * 0.5f;
		}

		inline float GetHalfSizeY() const
		{
			return m_rect.size.y * 0.5f;
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

		inline TypeID SetTID(TypeID tid)
		{
			m_tid = tid;
		}

		inline TypeID GetTID() const
		{
			return m_tid;
		}

		inline bool GetIsPressed() const
		{
			return m_isPressed;
		}

		inline TBLR& GetBorderThickness()
		{
			return m_borderThickness;
		}

		inline void SetBorderColor(const Color& c)
		{
			m_colorBorders = c;
		}

		inline Color& GetBorderColor()
		{
			return m_colorBorders;
		}

		inline const String& GetTooltip() const
		{
			return m_tooltip;
		}

		inline void SetTooltip(const String& str)
		{
			m_tooltip = str;
		}

		inline void SetPosAlignmentSourceX(PosAlignmentSource src)
		{
			m_posAlignSourceX = src;
		}

		inline void SetPosAlignmentSourceY(PosAlignmentSource src)
		{
			m_posAlignSourceY = src;
		}

		inline PosAlignmentSource GetPosAlignmentSourceX()
		{
			return m_posAlignSourceX;
		}

		inline PosAlignmentSource GetPosAlignmentSourceY()
		{
			return m_posAlignSourceY;
		}

		inline TBLR& GetChildMargins()
		{
			return m_childMargins;
		}

		inline float GetChildPadding() const
		{
			return m_childPadding;
		}

		inline void SetChildPadding(float padding)
		{
			m_childPadding = padding;
		}

		inline bool GetIsDisabled() const
		{
			return m_isDisabled;
		}

		inline WidgetManager* GetWidgetManager() const
		{
			return m_manager;
		}

		inline System* GetSystem() const
		{
			return m_system;
		}

		inline void SetVisible(bool visible)
		{
			m_isVisible = visible;
		}

		inline bool GetIsVisible() const
		{
			return m_isVisible;
		}

		inline void SetCustomTooltipUserData(void* ud)
		{
			m_customTooltipUserData = ud;
		}

		inline void SetBuildCustomTooltip(Delegate<Widget*(void*)>&& tt)
		{
			m_buildCustomTooltip = tt;
		}

		inline void SetScrollerOffset(float f)
		{
			m_scrollerOffset = f;
		}

		inline float GetScrollerOffset() const
		{
			return m_scrollerOffset;
		}

		inline void SetLocalControlsManager(Widget* w)
		{
			m_localControlsManager = w;
		}

		inline Widget* GetLocalControlsManager() const
		{
			return m_localControlsManager;
		}

		inline void SetLocalControlsOwner(Widget* w)
		{
			m_localControlsOwner = w;
		}

		inline Widget* GetLocalControlsOwner() const
		{
			return m_localControlsOwner;
		}

		inline void SetOnGrabbedControls(Delegate<void()>&& onGrabbed)
		{
			m_onGrabbedControls = onGrabbed;
		}

		inline void SetUserData(void* data)
		{
			m_userData = data;
		}

		inline void* GetUserData() const
		{
			return m_userData;
		}

		inline void SetTickHook(Delegate<void(float delta)>&& hook)
		{
			m_tickHook = hook;
		}

		template <typename... Args> void AddChild(Args&&... args)
		{
			(AddChild(std::forward<Widget*>(args)), ...);
		}

		template <typename T> static void GetWidgetsOfType(Vector<T*>& outWidgets, Widget* root)
		{
			auto tid = GetTypeID<T>();

			for (auto* c : root->GetChildren())
			{
				if (c->GetTID() == tid)
					outWidgets.push_back(static_cast<T*>(c));

				GetWidgetsOfType<T>(outWidgets, c);
			}
		}

		template <typename T> static T* GetWidgetOfType(Widget* root)
		{
			auto tid = GetTypeID<T>();

			for (Widget* c : root->GetChildren())
			{
				if (c->GetTID() == tid)
					return static_cast<T*>(c);

				T* widgetInChild = GetWidgetOfType<T>(c);
				if (widgetInChild != nullptr)
					return widgetInChild;
			}

			return nullptr;
		}

		inline uint32 GetCacheIndex() const
		{
			return m_cacheIndex;
		}
		V2_GET_MUTATE(FixedSize, m_fixedSize);
		V2_GET_MUTATE(AlignedSize, m_alignedSize);
		V2_INCREMENTERS(AlignedSize, m_alignedSize);
		V2_GET_MUTATE(Pos, m_rect.pos);
		V2_GET_MUTATE(Size, m_rect.size);
		V2_GET_MUTATE(AlignedPos, m_alignedPos);
		V2_INCREMENTERS(AlignedPos, m_alignedPos);
		LinaVG::Drawer* m_lvg = nullptr;

	protected:
		friend class WidgetManager;
		template <typename U> friend class WidgetCache;

		void ChangedParent(Widget* w);
		void CheckCustomTooltip();

	protected:
		ALLOCATOR_BUCKET_MEM;
		WidgetManager*				m_manager			   = nullptr;
		ResourceManagerV2*			m_resourceManager	   = nullptr;
		LinaGX::Window*				m_lgxWindow			   = nullptr;
		System*						m_system			   = nullptr;
		Widget*						m_parent			   = nullptr;
		Widget*						m_next				   = nullptr;
		Widget*						m_prev				   = nullptr;
		Widget*						m_controlsOwner		   = nullptr;
		Widget*						m_controlsManager	   = nullptr;
		Widget*						m_customTooltip		   = nullptr;
		Widget*						m_localControlsManager = nullptr;
		Widget*						m_localControlsOwner   = nullptr;
		Delegate<Widget*(void*)>	m_buildCustomTooltip;
		Vector<Widget*>				m_children;
		Vector<Delegate<void()>>	m_executeNextFrame;
		Delegate<void()>			m_onGrabbedControls;
		Delegate<void(float delta)> m_tickHook;
		Rect						m_rect					= {};
		Vector2						m_fixedSize				= Vector2::Zero;
		Vector2						m_alignedPos			= Vector2::Zero;
		Vector2						m_alignedSize			= Vector2::Zero;
		String						m_tooltip				= "";
		String						m_debugName				= "Widget";
		Color						m_colorBorders			= Theme::GetDef().background2;
		TypeID						m_tid					= 0;
		int32						m_drawOrder				= 0;
		uint32						m_childID				= 0;
		Bitmask32					m_flags					= 0;
		TBLR						m_childMargins			= {};
		TBLR						m_borderThickness		= {};
		PosAlignmentSource			m_posAlignSourceX		= PosAlignmentSource::Start;
		PosAlignmentSource			m_posAlignSourceY		= PosAlignmentSource::Start;
		bool						m_isHovered				= false;
		bool						m_isPressed				= false;
		bool						m_isDisabled			= false;
		bool						m_isVisible				= true;
		uint32						m_loadedVersion			= 0;
		float						m_childPadding			= 0.0f;
		float						m_scrollerOffset		= 0.0f;
		void*						m_customTooltipUserData = nullptr;
		void*						m_userData				= nullptr;
		uint32						m_cacheIndex			= 0;
		int32						m_drawOrderIncrement	= 0;
	};

	LINA_REFLECTWIDGET_BEGIN(Widget)
	LINA_REFLECTWIDGET_END(Widget)

} // namespace Lina
