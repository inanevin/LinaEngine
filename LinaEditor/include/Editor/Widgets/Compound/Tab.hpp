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
#include "Common/Common.hpp"

namespace Lina
{
	class Text;
	class Icon;
	class Tween;
}; // namespace Lina

namespace Lina::Editor
{

	class TabRow;

	class Tab : public Widget
	{
	public:
		Tab() : Widget(2){};
		virtual ~Tab() = default;

		static constexpr float SELECTION_RECT_WIDTH = 2.0f;
		static constexpr float CLOSEBG_ANIM_TIME	= 0.15f;
		static constexpr float CLOSERECT_SIZEX_PERC = 0.2f;
		static constexpr float INTERP_SPEED			= 14.0f;

		struct Properties
		{
			Widget* tiedWidget		= nullptr;
			bool	isSelected		= false;
			bool	disableClose	= false;
			bool	disableMovement = false;
			float	desiredX		= 0.0f;
		};

		virtual void Construct() override;
		virtual void Destruct() override;
		virtual void Initialize() override;
		virtual void PreTick() override;
		virtual void Tick(float delta) override;
		virtual void Draw(int32 threadIndex) override;
		virtual bool OnMouse(uint32 button, LinaGX::InputAction action) override;

		inline Properties& GetProps()
		{
			return m_props;
		}

		inline Text* GetText() const
		{
			return m_text;
		}

		inline Icon* GetIcon() const
		{
			return m_icon;
		}

	private:
		friend class TabRow;

		Text*	   m_text					= nullptr;
		Icon*	   m_icon					= nullptr;
		Properties m_props					= {};
		TabRow*	   m_ownerRow				= nullptr;
		Tween*	   m_closeRectAnim			= nullptr;
		Tween*	   m_selectionRectAnim		= nullptr;
		Rect	   m_selectionRect			= {};
		Rect	   m_closeRect				= {};
		float	   m_closeRectAnimValue		= 0.0f;
		float	   m_selectionRectAnimValue = 0.0f;
		bool	   m_wasSelected			= false;
		bool	   m_closeRectHovered		= false;
		bool	   m_closeRectPressed		= false;
		Vector2	   m_offsetAtPress			= Vector2::Zero;
		uint32	   m_indexInParent			= 0;
	};

} // namespace Lina::Editor