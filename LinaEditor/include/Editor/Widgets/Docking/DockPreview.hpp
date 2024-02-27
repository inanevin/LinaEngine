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
#include "Editor/CommonEditor.hpp"

namespace Lina
{
	class Icon;
	class Tween;
} // namespace Lina

namespace Lina::Editor
{
	class DockPreview : public Widget
	{

	private:
		struct DockRect
		{
			float	size	  = 0.0f;
			bool	isHovered = false;
			float	expand	  = 0.0f;
			Vector2 position  = Vector2::Zero;
			Vector2 extraPos  = Vector2::Zero;
			Vector2 direction = Vector2::Zero;
			Icon*	icon	  = nullptr;
		};

	public:
		DockPreview()		   = default;
		virtual ~DockPreview() = default;

		struct Properties
		{
			bool isCentral = false;
		};
		static constexpr float ANIM_TIME		= 0.15f;
		static constexpr float BOUNCE_ANIM_TIME = 0.75f;
		static constexpr float SMALL_RECT_SZ	= 42.0f;

		virtual void Construct() override;
		virtual void Initialize() override;
		virtual void Tick(float delta) override;
		virtual void Draw(int32 threadIndex) override;
		void		 GetHoveredDirection(DockDirection& outDirection, bool& outIsHovered);

		inline Properties& GetProps()
		{
			return m_props;
		}

	private:
		void DrawDockRect(int32 threadIndex, const DockRect& dr);

	private:
		Properties m_props = {};
		DockRect   m_dockRects[5];
		float	   m_animationAlpha = 0.0f;
		float	   m_smallRectSize	= 0.0f;
	};

} // namespace Lina::Editor
