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

#include "Core/CommonECS.hpp"
#include "Core/SizeDefinitions.hpp"
#include "Log/Log.hpp"

#include <string>

namespace Lina
{
    namespace Graphics
    {
        class Texture;
        class Model;
        class Material;
    } // namespace Graphics
} // namespace Lina

namespace Lina::Editor
{
    struct EditorPathConstants
    {
        static std::string& contentsPath;
    };

    struct EMenuBarElementClicked
    {
        uint8 m_item;
    };

    struct EToolbarItemClicked
    {
        uint8 m_item;
    };

    struct EEntityUnselected
    {
    };
    struct EEntitySelected
    {
        ECS::Entity m_entity;
    };
    struct EMaterialTextureSelected
    {
        Graphics::Texture* m_texture;
    };
    struct ETextureReimported
    {
        Graphics::Texture* m_selected;
        Graphics::Texture* m_reimported;
    };
    struct ETextureSelected
    {
        Graphics::Texture* m_texture;
    };
    struct EModelSelected
    {
        Graphics::Model* m_model;
    };
    struct ETransformGizmoChanged
    {
        int m_currentGizmo;
    };
    struct ETransformPivotChanged
    {
        bool m_isGlobal;
    };
    struct EComponentOrderSwapped
    {
        ECS::TypeID m_id1;
        ECS::TypeID m_id2;
    };
    struct EShortcut
    {
        std::string m_name;
        int         m_heldKey;
        int         m_pressedKey;
    };

#define EDITOR_CAMERA_NAME        "Editor Camera"
#define CURSOR_X_LABELS           12
#define CURSOR_X_VALUES           ImGui::GetWindowSize().x * 0.3f
#define DRAG_POWER                0.01f
#define VALUE_OFFSET_FROM_WINDOW  10
#define COMBO_REMOVE_BUTTON_WIDTH 30
#define HEADER_WIDGET_HEIGHT      25
#define DEFAULT_TOGGLE_SIZE       ImVec2(35.0f, 15.0f)

    // GLOBAL
#define GLOBAL_FRAMEPADDING_WINDOW ImVec2(0, 6)

// HEADER PANEL
#define ID_HEADER                         "Header"
#define HEADER_FRAMEPADDING_FILEMENU      ImVec2(5, 3)
#define HEADER_FRAMEPADDING_TOPBUTTONS    ImVec2(3, 2)
#define HEADER_WINDOWPADDING_MENUBUTTON   ImVec2(7, 4)
#define HEADER_COLOR_BG                   Color(0, 0, 0, 1)
#define HEADER_COLOR_MENUBARBUTTON_ACTIVE Color(0.5f, 0.5f, 0.5f, 1.0f)
#define HEADER_RESIZE_THRESHOLD           10
#define HEADER_HEIGHT                     60
#define HEADER_OFFSET_TOPBUTTONS          80
#define TOOLBAR_HEIGHT                    30
#define FOOTER_HEIGHT                     20
#define DOCKSPACE_OFFSET                  0

// LOG PANEL
#define ID_LOG "Log"

// ECS
#define ID_ECS         "Entities"
#define ECS_MOVEENTITY "ECS_MOVE_ENTITY"

// Resources
#define ID_RESOURCES              "Resources"
#define RESOURCES_MOVETEXTURE_ID  "RSR_MOVE_TEXTURE"
#define RESOURCES_MOVEHDRI_ID     "RSR_MOVE_HDRI"
#define RESOURCES_MOVEMATERIAL_ID "RSR_MOVE_MATERIAL"
#define RESOURCES_MOVEMESH_ID     "RSR_MOVE_MESH"
#define RESOURCES_MOVESHADER_ID   "RSR_MOVE_SHADER"

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
} // namespace Lina::Editor

#endif
