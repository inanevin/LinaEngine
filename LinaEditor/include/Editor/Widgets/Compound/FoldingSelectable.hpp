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

namespace Lina
{
	class Text;
	class Icon;
	class DirectionalLayout;
} // namespace Lina

namespace Lina::Editor
{
	class FoldingSelectable : public Widget
	{
	public:
		FoldingSelectable() : Widget(-1, WF_SELECTABLE){};
		virtual ~FoldingSelectable() = default;

		struct Properties
		{

			Color				 colorSelectedEnd			= Theme::GetDef().accentPrimary0;
			Color				 colorSelectedStart			= Theme::GetDef().accentPrimary1;
			Color				 colorSelectedInactiveStart = Theme::GetDef().silent0;
			Color				 colorSelectedInactiveEnd	= Theme::GetDef().silent1;
			Delegate<void(bool)> onFoldChanged;
			uint8				 level	= 0;
			Widget*				 owner	= nullptr;
			float				 height = Theme::GetDef().baseItemHeight;
		};

		virtual void Construct() override;
		virtual void Initialize() override;
		virtual void CalculateSize(float dt) override;
		virtual void Tick(float dt) override;
		virtual void Draw(int32 threadIndex) override;
		virtual bool OnMouse(uint32 button, LinaGX::InputAction act) override;
		virtual bool OnKey(uint32 keycode, int32 scancode, LinaGX::InputAction act) override;
		void		 ChangeFold(bool folded);

		inline Properties& GetProps()
		{
			return m_props;
		}

		inline DirectionalLayout* GetLayout() const
		{
			return m_layout;
		}

		inline bool GetIsFolded() const
		{
			return m_folded;
		}

	private:
		static constexpr float COLOR_SPEED = 15.0f;

	private:
		Properties m_props = {};

		Color			   m_usedColorStart = Color(0, 0, 0, 0);
		Color			   m_usedColorEnd	= Color(0, 0, 0, 0);
		DirectionalLayout* m_layout			= nullptr;
		bool			   m_selected		= false;
		bool			   m_folded			= true;
	};

} // namespace Lina::Editor
