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

namespace Lina::Editor
{
	class Panel : public Widget
	{
	public:
		Panel() = delete;
		Panel(PanelType type, StringID subData) : m_panelType(type), m_subData(subData), Widget(WF_CONTROLS_MANAGER){};
		virtual ~Panel() = default;

		virtual void			 Draw(int32 threadIndex) override;
		virtual PanelLayoutExtra GetExtraLayoutData()
		{
			return {};
		};
		virtual void SetExtraLayoutData(const PanelLayoutExtra& data){};

		inline PanelType GetType() const
		{
			return m_panelType;
		}

		inline StringID GetSubData() const
		{
			return m_subData;
		}

	private:
		PanelType m_panelType = PanelType::Resources;
		StringID  m_subData	  = 0;
	};

} // namespace Lina::Editor
