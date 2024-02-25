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

namespace Lina
{
	class DirectionalLayout : public Widget
	{
	public:
		DirectionalLayout()			 = default;
		virtual ~DirectionalLayout() = default;

		enum class Mode
		{
			Default,
			EqualPositions,
			EqualSizes,
			SpaceBetween,
		};

		struct Properties
		{
			TBLR			margins	  = {};
			float			padding	  = 0.0f;
			WidgetDirection direction = WidgetDirection::Horizontal;
			Mode			mode	  = Mode::Default;
		};

		virtual void Tick(float delta) override;

		inline void SetProps(const Properties& props)
		{
			m_props = props;
		}

		inline Properties& GetProps()
		{
			return m_props;
		}

	private:
		void BehaviourDefault(float delta);
		void BehaviourEqualPositions(float delta);
		void BehaviourEqualSizes(float delta);
		void BehaviourSpaceBetween(float delta);

		void ExpandWidgetInCrossAxis(Widget* w);
		void AlignWidgetInCrossAxis(Widget* w);

	private:
		Properties m_props	= {};
		Vector2	   m_start	= Vector2::Zero;
		Vector2	   m_end	= Vector2::Zero;
		Vector2	   m_sz		= Vector2::Zero;
		Vector2	   m_center = Vector2::Zero;
	};

} // namespace Lina
