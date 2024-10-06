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
#include "Common/Data/String.hpp"
#include "Common/Tween/Tween.hpp"

namespace Lina
{
	class Texture;
}

namespace Lina::Editor
{
	class LinaLoading : public Widget
	{
	public:
		struct Props
		{
			float tweenTime		= 0.15f;
			float waitTime		= 0.1f;
			float dispersePower = 0.2f;
		};

		LinaLoading()		   = default;
		virtual ~LinaLoading() = default;

		virtual void Construct() override;
		virtual void Tick(float delta) override;
		virtual void Draw() override;

		inline Props& GetProps()
		{
			return m_props;
		}

	private:
		int32 m_action = -1;
		Tween m_tween;

		Props	 m_props		= {};
		Vector2	 m_leftOffset	= Vector2::Zero;
		Vector2	 m_rightOffset	= Vector2::Zero;
		Vector2	 m_bottomOffset = Vector2::Zero;
		Color	 m_rightTint	= Color::White;
		Color	 m_bottomTint	= Color::White;
		Color	 m_leftTint		= Color::White;
		Texture* m_imgLeft		= nullptr;
		Texture* m_imgRight		= nullptr;
		Texture* m_imgBottom	= nullptr;
	};
	LINA_WIDGET_BEGIN(LinaLoading, Hidden)
	LINA_CLASS_END(LinaLoading)
} // namespace Lina::Editor
