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
#include "Common/Tween/Tween.hpp"

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
		Tab()		   = default;
		virtual ~Tab() = default;

		static constexpr float SELECTION_RECT_WIDTH = 2.0f;
		static constexpr float SELECTION_ANIM_TIME	= 0.15f;
		static constexpr float INTERP_SPEED			= 14.0f;

		struct Properties
		{
			bool   isSelected	   = false;
			bool   disableMovement = false;
			float  desiredX		   = 0.0f;
			String title		   = "";
		};

		virtual void Construct() override;
		virtual void Initialize() override;
		virtual void CalculateSize(float delta) override;
		virtual void Tick(float delta) override;
		virtual void Draw() override;
		virtual bool OnMouse(uint32 button, LinaGX::InputAction action) override;
		virtual bool OnMousePos(const Vector2& pos) override;
		void		 DisableClosing(bool disabled);

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

		Text*	   m_text	  = nullptr;
		Icon*	   m_icon	  = nullptr;
		Properties m_props	  = {};
		TabRow*	   m_ownerRow = nullptr;
		Tween	   m_selectionRectAnim;
		Rect	   m_selectionRect	= {};
		Rect	   m_closeRect		= {};
		bool	   m_wasSelected	= false;
		Vector2	   m_offsetAtPress	= Vector2::Zero;
		uint32	   m_indexInParent	= 0;
		float	   m_alpha			= 0.0f;
		bool	   m_requestedClose = false;
		bool	   m_requestDockOut = false;
	};

} // namespace Lina::Editor
