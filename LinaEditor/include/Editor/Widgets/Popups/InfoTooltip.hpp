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

#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"

namespace Lina
{
	class Tween;
	class Icon;
	class Text;
} // namespace Lina
namespace Lina::Editor
{
	class InfoTooltip : public DirectionalLayout
	{
	public:
		struct TooltipProperties
		{
			String	  text		= "";
			LogLevel  level		= LogLevel::Info;
			Direction direction = Direction::Right;
			float	  time		= 5.0f;
		};

		InfoTooltip()		   = default;
		virtual ~InfoTooltip() = default;

		virtual void Construct() override;
		virtual void Initialize() override;
		virtual void Tick(float delta) override;
		virtual void Draw() override;

		inline TooltipProperties& GetTooltipProps()
		{
			return m_tooltipProps;
		}

	private:
		static constexpr float TWEEN_TIME = 0.1f;

		Color GetColorFromLevel();

	private:
		TooltipProperties m_tooltipProps  = {};
		Icon*			  m_icon		  = nullptr;
		Text*			  m_text		  = nullptr;
		float			  m_tweenValue	  = 0.0f;
		bool			  m_firstTick	  = true;
		Vector2			  m_startPosition = Vector2::Zero;
		float			  m_counter		  = 0.0f;
		Tween*			  m_tween		  = nullptr;
	};

} // namespace Lina::Editor
