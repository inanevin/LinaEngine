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
#include "Utility/StringId.hpp"

#include <Data/String.hpp>

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
        static String& contentsPath;
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
        int m_currentOperation;
    };
    struct ETransformPivotChanged
    {
        bool m_isGlobal;
    };
    struct EComponentOrderSwapped
    {
        TypeID m_id1;
        TypeID m_id2;
    };
    struct EShortcut
    {
        String m_name;
        int         m_heldKey;
        int         m_pressedKey;
    };
    struct EGUILayerRender
    {
    };

    enum class ResourceSelectorType
    {
        None,
        Texture,
        Shader,
        Material,
        Model,
        Audio,
        PhysicsMaterial,
    };

#define EDITOR_CAMERA_NAME        "Editor Camera"
#define CURSOR_X_LABELS           12 * GUILayer::Get()->GetDPIScale()
#define CURSOR_X_VALUES           ImGui::GetWindowSize().x * 0.3f
#define DRAG_POWER                0.01f
#define VALUE_OFFSET_FROM_WINDOW  10 * GUILayer::Get()->GetDPIScale()
#define COMBO_REMOVE_BUTTON_WIDTH 30
#define HEADER_WIDGET_HEIGHT      25 * GUILayer::Get()->GetDPIScale()
#define DEFAULT_TOGGLE_SIZE       ImVec2(35.0f * GUILayer::Get()->GetDPIScale(), 15.0f * GUILayer::Get()->GetDPIScale())

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

// Entities
#define ID_ENTITIES    "Entities"
#define ECS_MOVEENTITY "ECS_MOVE_ENTITY"

// Systems
#define ID_SYSTEMS "Systems"

// Resources
#define ID_RESOURCES              "Resources"
#define RESOURCES_MOVETEXTURE_ID  "RSR_MOVE_TEXTURE"
#define RESOURCES_MOVEHDRI_ID     "RSR_MOVE_HDRI"
#define RESOURCES_MOVEMATERIAL_ID "RSR_MOVE_MATERIAL"
#define RESOURCES_MOVEMODEL_ID    "RSR_MOVE_MESH"
#define RESOURCES_MOVESHADER_ID   "RSR_MOVE_SHADER"
#define RESOURCES_MOVEAUDIO_ID    "RSR_MOVE_AUDIO"
#define RESOURCES_MOVEPHYMAT_ID   "RSR_MOVE_PHYMAT"

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

// Resource Selector
#define ID_RESOURCESELECTOR "ResourceSelector"

// Progress panel
#define ID_PROGRESSPANEL "ProgressPanel"

// Preview panel
#define ID_PREVIEWPANEL "PreviewPanel"

// Text Editor Panel
#define ID_TEXTEDITOR "Text Editor"
} // namespace Lina::Editor

#endif
