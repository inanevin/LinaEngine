/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

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

/*
Class: EditorCommon

General purpose macros for LinaEditor namespace.

Timestamp: 5/8/2020 11:04:45 PM

*/
#pragma once

#ifndef EditorCommon_HPP
#define EditorCommon_HPP

#include "Utility/EditorUtility.hpp"
#include "Log/Log.hpp"
#include "ECS/ECS.hpp"
#include <string>

namespace Lina
{
	namespace Graphics
	{
		class Texture;
		class Model;
		class Material;
	}


}
namespace Lina::Editor
{
	struct EditorPathConstants
	{
		static std::string& contentsPath;
	};

	enum class MenuBarItems
	{
		NewProject = 0,
		LoadProject = 1,
		SaveProject = 2,
		PackageProject = 3,

		Edit = 10,
		View = 20,

		SaveLevelData = 30,
		LoadLevelData = 31,
		NewLevelData = 32,

		ECSPanel = 40,
		HeaderPanel = 11,
		LogPanel = 42,
		PropertiesPanel = 44,
		ResourcesPanel = 45,
		ScenePanel = 46,
		ProfilerPanel = 47,
		GlobalSettingsPanel = 48,
		ImGuiPanel = 50,

		DebugViewPhysics = 61,
		DebugViewShadows = 62,
		DebugViewNormal = 63,

		Cube = 80,
		Sphere = 81,
		Capsule = 82,
		Cylinder = 83,
		Plane = 84,
		Quad = 85

	};

	struct EMenuBarItemClicked { MenuBarItems m_item; };
	struct EEntityUnselected { };
	struct EEntitySelected { ECS::Entity m_entity; };
	struct EMaterialTextureSelected { Graphics::Texture* m_texture; };
	struct ETextureReimported { Graphics::Texture* m_selected; Graphics::Texture* m_reimported; };
	struct ETextureSelected { Graphics::Texture* m_texture; };
	struct EMaterialSelected { EditorFile* m_file; Graphics::Material* m_material; };
	struct EModelSelected { Graphics::Model* m_model; };
	struct ETransformGizmoChanged { int m_currentGizmo; };
	struct ETransformPivotChanged { bool m_isGlobal; };
	struct EComponentOrderSwapped { ECS::TypeID m_id1; ECS::TypeID m_id2; };

#define EDITOR_CAMERA_NAME "Editor Camera"

	// GLOBAL
#define GLOBAL_FRAMEPADDING_WINDOW ImVec2(0,6)


// HEADER PANEL
#define ID_HEADER "Header"
#define HEADER_FRAMEPADDING_FILEMENU ImVec2(5,3)
#define HEADER_FRAMEPADDING_TOPBUTTONS ImVec2(3,2)
#define HEADER_WINDOWPADDING_MENUBUTTON ImVec2(7,4)
#define HEADER_COLOR_BG Lina::Color(0, 0, 0, 1)
#define HEADER_COLOR_TOPBUTTONS Lina::Color(1, 1, 1, 1)
#define HEADER_COLOR_MENUBARBUTTON_ACTIVE Lina::Color(0.5f, 0.5f, 0.5f, 1.0f)
#define HEADER_RESIZE_THRESHOLD 10
#define HEADER_HEIGHT 60
#define HEADER_OFFSET_TOPBUTTONS 80
#define HEADER_LINALOGO_ANIMSIZE 132
#define HEADER_LINALOGO_SIZE ImVec2(180, 29)
#define TOOLBAR_HEIGHT 30
#define FOOTER_HEIGHT 20
#define DOCKSPACE_OFFSET 0

// LOG PANEL
#define ID_LOG "Log"
#define LOGPANEL_ICONSENABLED false
#define LOGPANEL_COLOR_ICONDEFAULT ImVec4(1.0f, 1.0f, 1.0f, 0.5f);
#define LOGPANEL_COLOR_ICONHOVERED ImVec4(1.0f, 1.0f, 1.0f, 0.8f);
#define LOGPANEL_COLOR_ICONPRESSED  ImVec4(1.0f, 1.0f, 1.0f, .4f);

#define LOGPANEL_COLOR_DEBUG_DEFAULT ImVec4(0.0f, 0.6f, 0.0f, 1.0f) 
#define LOGPANEL_COLOR_DEBUG_HOVERED ImVec4(0.0f, 0.8f, 0.0f, 1.0f)
#define LOGPANEL_COLOR_DEBUG_PRESSED ImVec4(0.0f, 0.4f, 0.0f, 1.0f)

#define LOGPANEL_COLOR_INFO_DEFAULT ImVec4(0.8f, 0.8f, 0.8f, 1.0f) 
#define LOGPANEL_COLOR_INFO_HOVERED ImVec4(0.9f, 0.9f, 0.9f, 1.0f)
#define LOGPANEL_COLOR_INFO_PRESSED ImVec4(1.0f, 1.0f, 1.0f, 1.0f)

#define LOGPANEL_COLOR_TRACE_DEFAULT ImVec4(0.6f, 0.6f, 0.8f, 1.0f) 
#define LOGPANEL_COLOR_TRACE_HOVERED ImVec4(0.0f, 0.6f, 1.0f, 1.0f)
#define LOGPANEL_COLOR_TRACE_PRESSED ImVec4(0.0f, 0.2f, 0.4f, 1.0f)

#define LOGPANEL_COLOR_WARN_DEFAULT ImVec4(0.6f, 0.6f, 0.0f, 1.0f) 
#define LOGPANEL_COLOR_WARN_HOVERED ImVec4(0.8f, 0.8f, 0.0f, 1.0f)
#define LOGPANEL_COLOR_WARN_PRESSED ImVec4(0.4f, 0.4f, 0.0f, 1.0f)

#define LOGPANEL_COLOR_ERR_DEFAULT ImVec4(0.8f, 0.0f, 0.0f, 1.0f) 
#define LOGPANEL_COLOR_ERR_HOVERED ImVec4(1.0f, 0.0f, 0.0f, 1.0f)
#define LOGPANEL_COLOR_ERR_PRESSED ImVec4(0.6f, 0.0f, 0.0f, 1.0f)

#define LOGPANEL_COLOR_CRIT_DEFAULT ImVec4(0.0f, 0.0f, 0.0f, 1.0f) 
#define LOGPANEL_COLOR_CRIT_HOVERED ImVec4(0.3f, 0.3f, 0.3f, 1.0f)
#define LOGPANEL_COLOR_CRIT_PRESSED ImVec4(0.0f, 0.0f, 0.0f, 1.0f)

// ECS
#define ID_ECS "Entities"
#define ECS_MOVEENTITY "ECS_MOVE_ENTITY"

// Resources
#define ID_RESOURCES "Resources"
#define RESOURCES_MOVETEXTURE_ID "RSR_MOVE_TEXTURE"
#define RESOURCES_MOVEHDRI_ID "RSR_MOVE_HDRI"
#define RESOURCES_MOVEMATERIAL_ID "RSR_MOVE_MATERIAL"
#define RESOURCES_MOVEMESH_ID "RSR_MOVE_MESH"
#define RESOURCES_MOVESHADER_ID "RSR_MOVE_SHADER"

// Profiler
#define ID_PROFILER "Profiler"

// Global
#define ID_GLOBAL "Global"

// Properties
#define ID_PROPERTIES "Properties"

// Scene
#define ID_SCENE "Scene"

// Toolbar
#define ID_TOOLBAR "Toolbar"
}



#endif
