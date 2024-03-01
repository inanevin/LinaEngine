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

		struct Properties
		{
			Widget* tiedWidget = nullptr;
			bool	isSelected = false;
		};

		virtual void Construct() override;
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

		Rect	   m_selectionRect = {};
		Text*	   m_text		   = nullptr;
		Icon*	   m_icon		   = nullptr;
		Properties m_props		   = {};
		TabRow*	   m_ownerRow	   = nullptr;
	};

} // namespace Lina::Editor
