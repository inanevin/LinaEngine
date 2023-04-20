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

#ifndef EditorCommon_HPP
#define EditorCommon_HPP

#include "Data/String.hpp"

namespace Lina::Editor
{
	class GUINodePanel;
	class GUINodeDockArea;
	class GUIDrawerBase;

	struct EShortcut
	{
		String name		  = "";
		int	   heldKey	  = 0;
		int	   pressedKey = 0;
	};

	enum class EditorPanel
	{
		None,
		Entities,
		Level,
		Properties,
		ContentBrowser,
		Hierarchy,
		DebugResourceView,
		TextureViewer,
		ModelViewer,
	};

	enum class DockSplitType
	{
		None,
		Left,
		Right,
		Down,
		Up,
		Tab
	};

	enum class Direction
	{
		None,
		Horizontal,
		Vertical
	};

	const HashMap<EditorPanel, String> PANEL_TO_NAME_MAP = {{EditorPanel::Entities, "Entities"},
															{EditorPanel::Level, "Level"},
															{EditorPanel::Properties, "Properties"},
															{EditorPanel::ContentBrowser, "Content Browser"},
															{EditorPanel::Hierarchy, "Hierarchy"},
															{EditorPanel::ModelViewer, "Model Viewer"},
															{EditorPanel::TextureViewer, "Texture Viewer"},
															{EditorPanel::DebugResourceView, "Debug Resource View"}};

	enum class PayloadType
	{
		None,
	};

	struct PayloadDataPanel
	{
		GUINodePanel*	 srcPanel	   = nullptr;
		GUINodePanel*	 onFlightPanel = nullptr;
		GUINodeDockArea* ownerDockArea = nullptr;
	};

#define EDITOR_DEFAULT_DOCK_SPLIT		0.35f
#define EDITOR_DEFAULT_DOCK_SPLIT_OUTER 0.2f
#define EDITOR_IMAGES_SHEET_COLUMNS		8
#define EDITOR_IMAGES_SHEET_ROWS		2
#define EDITOR_PAYLOAD_WINDOW_SID		"EditorPaylodWindow"_hs

#define EDITOR_IMAGE_DOCK_DOWN		  3
#define EDITOR_IMAGE_DOCK_LEFT		  0
#define EDITOR_IMAGE_DOCK_RIGHT		  1
#define EDITOR_IMAGE_DOCK_UP		  2
#define EDITOR_IMAGE_CROSS			  4
#define EDITOR_IMAGE_DOCK_OUTER_LEFT  5
#define EDITOR_IMAGE_DOCK_OUTER_UP	  6
#define EDITOR_IMAGE_DOCK_OUTER_RIGHT 7
#define EDITOR_IMAGE_DOCK_OUTER_DOWN  8
#define EDITOR_IMAGE_DOCK_CENTER	  9

} // namespace Lina::Editor

#endif
