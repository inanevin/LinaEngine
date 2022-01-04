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

#ifdef LINA_PLATFORM_WINDOWS
#include <windows.h>
#include <shellapi.h>
#endif
#include "Core/GUILayer.hpp"
#include "Core/CentralDockingSpace.hpp"
#include "Core/Application.hpp"
#include "Core/CustomFontIcons.hpp"
#include "Core/EditorApplication.hpp"
#include "Core/EditorCommon.hpp"
#include "Core/Engine.hpp"
#include "Core/InputBackend.hpp"
#include "Core/PhysicsBackend.hpp"
#include "Core/RenderBackendFwd.hpp"
#include "ECS/Components/LightComponent.hpp"
#include "EventSystem/GraphicsEvents.hpp"
#include "EventSystem/ResourceEvents.hpp"
#include "Helpers/DrawParameterHelper.hpp"
#include "IconsFontAwesome5.h"
#include "IconsForkAwesome.h"
#include "IconsMaterialDesign.h"
#include "Log/Log.hpp"
#include "Resources/ResourceStorage.hpp"
#include "Utility/EditorUtility.hpp"
#include "Utility/UtilityFunctions.hpp"
#include "Widgets/MenuButton.hpp"
#include "Widgets/WidgetsUtility.hpp"

#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include <glad/glad.h>

// AFTER GLAD
#include <GLFW/glfw3.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/implot/implot.h"

static bool        s_showIMGUIDemo;
Graphics::Texture* splashScreenTexture;

namespace Lina::Editor
{
    GUILayer*            GUILayer::s_guiLayer = nullptr;
    Graphics::DrawParams m_drawParameters;

    void GUILayer::Initialize()
    {
        m_footerSize           = 20.0f;
        m_linaLogoIcon         = ICON_FA_FIRE;
        m_defaultWindowPadding = Vector2(8, 8);
        m_defaultFramePadding  = Vector2(8, 2);
        m_storage              = Resources::ResourceStorage::Get();

        Event::EventSystem::Get()->Connect<Event::EShutdown, &GUILayer::OnShutdown>(this);
        Event::EventSystem::Get()->Connect<Event::EPostRender, &GUILayer::OnPostRender>(this);
        Event::EventSystem::Get()->Connect<EMenuBarElementClicked, &GUILayer::OnMenuBarElementClicked>(this);

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImPlot::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        (void)io;

        // Add default font.
        io.FontDefault = io.Fonts->AddFontFromFileTTF("Resources/Editor/Fonts/Mukta-Medium.ttf", 20.0f, NULL);

        // merge in icons from Font Awesome
        static const ImWchar icons_rangesFA[]   = {ICON_MIN_FA, ICON_MAX_FA, 0};
        static const ImWchar icons_rangesCUST[] = {ICON_MIN_CS, ICON_MAX_CS, 0};

        ImFontConfig icons_config;
        icons_config.MergeMode     = false;
        icons_config.PixelSnapH    = false;
        icons_config.GlyphOffset.y = 1.2f;

        m_iconFontDefault = io.Fonts->AddFontFromFileTTF("Resources/Editor/Fonts/FontAwesome/fa-solid-900.ttf", 18.0f, &icons_config, icons_rangesFA);

        icons_config.MergeMode = true;
        io.Fonts->AddFontFromFileTTF("Resources/Editor/Fonts/CustomIcons/icomoon.ttf", 18.0f, &icons_config, icons_rangesCUST);

        icons_config.GlyphOffset.y = 2.25f;
        icons_config.MergeMode     = false;
        m_iconFontSmall            = io.Fonts->AddFontFromFileTTF("Resources/Editor/Fonts/FontAwesome/fa-solid-900.ttf", 13.0f, &icons_config, icons_rangesFA);

        icons_config.MergeMode = true;
        io.Fonts->AddFontFromFileTTF("Resources/Editor/Fonts/CustomIcons/icomoon.ttf", 13.0f, &icons_config, icons_rangesCUST);

        m_bigFont     = io.Fonts->AddFontFromFileTTF("Resources/Editor/Fonts/MuktaMahee-Medium.ttf", 30, NULL);
        m_mediumFont  = io.Fonts->AddFontFromFileTTF("Resources/Editor/Fonts/MuktaMahee-Medium.ttf", 24, NULL);
        m_defaultFont = io.FontDefault;

        ImGui::GetCurrentContext()->iconFont = m_iconFontSmall;

        // Setup configuration flags.
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        io.ConfigDockingTransparentPayload = true;

        ImGuiStyle& style     = ImGui::GetStyle();
        ImVec4*     colors    = ImGui::GetStyle().Colors;
        style.FrameBorderSize = 1.0f;
        style.PopupBorderSize = 1.0f;
        // style.AntiAliasedFill = false;
        // style.WindowRounding = 0.0f;
        style.TabRounding = 3.0f;
        // style.ChildRounding = 0.0f;
        style.PopupRounding = 3.0f;
        // style.FrameRounding = 0.0f;
        // style.ScrollbarRounding = 5.0f;
        style.FramePadding  = ImVec2(m_defaultFramePadding.x, m_defaultFramePadding.y);
        style.WindowPadding = ImVec2(m_defaultWindowPadding.x, m_defaultWindowPadding.y);
        style.CellPadding   = ImVec2(9, 2);
        // style.ItemInnerSpacing = ImVec2(8, 4);
        // style.ItemInnerSpacing = ImVec2(5, 4);
        // style.GrabRounding = 6.0f;
        // style.GrabMinSize     = 6.0f;
        style.ChildBorderSize = 0.0f;
        // style.TabBorderSize = 0.0f;
        style.WindowBorderSize                  = 1.0f;
        style.WindowMenuButtonPosition          = ImGuiDir_None;
        colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        colors[ImGuiCol_WindowBg]               = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
        colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_PopupBg]                = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
        colors[ImGuiCol_Border]                 = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_PopupBorder]            = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
        colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_FrameBg]                = ImVec4(0.04f, 0.04f, 0.04f, 0.54f);
        colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.44f, 0.26f, 0.26f, 1.00f);
        colors[ImGuiCol_FrameBgActive]          = ImVec4(0.47f, 0.19f, 0.19f, 1.00f);
        colors[ImGuiCol_TitleBg]                = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
        colors[ImGuiCol_TitleBgActive]          = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
        colors[ImGuiCol_MenuBarBg]              = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
        colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
        colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.23f, 0.23f, 0.23f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
        colors[ImGuiCol_CheckMark]              = ImVec4(0.47f, 0.19f, 0.19f, 1.00f);
        colors[ImGuiCol_SliderGrab]             = ImVec4(0.47f, 0.19f, 0.19f, 1.00f);
        colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.74f, 0.74f, 0.74f, 1.00f);
        colors[ImGuiCol_Button]                 = ImVec4(0.23f, 0.23f, 0.23f, 1.00f);
        colors[ImGuiCol_ButtonHovered]          = ImVec4(0.35f, 0.49f, 0.62f, 1.00f);
        colors[ImGuiCol_ButtonActive]           = ImVec4(0.24f, 0.37f, 0.53f, 1.00f);
        colors[ImGuiCol_ButtonLocked]           = ImVec4(0.183f, 0.273f, 0.364f, 1.000f);
        colors[ImGuiCol_ButtonSecondary]        = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_ButtonSecondaryHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_ButtonSecondaryActive]  = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_ButtonSecondaryLocked]  = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_Folder]                 = ImVec4(0.23f, 0.23f, 0.23f, 1.00f);
        colors[ImGuiCol_FolderHovered]          = ImVec4(0.35f, 0.49f, 0.62f, 1.00f);
        colors[ImGuiCol_FolderActive]           = ImVec4(0.24f, 0.37f, 0.53f, 1.00f);
        colors[ImGuiCol_Toolbar]                = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
        colors[ImGuiCol_Icon]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        colors[ImGuiCol_TitleHeader]            = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
        colors[ImGuiCol_TitleHeaderHover]       = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
        colors[ImGuiCol_TitleHeaderPressed]     = ImVec4(0.09f, 0.09f, 0.09f, 1.00f);
        colors[ImGuiCol_TitleHeaderBorder]      = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_TitleHeaderDisabled]    = ImVec4(0.17f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_Header]                 = ImVec4(0.47f, 0.19f, 0.19f, 1.00f);
        colors[ImGuiCol_HeaderHovered]          = ImVec4(0.43f, 0.24f, 0.24f, 1.00f);
        colors[ImGuiCol_HeaderActive]           = ImVec4(0.49f, 0.32f, 0.32f, 1.00f);
        colors[ImGuiCol_Separator]              = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_SeparatorActive]        = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_ResizeGrip]             = ImVec4(0.44f, 0.44f, 0.44f, 1.00f);
        colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.58f, 0.58f, 0.58f, 1.00f);
        colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.73f, 0.73f, 0.73f, 1.00f);
        colors[ImGuiCol_Tab]                    = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
        colors[ImGuiCol_TabHovered]             = ImVec4(0.24f, 0.25f, 0.26f, 1.00f);
        colors[ImGuiCol_TabActive]              = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
        colors[ImGuiCol_TabUnfocused]           = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
        colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
        colors[ImGuiCol_DockingPreview]         = ImVec4(0.47f, 0.19f, 0.19f, 1.00f);
        colors[ImGuiCol_DockingEmptyBg]         = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
        colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        colors[ImGuiCol_PlotHistogram]          = ImVec4(0.69f, 0.15f, 0.29f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
        colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
        colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
        colors[ImGuiCol_TableBorderLight]       = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
        colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
        colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.47f, 0.20f, 0.20f, 0.71f);
        colors[ImGuiCol_DragDropTarget]         = ImVec4(0.58f, 0.23f, 0.23f, 0.71f);
        colors[ImGuiCol_NavHighlight]           = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
        colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.00f, 0.00f, 0.00f, 0.61f);

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding              = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        ImPlot::GetStyle().AntiAliasedLines = true;

#ifdef LINA_GRAPHICS_OPENGL
        GLFWwindow* window = static_cast<GLFWwindow*>(Graphics::WindowBackend::Get()->GetNativeWindow());
        // Setup Platform/Renderer bindings
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init();

#else
        LINA_ERR("Undefined platform for IMGUI!");
#endif

        m_drawParameters   = Graphics::DrawParameterHelper::GetGUILayer();
        m_shouldDrawSplash = true;

        // Splash screen
        Graphics::WindowBackend* splashWindow = Graphics::WindowBackend::Get();
        const GLFWvidmode*       mode         = glfwGetVideoMode(glfwGetPrimaryMonitor());
        Vector2                  splashSize   = Vector2(720, 450);
        splashWindow->SetPosCentered(Vector2((-splashSize.x / 2.0f), (-splashSize.y / 2.0f)));
        splashWindow->SetSize(splashSize);

        Event::EventSystem::Get()->Connect<Event::EResourceLoadUpdated, &GUILayer::OnResourceLoadUpdated>(this);
        splashScreenTexture = new Graphics::Texture();
        splashScreenTexture->LoadFromFile("Resources/Editor/Textures/SplashScreen.png");
        m_storage->Add(static_cast<void*>(splashScreenTexture), GetTypeID<Graphics::Texture>(), StringID("Resources/Editor/Textures/SplashScreen.png").value());
        DrawSplashScreen();

        Engine::Get()->StartLoadingResources();
        m_shouldDrawSplash = false;

        // Initialize first.
        m_shortcutManager.Initialize();
        m_headerPanel.Initialize(ID_HEADER, nullptr);

        // Init rest.
        m_toolbar.Initialize(ID_TOOLBAR, nullptr);
        m_entitiesPanel.Initialize(ID_ENTITIES, ICON_FA_CUBE);
        m_systemsPanel.Initialize(ID_SYSTEMS, ICON_FA_COGS);
        m_logPanel.Initialize(ID_LOG, ICON_FA_CLIPBOARD);
        m_profilerPanel.Initialize(ID_PROFILER, ICON_FA_CHART_LINE);
        m_propertiesPanel.Initialize(ID_PROPERTIES, ICON_FA_COG);
        m_levelPanel.Initialize(ID_SCENE, ICON_FA_GAMEPAD);
        m_resourcesPanel.Initialize(ID_RESOURCES, ICON_FA_FILE);
        m_globalSettingsPanel.Initialize(ID_GLOBAL, ICON_FA_GLOBE);
        m_resourceSelectorPanel.Initialize(ID_RESOURCESELECTOR, ICON_FA_FILE_ARCHIVE);
        m_modelPanel.Initialize(ID_MODELPANEL, ICON_FA_CUBES);
        m_materialPanel.Initialize(ID_MATERIALPANEL, ICON_FA_GLOBE);
        m_shaderPanel.Initialize(ID_SHADERPANEL, ICON_FA_DICE_D20);
        m_texturePanel.Initialize(ID_TEXTUREPANEL, ICON_FA_IMAGE);

        m_modelPanel.Close();
        m_materialPanel.Close();
        m_shaderPanel.Close();
        m_texturePanel.Close();
    }

    void GUILayer::OnShutdown(const Event::EShutdown& ev)
    {
        // Cleanup
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        ImPlot::DestroyContext();
    }

    bool   show_demo_window    = true;
    bool   show_another_window = false;
    ImVec4 clear_color         = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    void GUILayer::OnPostRender(const Event::EPostRender& ev)
    {
        // Set draw params first.
        Graphics::RenderEngineBackend::Get()->SetDrawParameters(m_drawParameters);

#ifdef LINA_GRAPHICS_OPENGL
        // Setup
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (s_showIMGUIDemo)
            ImGui::ShowDemoWindow(&s_showIMGUIDemo);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_PopupBorderSize, 1.0f);

        m_headerPanel.Draw();
        CentralDockingSpace::Draw();
        m_resourcesPanel.Draw();
        m_entitiesPanel.Draw();
        m_systemsPanel.Draw();
        m_levelPanel.Draw();
        m_logPanel.Draw();
        m_profilerPanel.Draw();
        m_propertiesPanel.Draw();
        m_globalSettingsPanel.Draw();
        m_toolbar.DrawFooter();
        m_resourceSelectorPanel.Draw();
        m_modelPanel.Draw();
        m_materialPanel.Draw();
        m_texturePanel.Draw();
        m_shaderPanel.Draw();

        Event::EventSystem::Get()->Trigger<EGUILayerRender>(EGUILayerRender());

        ImGui::PopStyleVar();
        ImGui::PopStyleVar();

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

#endif
    }

    void GUILayer::OnMenuBarElementClicked(const EMenuBarElementClicked& event)
    {
        MenuBarElementType item = static_cast<MenuBarElementType>(event.m_item);

        // File
        if (item == MenuBarElementType::NewProject)
        {
        }
        else if (item == MenuBarElementType::LoadProject)
        {
        }
        else if (item == MenuBarElementType::SaveProject)
        {
        }
        else if (item == MenuBarElementType::PackageProject)
        {
            std::string fullPath = "";
            fullPath             = EditorUtility::SaveFile(".linabundle", Graphics::WindowBackend::Get()->GetNativeWindow());

            if (fullPath.compare("") != 0)
            {
                size_t      lastIndex  = fullPath.find_last_of("/");
                std::string folderPath = fullPath.substr(0, lastIndex);
                std::string fileName   = fullPath.substr(lastIndex + 1);
                Application::Get()->PackageProject(folderPath, fileName);
            }
        }

        // Level
        else if (item == MenuBarElementType::NewLevelData)
        {
            // Build a new level.
            Application::Get()->InstallLevel(m_defaultLevel);
        }
        else if (item == MenuBarElementType::SaveLevelData)
        {
            std::string fullPath = "";
            fullPath             = EditorUtility::SaveFile(".linaleveldata", Graphics::WindowBackend::Get()->GetNativeWindow());

            if (fullPath.compare("") != 0)
            {
                size_t      lastIndex  = fullPath.find_last_of("/");
                std::string folderPath = fullPath.substr(0, lastIndex);
                std::string fileName   = fullPath.substr(lastIndex + 1);
                Application::Get()->SaveLevelData(folderPath, fileName);
            }
        }
        else if (item == MenuBarElementType::LoadLevelData)
        {
            std::string fullPath = "";
            fullPath             = EditorUtility::OpenFile(".linaleveldata", Graphics::WindowBackend::Get()->GetNativeWindow());

            if (fullPath.compare("") != 0)
            {
                size_t      lastIndex  = fullPath.find_last_of("/");
                std::string folderPath = fullPath.substr(0, lastIndex);
                std::string fileName   = EditorUtility::RemoveExtensionFromFilename(fullPath.substr(lastIndex + 1));
                Application::Get()->LoadLevelData(folderPath, fileName);
            }
        }

        // Panels.
        else if (item == MenuBarElementType::EntitiesPanel)
            m_entitiesPanel.Open();
        else if (item == MenuBarElementType::SystemsPanel)
            m_systemsPanel.Open();
        else if (item == MenuBarElementType::LevelPanel)
            m_levelPanel.Open();
        else if (item == MenuBarElementType::ResourcesPanel)
            m_resourcesPanel.Open();
        else if (item == MenuBarElementType::PropertiesPanel)
            m_propertiesPanel.Open();
        else if (item == MenuBarElementType::LogPanel)
            m_logPanel.Open();
        else if (item == MenuBarElementType::GlobalSettingsPanel)
            m_globalSettingsPanel.Open();
        else if (item == MenuBarElementType::ProfilerPanel)
            m_profilerPanel.Open();

        // Debug
        else if (item == MenuBarElementType::ImGuiPanel)
            s_showIMGUIDemo = true;

        // Objects
        else if (item == MenuBarElementType::Empty)
            ECS::Registry::Get()->CreateEntity("Empty");
        else if (item == MenuBarElementType::Cube)
            Graphics::RenderEngineBackend::Get()->GetModelNodeSystem()->CreateModelHierarchy(m_storage->GetResource<Graphics::Model>("Resources/Engine/Meshes/Primitives/Cube.fbx"));
        else if (item == MenuBarElementType::Cylinder)
            Graphics::RenderEngineBackend::Get()->GetModelNodeSystem()->CreateModelHierarchy(m_storage->GetResource<Graphics::Model>("Resources/Engine/Meshes/Primitives/Cylinder.fbx"));
        else if (item == MenuBarElementType::Capsule)
            Graphics::RenderEngineBackend::Get()->GetModelNodeSystem()->CreateModelHierarchy(m_storage->GetResource<Graphics::Model>("Resources/Engine/Meshes/Primitives/Capsule.fbx"));
        else if (item == MenuBarElementType::Quad)
            Graphics::RenderEngineBackend::Get()->GetModelNodeSystem()->CreateModelHierarchy(m_storage->GetResource<Graphics::Model>("Resources/Engine/Meshes/Primitives/Quad.fbx"));
        else if (item == MenuBarElementType::Sphere)
            Graphics::RenderEngineBackend::Get()->GetModelNodeSystem()->CreateModelHierarchy(m_storage->GetResource<Graphics::Model>("Resources/Engine/Meshes/Primitives/Sphere.fbx"));
        else if (item == MenuBarElementType::Plane)
            Graphics::RenderEngineBackend::Get()->GetModelNodeSystem()->CreateModelHierarchy(m_storage->GetResource<Graphics::Model>("Resources/Engine/Meshes/Primitives/Plane.fbx"));
        else if (item == MenuBarElementType::PLight)
        {
            auto ent = ECS::Registry::Get()->CreateEntity("Point Light");
            ECS::Registry::Get()->emplace<ECS::PointLightComponent>(ent);
        }
        else if (item == MenuBarElementType::DLight)
        {
            auto ent = ECS::Registry::Get()->CreateEntity("Sun Light");
            ECS::Registry::Get()->emplace<ECS::DirectionalLightComponent>(ent);
        }
        else if (item == MenuBarElementType::SLight)
        {
            auto ent = ECS::Registry::Get()->CreateEntity("Spot Light");
            ECS::Registry::Get()->emplace<ECS::SpotLightComponent>(ent);
        }

        // About
        else if (item == MenuBarElementType::Github)
        {

#ifdef LINA_PLATFORM_WINDOWS
            ShellExecute(NULL, NULL, "http://www.github.com/inanevin/LinaEngine", NULL, NULL, SW_SHOWNORMAL);
#endif
        }
        else if (item == MenuBarElementType::Website)
        {
#ifdef LINA_PLATFORM_WINDOWS
            ShellExecute(NULL, NULL, "http://www.inanevin.com", NULL, NULL, SW_SHOWNORMAL);

#endif
        }
    }

    void GUILayer::OnResourceLoadUpdated(const Event::EResourceLoadUpdated& ev)
    {
        m_currentlyLoadingResource = ev.m_currentResource;
        m_percentage               = ev.m_percentage;

        if (m_shouldDrawSplash)
            DrawSplashScreen();
        else
            m_progressPanel.Draw(m_currentlyLoadingResource, m_percentage);
    }

    void GUILayer::DrawSplashScreen()
    {
        // Nf
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Setup wndow.
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);

        // Draw window.
#pragma warning(disable : 4312)

        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::Begin("SplashScreen", NULL, ImGuiWindowFlags_NoDecoration);

        const ImVec2 splashMin  = viewport->Pos;
        const ImVec2 splashSize = viewport->Size;
        const ImVec2 splashMax  = ImVec2(splashMin.x + splashSize.x, splashMin.y + splashSize.y);

        ImGui::GetWindowDrawList()->AddImage((void*)(splashScreenTexture->GetID()), splashMin, splashMax, ImVec2(0, 1), ImVec2(1, 0));
        ImGui::SetNextWindowPos(ImVec2(splashMin.x + 40, splashMin.y + 310));
        ImGui::SetNextWindowBgAlpha(0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);
        ImGui::BeginChild("text", ImVec2(640, 90), false, ImGuiWindowFlags_NoDecoration);
        ImGui::Text("Loading %c", "|/-\\"[(int)(ImGui::GetTime() / 0.05f) & 3]);
        ImGui::Text(m_currentlyLoadingResource.c_str());
        std::string loadData = std::to_string(m_percentage) + "%";
        ImGui::Text(loadData.c_str());
        WidgetsUtility::IncrementCursorPosY(10);
        WidgetsUtility::HorizontalDivider(2.0f);

        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_Header));
        WidgetsUtility::HorizontalDivider(2.0f, 2.0f, ImGui::GetWindowWidth() * m_percentage / 100.0f);
        ImGui::PopStyleColor();
        ImGui::EndChild();
        ImGui::PopStyleVar();
        ImGui::End();
        ImGui::PopStyleVar();
        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        // swap buffers.
        Graphics::WindowBackend::Get()->Tick();
    }

    void GUILayer::DrawFPSCounter(int corner)
    {
        // FIXME-VIEWPORT: Select a default viewport
        const float DISTANCE = 10.0f;
        ImGuiIO&    io       = ImGui::GetIO();
        if (corner != -1)
        {
            ImGuiViewport* viewport         = ImGui::GetMainViewport();
            ImVec2         work_area_pos    = viewport->WorkPos; // Instead of using viewport->Pos we use GetWorkPos() to avoid menu bars, if any!
            ImVec2         work_area_size   = viewport->WorkSize;
            ImVec2         window_pos       = ImVec2((corner & 1) ? (work_area_pos.x + work_area_size.x - DISTANCE) : (work_area_pos.x + DISTANCE), (corner & 2) ? (work_area_pos.y + work_area_size.y - DISTANCE) : (work_area_pos.y + DISTANCE));
            ImVec2         window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
            ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
            ImGui::SetNextWindowViewport(viewport->ID);
        }

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

        if (ImGui::Begin("FPSOverlay", NULL, window_flags))
        {
            ImGui::Text("FPS Overlay");
            ImGui::Separator();
            std::string fpsText = std::to_string(Engine::Get()->GetCurrentFPS()) + " Frames per second";
            ImGui::Text(fpsText.c_str());
        }
        ImGui::End();
    }

} // namespace Lina::Editor
