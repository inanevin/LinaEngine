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

#include "Core/GUI/Widgets/Widget.hpp"
#include "Common/Data/Vector.hpp"
#include "Common/Common.hpp"

namespace Lina
{
	class DirectionalLayout : public Widget
	{
	public:
		DirectionalLayout(uint32 flags = 0) : Widget(flags){};
		virtual ~DirectionalLayout() = default;

		enum class Mode
		{
			Default = 0,
			EqualPositions,
			EqualSizes,
			Bordered,
		};

		struct Properties
		{
			DirectionOrientation direction				  = DirectionOrientation::Horizontal;
			Mode				 mode					  = Mode::Default;
			Color				 colorBorder			  = Theme::GetDef().background0;
			Color				 colorBorderHovered		  = Theme::GetDef().background3;
			float				 borderThickness		  = Theme::GetDef().baseBorderThickness;
			float				 borderMinSize			  = 0.1f;
			int32				 borderExpandForMouse	  = 0;
			uint32				 borderDrawOrderIncrement = 0;

			Delegate<void()> onClicked;
			Delegate<void()> onDoubleClicked;
			Delegate<void()> onPressed;
			Delegate<void()> onDestructed;
			Delegate<void()> onHoverBegin;
			Delegate<void()> onHoverEnd;
			Delegate<void()> onBordersChanged;

			void SaveToStream(OStream& stream) const;
			void LoadFromStream(IStream& stream);
		};

		virtual void  Construct() override;
		virtual void  Destruct() override;
		virtual void  PreTick() override;
		virtual void  Tick(float delta) override;
		virtual void  Draw() override;
		virtual void  DebugDraw(int32 drawOrder) override;
		virtual bool  OnMouse(uint32 button, LinaGX::InputAction act) override;
		virtual float CalculateChildrenSize() override;

		virtual void SaveToStream(OStream& stream) const override
		{
			Widget::SaveToStream(stream);
			stream << m_props;
		}

		virtual void LoadFromStream(IStream& stream) override
		{
			Widget::LoadFromStream(stream);
			stream >> m_props;
		}

		inline void SetProps(const Properties& props)
		{
			m_props = props;
		}

		inline Properties& GetProps()
		{
			return m_props;
		}

	private:
		static constexpr float ANIM_TIME = 0.1f;
		void				   BehaviourDefault(float delta);
		void				   BehaviourEqualPositions(float delta);
		void				   BehaviourEqualSizes(float delta);
		void				   BehaviourBorders(float delta);
		void				   GetPressRect(const Rect& original, Rect& outPress);

	protected:
		LINA_REFLECTION_ACCESS(DirectionalLayout);
		Properties m_props = {};

	private:
		int32		 m_pressedBorder   = -1;
		float		 m_borderPressDiff = 0.0f;
		Vector<Rect> m_borderRects;
		Vector2		 m_start		   = Vector2::Zero;
		Vector2		 m_end			   = Vector2::Zero;
		Vector2		 m_sz			   = Vector2::Zero;
		Vector2		 m_center		   = Vector2::Zero;
		bool		 m_lastHoverStatus = false;
		float		 m_animValue	   = 0.0f;
	};

	LINA_WIDGET_BEGIN(DirectionalLayout, Layout)
	LINA_FIELD(DirectionalLayout, m_props, "Directional Layout Properties", FieldType::UserClass, GetTypeID<DirectionalLayout::Properties>())
	LINA_CLASS_END(DirectionalLayout)

	LINA_CLASS_BEGIN(DirectionalLayoutProperties)
	LINA_FIELD(DirectionalLayout::Properties, direction, "Direction", FieldType::Enum, GetTypeID<DirectionOrientation>())
	LINA_FIELD(DirectionalLayout::Properties, mode, "Mode", FieldType::Enum, GetTypeID<DirectionalLayout::Mode>())
	LINA_FIELD(DirectionalLayout::Properties, colorBorder, "Color Border", FieldType::Color, 0)
	LINA_FIELD(DirectionalLayout::Properties, colorBorderHovered, "Color Border Hovered", FieldType::Color, 0)
	LINA_FIELD(DirectionalLayout::Properties, borderThickness, "Border Thickness", FieldType::Float, 0)
	LINA_FIELD(DirectionalLayout::Properties, borderExpandForMouse, "Border Mouse Expand", FieldType::Int32, 0)
	LINA_FIELD(DirectionalLayout::Properties, borderMinSize, "Border Min Size", FieldType::Float, 0)
	LINA_FIELD_LIMITS(DirectionalLayout::Properties, borderMinSize, 0.0f, 1.0f, 0.1f)

	LINA_FIELD_DEPENDENCY(DirectionalLayout::Properties, colorBorder, "mode", "3")
	LINA_FIELD_DEPENDENCY(DirectionalLayout::Properties, colorBorderHovered, "mode", "3")
	LINA_FIELD_DEPENDENCY(DirectionalLayout::Properties, borderThickness, "mode", "3")
	LINA_FIELD_DEPENDENCY(DirectionalLayout::Properties, borderExpandForMouse, "mode", "3")
	LINA_FIELD_DEPENDENCY(DirectionalLayout::Properties, borderMinSize, "mode", "3")
	LINA_CLASS_END(DirectionalLayoutProperties)

	LINA_CLASS_BEGIN(DirectionalLayoutMode)
	LINA_PROPERTY_STRING(DirectionalLayout::Mode, 0, "Default")
	LINA_PROPERTY_STRING(DirectionalLayout::Mode, 1, "Equal Positions")
	LINA_PROPERTY_STRING(DirectionalLayout::Mode, 2, "Equal Sizes")
	LINA_PROPERTY_STRING(DirectionalLayout::Mode, 3, "Bordered")
	LINA_CLASS_END(DirectionalLayoutMode)

} // namespace Lina
