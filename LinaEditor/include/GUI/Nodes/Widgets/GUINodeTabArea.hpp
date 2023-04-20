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

#ifndef GUINodeTabArea_HPP
#define GUINodeTabArea_HPP

#include "GUI/Nodes/GUINode.hpp"
#include "Data/Vector.hpp"
#include "Data/String.hpp"
#include "Data/Functional.hpp"

namespace Lina::Editor
{
	class GUINodeButton;
	class GUINodeTab;

	class GUINodeTabArea : public GUINode
	{
	public:
		GUINodeTabArea(GUIDrawerBase* drawer, int drawOrder) : GUINode(drawer, drawOrder){};
		virtual ~GUINodeTabArea() = default;

		virtual void Draw(int threadID) override;
		void		 AddTab(const String& title, StringID sid);
		void		 RemoveTab(StringID sid);
		void		 OnTabClicked(GUINodeTab* tab);
		void		 OnTabDismissed(GUINodeTab* tab);
		void		 OnTabDetached(GUINodeTab* tab, const Vector2& detachDelta);

		inline void SetFocusedTab(StringID sid)
		{
			m_focusedSID = sid;
		}

		inline void SetCallbackTabDismissed(Delegate<void(GUINodeTab*)>&& cb)
		{
			m_onTabDismissed = cb;
		}

		inline void SetCallbackTabClicked(Delegate<void(GUINodeTab*)>&& cb)
		{
			m_onTabClicked = cb;
		}

		inline void SetCallbackTabDetached(Delegate<void(GUINodeTab*, const Vector2&)>&& cb)
		{
			m_onTabDetached = cb;
		}

		inline void SetIsReorderEnabled(bool isEnabled)
		{
			m_isReorderEnabled = isEnabled;
		}

		inline void SetIsPanelTabs(bool isPanelTabs)
		{
			m_isPanelTabs = isPanelTabs;
		}

		inline void SetCanClosePanels(bool canClose)
		{
			m_canClosePanels = canClose;
		}

		inline bool GetIsEmpty()
		{
			return m_tabs.empty();
		}

	protected:
		StringID									m_focusedSID	 = 0;
		bool										m_canClosePanels = false;
		Delegate<void(GUINodeTab*)>					m_onTabClicked;
		Delegate<void(GUINodeTab*)>					m_onTabDismissed;
		Delegate<void(GUINodeTab*, const Vector2&)> m_onTabDetached;
		Vector<GUINodeTab*>							m_tabs;
		bool										m_isReorderEnabled = true;
		bool										m_isPanelTabs	   = false;
	};

} // namespace Lina::Editor

#endif
