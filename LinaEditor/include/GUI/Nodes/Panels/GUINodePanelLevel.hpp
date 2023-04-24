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

#ifndef GUINodePanelLevel_HPP
#define GUINodePanelLevel_HPP

#include "GUI/Nodes/Panels/GUINodePanel.hpp"
#include "Event/ISystemEventListener.hpp"

namespace Lina
{
	class LevelManager;
	class Level;
	class GfxManager;
	class WorldRenderer;
	class ResourceManager;
} // namespace Lina

namespace Lina::Editor
{
	class GUINodeText;
	class GUINodeTextRichColors;

	class GUINodePanelLevel : public GUINodePanel, public ISystemEventListener
	{
	public:
		GUINodePanelLevel(GUIDrawerBase* drawer, int drawOrder, EditorPanel panelType, const String& title, GUINodeDockArea* parentDockArea);
		virtual ~GUINodePanelLevel();
		virtual void Draw(int threadID);
		virtual void OnSystemEvent(SystemEvent eventType, const Event& ev);

		virtual Bitmask32 GetSystemEventMask()
		{
			return EVS_LevelInstalled | EVS_LevelUninstalled;
		}

	private:
		void OnWorldRendererCreated(WorldRenderer* renderer);

	private:
		Vector2				   m_sizeWhenWorldRendererCreated = Vector2::Zero;
		WorldRenderer*		   m_worldRenderer				  = nullptr;
		ResourceManager*	   m_resourceManager			  = nullptr;
		GfxManager*			   m_gfxManager					  = nullptr;
		GUINodeText*		   m_noLevelText				  = nullptr;
		GUINodeTextRichColors* m_noLevelTextAlt				  = nullptr;
		Level*				   m_loadedLevel				  = nullptr;
		LevelManager*		   m_levelManager				  = nullptr;
	};
} // namespace Lina::Editor

#endif
