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

#ifndef GUINodeTopPanel_HPP
#define GUINodeTopPanel_HPP

#include "GUI/Nodes/GUINode.hpp"
#include "Event/ISystemEventListener.hpp"

namespace Lina::Editor
{
	class GUINodeFileMenu;
	class GUINodeFMPopupElement;
	class GUINodeWindowButtons;
	class GUINodeCustomLogo;

	class GUINodeTopPanel : public GUINode, public ISystemEventListener
	{
	public:
		GUINodeTopPanel(GUIDrawerBase* drawer, int drawOrder);
		virtual ~GUINodeTopPanel();

		virtual void Draw(int threadID) override;
		virtual void OnSystemEvent(SystemEvent eventType, const Event& ev);

		virtual Bitmask32 GetSystemEventMask()
		{
			return EVS_LevelInstalled | EVS_LevelUninstalled;
		}

	private:
		void OnPressedItem(GUINode* node);

	private:
		GUINodeFileMenu*	  m_fileMenu	  = nullptr;
		GUINodeWindowButtons* m_windowButtons = nullptr;
		GUINodeCustomLogo*	  m_customLogo	  = nullptr;

		GUINodeFMPopupElement* m_elementSaveLevel			  = nullptr;
		GUINodeFMPopupElement* m_elementSaveLevelAs			  = nullptr;
		GUINodeFMPopupElement* m_elementUninstallCurrentLevel = nullptr;
	};
} // namespace Lina::Editor

#endif
