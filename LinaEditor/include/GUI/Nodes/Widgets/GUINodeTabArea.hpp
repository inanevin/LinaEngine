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
		GUINodeTabArea(Editor* editor, ISwapchain* swapchain, int drawOrder) : GUINode(editor, swapchain, drawOrder){};
		virtual ~GUINodeTabArea() = default;

		virtual void Draw(int threadID) override;
		void		 AddTab(const String& title, StringID sid);
		void		 RemoveTab(StringID sid);

		inline GUINodeTabArea* SetCallbackTabDismissed(Delegate<void(GUINode*)>&& cb)
		{
			m_onTabDismissed = cb;
			return this;
		}

		inline GUINodeTabArea* SetCallbackTabClicked(Delegate<void(GUINode*)>&& cb)
		{
			m_onTabClicked = cb;
			return this;
		}

		inline GUINodeTabArea* SetCallbackTabDetached(Delegate<void(GUINode*, const Vector2&)>&& cb)
		{
			m_onTabDetached = cb;
			return this;
		}

		inline GUINodeTabArea* SetIsReorderEnabled(bool isEnabled)
		{
			m_isReorderEnabled = isEnabled;
			return this;
		}

		inline GUINodeTabArea* SetIsPanelTabs(bool isPanelTabs)
		{
			m_isPanelTabs = isPanelTabs;
			return this;
		}

		void OnTabClicked(GUINodeTab* tab);
		void OnTabDismissed(GUINodeTab* tab);
		void OnTabDetached(GUINodeTab* tab, const Vector2& detachDelta);

	protected:
		Delegate<void(GUINode*)>				 m_onTabClicked;
		Delegate<void(GUINode*)>				 m_onTabDismissed;
		Delegate<void(GUINode*, const Vector2&)> m_onTabDetached;
		Vector<GUINodeTab*>						 m_tabs;
		bool									 m_isReorderEnabled = true;
		bool									 m_isPanelTabs		= false;
	};

} // namespace Lina::Editor

#endif
