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
#include "Core/GUI/CommonGUI.hpp"
#include "Core/GUI/Theme.hpp"
#include "Core/Resources/CommonResources.hpp"
#include "Common/Data/Bitmask.hpp"
#include "Common/Data/HashSet.hpp"
#include "Common/Tween/Tween.hpp"
#include "Core/Reflection/WidgetReflection.hpp"

// Common inputs.
#include <LinaGX/Core/Window.hpp>
#include <LinaGX/Core/Input.hpp>

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
	class Icon;
	class System;
	class ResourceManagerV2;
	class IStream;
	class WidgetManager;
	struct TextureAtlasImage;
	class Texture;

#define WIDGET_VERSION		0
#define WIDGETPROPS_VERSION 0
#define DS_VERSION			0
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

	struct DropshadowProps
	{
		bool		enabled			   = false;
		bool		isInner			   = false;
		float		margin			   = 0.0f;
		float		thickness		   = Theme::GetDef().baseOutlineThickness;
		float		rounding		   = 0.0f;
		int32		drawOrderIncrement = 0;
		uint32		steps			   = 4;
		Color		color			   = Theme::GetDef().black;
		Direction	direction		   = Direction::Center;
		Vector<int> onlyRound;

		virtual void SaveToStream(OStream& stream) const;
		virtual void LoadFromStream(IStream& stream);
	};

	struct WidgetProps
	{
		ResourceID			 material					 = ENGINE_MATERIAL_GUI_DEFAULT_ID;
		bool				 clipChildren				 = false;
		bool				 drawBackground				 = false;
		bool				 backgroundIsCentralGradient = false;
		bool				 interpolateColor			 = false;
		bool				 fitTexture					 = false;
		bool				 hoveredIsDifferentColor	 = false;
		bool				 pressedIsDifferentColor	 = false;
		bool				 activeTextureTiling		 = false;
		bool				 useSpecialTexture			 = false;
		bool				 outlineIsInner				 = false;
		bool				 outlineAffectedByMainColor	 = false;
		float				 colorInterpolateSpeed		 = 0.0f;
		float				 outlineThickness			 = Theme::GetDef().baseOutlineThickness;
		float				 rounding					 = 0.0f;
		float				 childPadding				 = 0.0f;
		void*				 lvgUserData				 = nullptr;
		int32				 drawOrderIncrement			 = 0;
		String				 tooltip					 = "";
		String				 debugName					 = "";
		TBLR				 childMargins				 = {};
		TBLR				 borderThickness			 = {};
		Color				 colorBorders				 = Theme::GetDef().outlineColorBase;
		ColorGrad			 colorBackground			 = Theme::GetDef().background0;
		ColorGrad			 colorBackgroundAlt			 = Theme::GetDef().background0;
		ColorGrad			 colorOutline				 = Theme::GetDef().outlineColorBase;
		ColorGrad			 colorOutlineControls		 = Theme::GetDef().outlineColorControls;
		ColorGrad			 colorHovered				 = Theme::GetDef().background2;
		ColorGrad			 colorPressed				 = Theme::GetDef().background1;
		ColorGrad			 colorDisabled				 = Theme::GetDef().silent0;
		DirectionOrientation colorBackgroundDirection	 = DirectionOrientation::Horizontal;
		TextureAtlasImage*	 textureAtlas				 = nullptr;
		Texture*			 rawTexture					 = nullptr;
		void*				 specialTexture				 = 0;
		Vector2				 textureTiling				 = Vector2::One;
		Vector<int32>		 onlyRound					 = {};
		Rect				 childrenClipOffset			 = {};
		ColorGrad			 _interpolatedColor			 = Color();
		DropshadowProps		 dropshadow;
		bool				 altColorsToggled = false;
		bool				 useSizeTween	  = false;
		bool				 aaEnabled		  = false;
		Tween				 sizeTween		  = Tween(1.0f, 1.0f, 0.1f);

		virtual void SaveToStream(OStream& stream) const;
		virtual void LoadFromStream(IStream& stream);
	};

	struct WidgetCallbacks
	{
		Delegate<void()> onEditStarted;
		Delegate<void()> onEdited;
		Delegate<void()> onEditEnded;
	};

	class Widget
	{
	public:
		Widget(Bitmask32 flags = 0) : m_flags(flags), m_tid(GetTypeID<Widget>()) {};
		virtual ~Widget() = default;

		virtual void Initialize();
		virtual void Construct() {};
		virtual void Destruct() {};
		virtual void PreDestruct() {};
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

		virtual Widget* GetNextControls()
		{
			return nullptr;
		}

		virtual Widget* GetPrevControls()
		{
			return nullptr;
		}

		virtual void OnGrabbedControls(bool isForward, Widget* prevControls) {

		};

		virtual void OnLostControls(Widget* newControls) {

		};

		virtual void DebugDraw(int32 drawOrder);
		virtual void PreTick() {};
		virtual void CalculateSize(float delta) {};
		virtual void Tick(float delta) {};
		virtual void Draw() {};
		virtual void SaveToStream(OStream& stream) const;
		virtual void LoadFromStream(IStream& stream);
		virtual void CollectResourceReferences(HashSet<ResourceID>& out);

		void	SetWidgetManager(WidgetManager* wm);
		void	DrawDropshadow();
		void	DrawBackground();
		void	AddChild(Widget* w);
		void	RemoveChild(Widget* w);
		void	DeallocAllChildren();
		void	RemoveAllChildren();
		void	SetIsHovered();
		void	DrawBorders();
		void	DrawTooltip();
		Vector2 GetStartFromMargins();
		Vector2 GetEndFromMargins();
		Rect	GetTemporaryAlignedRect();
		Vector2 GetMonitorSize();
		Vector2 GetWindowPos();
		Widget* FindChildWithUserdata(void* ud);
		Widget* FindChildWithUniqueID(uint32 uniqueID);
		Widget* FindChildWithDebugName(const String& name);
		Widget* FindDeepestHovered();
		bool	ShouldSkipDrawOutsideWindow() const;
		void	CollectResourceReferencesRecursive(HashSet<ResourceID>& out);

		bool IsWidgetInHierarchy(Widget* widget);
		void CheckClipChildren();
		void CheckClipChildrenEnd();
		void PropagateCBOnEditStarted();
		void PropagateCBOnEdited();
		void PropagateCBOnEditEnded();

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

		inline void SetAnchorX(Anchor src)
		{
			m_anchorX = src;
		}

		inline void SetAnchorY(Anchor src)
		{
			m_anchorY = src;
		}

		inline Anchor GetPosAlignmentSourceX()
		{
			return m_anchorX;
		}

		inline Anchor GetPosAlignmentSourceY()
		{
			return m_anchorY;
		}

		inline WidgetManager* GetWidgetManager() const
		{
			return m_manager;
		}

		inline System* GetSystem() const
		{
			return m_system;
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

		inline void AddPreTickHook(Delegate<void()>&& hook)
		{
			m_preTickHooks.push_back(std::move(hook));
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

		inline WidgetProps& GetWidgetProps()
		{
			return m_widgetProps;
		}

		inline uint32 GetCacheIndex() const
		{
			return m_cacheIndex;
		}

		inline void AddChildRequest(Widget* w)
		{
			m_addChildRequests.push_back(w);
		}

		inline WidgetCallbacks& GetCallbacks()
		{
			return m_callbacks;
		}

		inline void SetUniqueID(uint32 id)
		{
			m_uniqueID = id;
		}

		inline uint32 GetUniqueID()
		{
			return m_uniqueID;
		}

		V2_GET_MUTATE(FixedSize, m_fixedSize);
		V2_GET_MUTATE(AlignedSize, m_alignedSize);
		V2_INCREMENTERS(AlignedSize, m_alignedSize);
		V2_GET_MUTATE(Pos, m_rect.pos);
		V2_GET_MUTATE(Size, m_rect.size);
		V2_GET_MUTATE(AlignedPos, m_alignedPos);
		V2_INCREMENTERS(AlignedPos, m_alignedPos);
		LinaVG::Drawer* m_lvg = nullptr;

		bool _fold = false;

	protected:
		friend class WidgetManager;
		template <typename U> friend class WidgetCache;

		void ChangedParent(Widget* w);
		void CheckCustomTooltip();

	protected:
		LINA_REFLECTION_ACCESS(Widget);
		ALLOCATOR_BUCKET_MEM;
		WidgetManager*				m_manager			   = nullptr;
		ResourceManagerV2*			m_resourceManager	   = nullptr;
		LinaGX::Window*				m_lgxWindow			   = nullptr;
		System*						m_system			   = nullptr;
		Widget*						m_parent			   = nullptr;
		Widget*						m_next				   = nullptr;
		Widget*						m_prev				   = nullptr;
		Widget*						m_customTooltip		   = nullptr;
		Widget*						m_localControlsManager = nullptr;
		Widget*						m_localControlsOwner   = nullptr;
		Delegate<Widget*(void*)>	m_buildCustomTooltip;
		Vector<Widget*>				m_children;
		Delegate<void(float delta)> m_tickHook;
		Vector<Delegate<void()>>	m_preTickHooks;
		Rect						m_rect					= {};
		Vector2						m_fixedSize				= Vector2::Zero;
		Vector2						m_alignedPos			= Vector2::Zero;
		Vector2						m_alignedSize			= Vector2::Zero;
		TypeID						m_tid					= 0;
		int32						m_drawOrder				= 0;
		Bitmask32					m_flags					= 0;
		Anchor						m_anchorX				= Anchor::Start;
		Anchor						m_anchorY				= Anchor::Start;
		bool						m_isHovered				= false;
		bool						m_isPressed				= false;
		uint32						m_loadedVersion			= 0;
		float						m_scrollerOffset		= 0.0f;
		void*						m_customTooltipUserData = nullptr;
		void*						m_userData				= nullptr;
		uint32						m_cacheIndex			= 0;
		WidgetProps					m_widgetProps			= {};
		bool						m_initializing			= false;
		bool						m_initialized			= false;
		Vector<Widget*>				m_addChildRequests;
		WidgetCallbacks				m_callbacks = {};
		uint32						m_uniqueID	= 0;
	};

	LINA_WIDGET_BEGIN(Widget, General)
	LINA_FIELD(Widget, m_flags, "Flags", FieldType::Bitmask32, GetTypeID<WidgetFlags>())
	LINA_FIELD(Widget, m_alignedPos, "Aligned Position", FieldType::Vector2, 0)
	LINA_FIELD_LIMITS(Widget, m_alignedPos, 0.0f, 1.0f, 0.1f)
	LINA_FIELD(Widget, m_alignedSize, "Aligned Size", FieldType::Vector2, 0)
	LINA_FIELD_LIMITS(Widget, m_alignedSize, 0.0f, 1.0f, 0.1f)
	LINA_FIELD(Widget, m_fixedSize, "Fixed Size", FieldType::Vector2, 0)
	LINA_FIELD(Widget, m_anchorX, "Anchor X", FieldType::Enum, GetTypeID<Anchor>())
	LINA_FIELD(Widget, m_anchorY, "Anchor Y", FieldType::Enum, GetTypeID<Anchor>())
	LINA_FIELD(Widget, m_widgetProps, "Widget Properties", FieldType::UserClass, GetTypeID<WidgetProps>())
	LINA_CLASS_END(Widget)

	LINA_CLASS_BEGIN(DropshadowProps)
	LINA_FIELD(DropshadowProps, enabled, "Enabled", FieldType::Boolean, 0);
	LINA_FIELD(DropshadowProps, isInner, "Is Inner", FieldType::Boolean, 0);
	LINA_FIELD(DropshadowProps, margin, "Margin", FieldType::Float, 0);
	LINA_FIELD(DropshadowProps, thickness, "Thickness", FieldType::Float, 0);
	LINA_FIELD(DropshadowProps, rounding, "Rounding", FieldType::Float, 0);
	LINA_FIELD(DropshadowProps, drawOrderIncrement, "Draw Order Increment", FieldType::Int32, 0);
	LINA_FIELD(DropshadowProps, steps, "Steps", FieldType::UInt32, 0);
	LINA_FIELD(DropshadowProps, color, "Color", FieldType::Color, 0);
	LINA_FIELD(DropshadowProps, direction, "Direction", FieldType::Enum, GetTypeID<Direction>());
	LINA_FIELD_VEC(DropshadowProps, onlyRound, "Only Round", FieldType::Int32, int32, 0);

	LINA_FIELD_DEPENDENCY(DropshadowProps, isInner, "enabled", "1");
	LINA_FIELD_DEPENDENCY(DropshadowProps, margin, "enabled", "1");
	LINA_FIELD_DEPENDENCY(DropshadowProps, thickness, "enabled", "1");
	LINA_FIELD_DEPENDENCY(DropshadowProps, rounding, "enabled", "1");
	LINA_FIELD_DEPENDENCY(DropshadowProps, drawOrderIncrement, "enabled", "1");
	LINA_FIELD_DEPENDENCY(DropshadowProps, steps, "enabled", "1");
	LINA_FIELD_DEPENDENCY(DropshadowProps, color, "enabled", "1");
	LINA_FIELD_DEPENDENCY(DropshadowProps, direction, "enabled", "1");
	LINA_FIELD_DEPENDENCY(DropshadowProps, onlyRound, "enabled", "1");
	LINA_CLASS_END(DropshadowProps)

} // namespace Lina
