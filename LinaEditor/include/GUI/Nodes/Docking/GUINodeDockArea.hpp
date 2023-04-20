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

#include "GUI/Nodes/Layouts/GUINodeDivisible.hpp"
#include "Data/Vector.hpp"
#include "Data/HashMap.hpp"
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
	class GUINodeTab;

	struct DismissTabData
	{
		GUINodeTab* tab		 = nullptr;
		bool		isDetach = false;
	};

	class GUINodeDockArea : public GUINodeDivisible
	{
	public:
		GUINodeDockArea(GUIDrawerBase* drawer, int drawOrder);
		virtual ~GUINodeDockArea() = default;

		virtual void Draw(int threadID) override;
		void		 AddPanel(GUINodePanel* panel);
		void		 RemovePanel(GUINodePanel* panel);
		void		 SetDockPreviewEnabled(bool enabled);

		inline GUINodeDockPreview* GetDockPreview() const
		{
			return m_dockPreview;
		}

		inline GUINodePanel* GetFirstPanel() const
		{
			return m_panels[0];
		}

		inline bool GetIsAlone() const
		{
			return m_isAlone;
		}

		void SetIsAlone(bool isAlone)
		{
			m_isAlone = isAlone;
		}

	private:
		void OnTabClicked(GUINodeTab* node);
		void OnTabDismissed(GUINodeTab* node);
		void OnTabDetached(GUINodeTab* node, const Vector2& detachDelta);

	protected:
		Vector<DismissTabData> m_dismissedTabs;
		bool				   m_isAlone				 = true;
		bool				   m_isDockingPreviewEnabled = false;
		Lina::Input*		   m_input					 = nullptr;
		Vector<GUINodePanel*>  m_panels;
		GUINodePanel*		   m_focusedPanel = nullptr;
		GUINodeTabArea*		   m_tabArea	  = nullptr;
		GUINodeDockPreview*	   m_dockPreview  = nullptr;
	};
} // namespace Lina::Editor

#endif
