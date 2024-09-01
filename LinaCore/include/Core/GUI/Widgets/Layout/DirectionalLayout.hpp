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
			Default,
			EqualPositions,
			EqualSizes,
			Bordered,
		};

		enum class BackgroundStyle
		{
			None,
			Default,
			CentralGradient
		};

		struct Properties
		{
			DirectionOrientation direction			= DirectionOrientation::Horizontal;
			Mode				 mode				= Mode::Default;
			Color				 colorBorder		= Theme::GetDef().background0;
			Color				 colorBorderHovered = Theme::GetDef().background3;
			float				 borderThickness	= Theme::GetDef().baseBorderThickness;
			float				 borderMinSize		= 0.15f;
			bool				 receiveInput		= false;
			bool				 _category			= false;

			Delegate<void()> onClicked;
			Delegate<void()> onDoubleClicked;
			Delegate<void()> onRightClicked;
			Delegate<void()> onPressed;
			Delegate<void()> onDestructed;
			Delegate<void()> onHoverBegin;
			Delegate<void()> onHoverEnd;

			void SaveToStream(OStream& stream) const;
			void LoadFromStream(IStream& stream);
		};

		virtual void			   Destruct() override;
		virtual void			   Initialize() override;
		virtual void			   PreTick() override;
		virtual void			   Tick(float delta) override;
		virtual void			   Draw() override;
		virtual void			   DebugDraw(int32 drawOrder) override;
		virtual bool			   OnMouse(uint32 button, LinaGX::InputAction act) override;
		virtual float			   CalculateChildrenSize() override;
		virtual LinaGX::CursorType GetCursorOverride() override;

		virtual void SaveToStream(OStream& stream) const override
		{
			Widget::SaveToStream(stream);
			m_props.SaveToStream(stream);
		}

		virtual void LoadFromStream(IStream& stream) override
		{
			Widget::LoadFromStream(stream);
			m_props.LoadFromStream(stream);
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
	LINA_FIELD(DirectionalLayout, m_props, "Directional Layout Properties", "Class", GetTypeID<DirectionalLayout::Properties>())
	LINA_CLASS_END(DirectionalLayout)

	LINA_CLASS_BEGIN(DirectionalLayoutProperties)
	LINA_FIELD(DirectionalLayout::Properties, _category, "Directional Layout Properties", "Category", 0)
	LINA_FIELD(DirectionalLayout::Properties, direction, "Direction", "enum", GetTypeID<DirectionOrientation>())
	LINA_FIELD(DirectionalLayout::Properties, mode, "Mode", "enum", GetTypeID<DirectionalLayout::Mode>())
	LINA_FIELD(DirectionalLayout::Properties, colorBorder, "Color Border", "Color", 0)
	LINA_FIELD(DirectionalLayout::Properties, colorBorderHovered, "Color Border Hovered", "Color", 0)
	LINA_FIELD(DirectionalLayout::Properties, borderThickness, "Border Thickness", "float", 0)
	LINA_FIELD(DirectionalLayout::Properties, borderMinSize, "Border Min Size", "float", 0)
	LINA_FIELD_LIMITS(DirectionalLayout::Properties, borderMinSize, "0.0", "1.0f", "0.1f")
	LINA_FIELD(DirectionalLayout::Properties, receiveInput, "Receive Input", "bool", 0)

	LINA_FIELD_DEPENDENCY_POS(DirectionalLayout::Properties, colorBorder, "mode", 3)
	LINA_FIELD_DEPENDENCY_POS(DirectionalLayout::Properties, colorBorderHovered, "mode", 3)
	LINA_FIELD_DEPENDENCY_POS(DirectionalLayout::Properties, borderThickness, "mode", 3)
	LINA_FIELD_DEPENDENCY_POS(DirectionalLayout::Properties, borderMinSize, "mode", 3)
	LINA_CLASS_END(DirectionalLayoutProperties)

	LINA_CLASS_BEGIN(DirectionalLayoutMode)
	LINA_PROPERTY_STRING(DirectionalLayout::Mode, "0", "Default")
	LINA_PROPERTY_STRING(DirectionalLayout::Mode, "1", "Equal Positions")
	LINA_PROPERTY_STRING(DirectionalLayout::Mode, "2", "Equal Sizes")
	LINA_PROPERTY_STRING(DirectionalLayout::Mode, "3", "Bordered")
	LINA_CLASS_END(DirectionalLayoutMode)

} // namespace Lina
