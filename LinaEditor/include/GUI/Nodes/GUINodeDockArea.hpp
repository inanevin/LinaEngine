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

#ifndef GUINodeDockArea_HPP
#define GUINodeDockArea_HPP

#include "GUINode.hpp"
#include "Data/Vector.hpp"
#include "Math/Rect.hpp"
#include "Core/EditorCommon.hpp"

namespace Lina
{
	class Input;
}

namespace Lina::Editor
{
	class GUINodePanel;
	class GUIDrawerBase;
	class GUINodeTabArea;
	class GUINodeDockPreview;

	class GUINodeDockArea : public GUINode
	{
	public:
		GUINodeDockArea(GUIDrawerBase* drawer, Editor* editor, ISwapchain* swapchain, int drawOrder);
		virtual ~GUINodeDockArea() = default;

		virtual void Draw(int threadID) override;
		void		 AddPanel(GUINodePanel* panel);
		void		 RemovePanel(GUINodePanel* panel, bool deletePanel = true, bool deleteIfEmpty = true);
		virtual void OnPayloadCreated(PayloadType type, void* payloadData) override;
		virtual bool OnPayloadDropped(PayloadType type, void* payloadData) override;

		inline void SetSplitRect(const Rect& rect)
		{
			m_splitPercentages = rect;
		}

		inline const Rect& GetSplitRect() const
		{
			return m_splitPercentages;
		}

		inline void SetSplittedArea(GUINodeDockArea* area)
		{
			m_splittedArea = area;
		}

		inline GUINodeDockArea* GetSplittedArea()
		{
			return m_splittedArea;
		}

	private:
		void OnTabClicked(GUINode* node);
		void OnTabDismissed(GUINode* node);
		void OnTabDetached(GUINode* node, const Vector2& detachDelta);

	protected:
		Lina::Input*		  m_input					= nullptr;
		bool				  m_isDockingPreviewEnabled = false;
		GUIDrawerBase*		  m_drawer					= nullptr;
		Rect				  m_splitPercentages;
		Vector<GUINodePanel*> m_panels;
		GUINodePanel*		  m_focusedPanel = nullptr;
		GUINodeDockArea*	  m_splittedArea = nullptr;
		GUINodeTabArea*		  m_tabArea		 = nullptr;
		GUINodeDockPreview*	  m_dockPreview	 = nullptr;
	};
} // namespace Lina::Editor

#endif
