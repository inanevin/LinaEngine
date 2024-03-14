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
#include "Common/Data/Vector.hpp"

namespace Lina
{
	class Button;
	class DirectionalLayout;
} // namespace Lina

namespace Lina::Editor
{
	class IconTabs : public Widget
	{
	public:
		IconTabs()			= default;
		virtual ~IconTabs() = default;

		struct Properties
		{
			Delegate<void(int32 selected)> onSelectionChanged;
			DirectionOrientation		   direction;
			Vector<String>				   icons;
			Vector<String>				   tooltips;
			int32						   selected		  = -1;
			float						   iconScale	  = 0.5f;
			float						   topRounding	  = 0.0f;
			float						   bottomRounding = 0.0f;
		};

		virtual void Construct() override;
		virtual void Initialize() override;
		virtual void Tick(float delta) override;

		inline Properties& GetProps()
		{
			return m_props;
		}

		inline DirectionalLayout* GetLayout()
		{
			return m_layout;
		}

	private:
		void SetButtonColors(Button* btn, bool isSelected);

	private:
		DirectionalLayout* m_layout = nullptr;
		Properties		   m_props	= {};
	};

} // namespace Lina::Editor
