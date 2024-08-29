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
	class ScrollArea;
	class DirectionalLayout;

	class Popup : public Widget
	{
	public:
		Popup()			 = default;
		virtual ~Popup() = default;

		struct Properties
		{
			float										 maxSizeY	   = Theme::GetDef().baseItemHeight * 10;
			bool										 useFixedSizeX = false;
			bool										 closeOnSelect = true;
			String										 selectedIcon  = Theme::GetDef().iconCircleFilled;
			Delegate<void(uint32 index, void* userData)> onSelectedItem;
		};

		virtual void Construct() override;
		virtual void Initialize() override;
		virtual void CalculateSize(float delta) override;
		virtual void Tick(float delta) override;
		virtual bool OnMouse(uint32 button, LinaGX::InputAction act) override;

		void AddTitleItem(const String& title, float heightMultiplier = 1.5f);
		void AddToggleItem(const String& title, bool isSelected, void* userData = nullptr, float heightMultiplier = 1.5f);
		void ScrollToItem(void* userData);
		void SwitchToggleItem(int32 item, bool on);

		inline void ScrollToItem(int32 target)
		{
			m_targetScroll = target;
		}

		inline DirectionalLayout* GetBackground() const
		{
			return m_background;
		}

		inline ScrollArea* GetScroll() const
		{
			return m_scroll;
		}
		inline Properties& GetProps()
		{
			return m_props;
		}

	private:
		Properties				   m_props			 = {};
		DirectionalLayout*		   m_background		 = nullptr;
		ScrollArea*				   m_scroll			 = nullptr;
		Tween					   m_tween			 = {};
		Vector<DirectionalLayout*> m_items			 = {};
		float					   m_maxItemWidth	 = 0.0f;
		float					   m_totalItemHeight = 0.0f;
		int32					   m_targetScroll	 = -1;
	};

	LINA_REFLECTWIDGET_BEGIN(Popup, Layout)
	LINA_REFLECTWIDGET_END(Popup)

} // namespace Lina
