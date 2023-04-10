/*
Class: EditorCommon

General purpose macros for LinaEditor namespace.

Timestamp: 5/8/2020 11:04:45 PM

*/
#pragma once

#ifndef EditorCommon_HPP
#define EditorCommon_HPP

#include "Data/String.hpp"

namespace Lina::Editor
{
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
	};

	enum class DockSplitType
	{
		None,
		Left,
		Right,
		Down,
		Up
	};

#define DEFAULT_DOCK_SPLIT			0.35f
#define EDITOR_IMAGES_SHEET_COLUMNS 5
#define EDITOR_IMAGES_SHEET_ROWS	1
#define EDITOR_IMAGE_DOCK_DOWN		0
#define EDITOR_IMAGE_DOCK_LEFT		1
#define EDITOR_IMAGE_DOCK_RIGHT		2
#define EDITOR_IMAGE_DOCK_UP		3
#define EDITOR_IMAGE_CROSS			4
} // namespace Lina::Editor

#endif
