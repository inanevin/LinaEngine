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
#include "Common/Tween/Tween.hpp"

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
			float	size	   = 0.0f;
			bool	isHovered  = false;
			float	expand	   = 0.0f;
			Vector2 position   = Vector2::Zero;
			Vector2 extraPos   = Vector2::Zero;
			Vector2 direction  = Vector2::Zero;
			Icon*	icon	   = nullptr;
			bool	isDisabled = false;
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
		virtual void Draw() override;
		void		 GetHoveredDirection(Direction& outDirection, bool& outIsHovered);
		void		 SetDirectionDisabled(const Direction& dir, bool disabled);

		inline Properties& GetProps()
		{
			return m_props;
		}

	private:
		void DrawDockRect(const DockRect& dr);

		static constexpr std::string_view DIR_TO_ICON[5] = {ICON_ARROW_RECT_UP, ICON_ARROW_RECT_DOWN, ICON_ARROW_RECT_LEFT, ICON_ARROW_RECT_RIGHT, ICON_RECT_FILLED};

	private:
		Properties m_props = {};
		DockRect   m_dockRects[5];
		Tween	   m_dockPreviewTween;
		float	   m_smallRectSize = 0.0f;
	};

	LINA_REFLECTWIDGET_BEGIN(DockPreview)
	LINA_REFLECTWIDGET_END(DockPreview)

} // namespace Lina::Editor
