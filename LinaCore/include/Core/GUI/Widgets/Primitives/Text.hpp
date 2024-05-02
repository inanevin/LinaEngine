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
#include "Common/Platform/LinaVGIncl.hpp"

namespace Lina
{
	class Font;

	class Text : public Widget
	{
	public:
		Text() : Widget(WF_OWNS_SIZE){};
		virtual ~Text() = default;

		struct Properties
		{
			Delegate<void()>	  onClicked;
			String				  text						= "";
			StringID			  font						= Theme::GetDef().defaultFont;
			Color				  color						= Theme::GetDef().foreground0;
			Color				  colorDisabled				= Theme::GetDef().silent1;
			LinaVG::TextAlignment alignment					= LinaVG::TextAlignment::Left;
			Vector4				  customClip				= Vector4::Zero;
			float				  textScale					= 1.0f;
			float				  wrapWidth					= 0.0f;
			bool				  isDynamic					= false;
			bool				  fetchCustomClipFromParent = false;
			bool				  fetchCustomClipFromSelf	= false;
			bool				  fetchWrapFromParent		= false;
			bool				  wordWrap					= true;
		};

		virtual void Initialize() override;
		virtual void CalculateSize(float delta) override;
		virtual void Draw() override;
		virtual bool OnMouse(uint32 button, LinaGX::InputAction act) override;

		void CalculateTextSize();

		inline Properties& GetProps()
		{
			return m_props;
		}

		inline LinaVG::LinaVGFont* GetLVGFont()
		{
			return m_lvgFont;
		}

	private:
		Properties			m_props				 = {};
		float				m_calculatedDPIScale = 0.0f;
		LinaVG::LinaVGFont* m_lvgFont			 = nullptr;
		bool				m_isSDF				 = false;
	};

} // namespace Lina
