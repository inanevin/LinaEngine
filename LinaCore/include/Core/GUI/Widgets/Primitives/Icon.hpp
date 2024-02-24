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

	class Icon : public Widget
	{
	public:
		Icon() : Widget(0, WF_OWNS_SIZE)
		{
		}
		virtual ~Icon() = default;

		struct Properties
		{
			String	 icon		= "";
			StringID font		= Theme::GetDef().iconFont;
			Color	 color		= Theme::GetDef().foreground0;
			Vector2	 offsetPerc = Vector2();
			float	 textScale	= 0.5f;
			bool	 isDynamic	= false;

			Color sdfOutlineColor	  = Color::White;
			float sdfThickness		  = 0.5f;
			float sdfSoftness		  = 0.05f;
			float sdfOutlineThickness = 0.0f;
			float sdfOutlineSoftness  = 0.0f;
		};

		virtual void Draw(int32 threadIndex) override;

		void CalculateIconSize();

		inline Properties& GetProps()
		{
			return m_props;
		}

		inline LinaVG::LinaVGFont* GetLVGFont()
		{
			return m_lvgFont;
		}

	private:
		Properties			   m_props				= {};
		LinaVG::SDFTextOptions m_sdfOptions			= {};
		float				   m_calculatedDPIScale = 0.0f;
		LinaVG::LinaVGFont*	   m_lvgFont			= nullptr;
	};

} // namespace Lina
