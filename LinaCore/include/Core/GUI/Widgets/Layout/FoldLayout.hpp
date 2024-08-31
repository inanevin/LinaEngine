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
#include "Common/Tween/Tween.hpp"

namespace Lina
{
	class FoldLayout : public Widget
	{
	public:
		FoldLayout() : Widget(WF_SIZE_AFTER_CHILDREN){};
		virtual ~FoldLayout() = default;

		struct Properties
		{
			Delegate<void(bool)> onFoldChanged;
			bool				 doubleClickChangesFold = true;
			bool				 lookForChevron			= true;
			float				 marginIncrease			= Theme::GetDef().baseIndentInner;
			bool				 useTween				= false;
			float				 tweenPower				= 6.0f;
			float				 tweenDuration			= 0.25f;
		};

		virtual void CalculateSize(float delta) override;
		virtual void Tick(float delta) override;
		virtual bool OnMouse(uint32 button, LinaGX::InputAction act) override;
		void		 SetIsUnfolded(bool unfolded);

		inline bool GetIsUnfolded() const
		{
			return m_unfolded;
		}

		inline Properties& GetProps()
		{
			return m_props;
		}

	private:
		static constexpr float FOLD_SPEED = 12.0f;

		Properties m_props	  = {};
		bool	   m_unfolded = false;
		Tween	   m_tween	  = {};
	};

	LINA_REFLECTWIDGET_BEGIN(FoldLayout, Layout)
	LINA_REFLECTWIDGET_END(FoldLayout)

} // namespace Lina
