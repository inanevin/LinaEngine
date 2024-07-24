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
		};

		enum class BackgroundStyle
		{
			None,
			Default,
			CentralGradient
		};

		struct Properties
		{
			DirectionOrientation direction = DirectionOrientation::Horizontal;
			Mode				 mode	   = Mode::Default;
			Delegate<void()>	 onClicked;
			Delegate<void()>	 onDoubleClicked;
			Delegate<void()>	 onRightClicked;
			Delegate<void()>	 onPressed;
			Delegate<void()>	 onDestructed;
			Delegate<void()>	 onHoverBegin;
			Delegate<void()>	 onHoverEnd;
			bool				 receiveInput = false;
		};

		virtual void  Destruct() override;
		virtual void  Initialize() override;
		virtual void  PreTick() override;
		virtual void  Tick(float delta) override;
		virtual void  Draw() override;
		virtual void  DebugDraw(int32 drawOrder) override;
		virtual bool  OnMouse(uint32 button, LinaGX::InputAction act) override;
		virtual float CalculateChildrenSize() override;

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

	protected:
		Properties m_props = {};

	private:
		Vector2 m_start			  = Vector2::Zero;
		Vector2 m_end			  = Vector2::Zero;
		Vector2 m_sz			  = Vector2::Zero;
		Vector2 m_center		  = Vector2::Zero;
		bool	m_lastHoverStatus = false;
		float	m_animValue		  = 0.0f;
	};

	LINA_REFLECTWIDGET_BEGIN(DirectionalLayout)
	LINA_REFLECTWIDGET_END(DirectionalLayout)

} // namespace Lina
