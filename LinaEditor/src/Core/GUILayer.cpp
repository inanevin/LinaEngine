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

#include "Core/GUILayer.hpp"

#include "Core/Application.hpp"
#include "Core/CustomFontIcons.hpp"
#include "Core/EditorApplication.hpp"
#include "Core/EditorCommon.hpp"
#include "Core/Engine.hpp"
#include "Core/InputBackend.hpp"
#include "Core/PhysicsBackend.hpp"
#include "Core/RenderBackendFwd.hpp"
#include "ECS/Components/ModelRendererComponent.hpp"
#include "EventSystem/GraphicsEvents.hpp"
#include "EventSystem/ResourceEvents.hpp"
#include "Helpers/DrawParameterHelper.hpp"
#include "IconsFontAwesome5.h"
#include "IconsForkAwesome.h"
#include "IconsMaterialDesign.h"
#include "Log/Log.hpp"
#include "Utility/EditorUtility.hpp"
#include "Utility/UtilityFunctions.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_internal.h"
#include "imgui/implot/implot.h"

#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include <glad/glad.h>

// AFTER GLAD
#include <GLFW/glfw3.h>

static ImGuiTreeNodeFlags s_baseFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
static bool               s_showIMGUIDemo;
static bool               s_setDockspaceLayout  = true;
static bool               s_physicsDebugEnabled = false;
static bool               s_dockWindowInit      = true;
static const char*        s_saveLevelDialogID   = "id_saveLevel";
static const char*        s_loadLevelDialogID   = "id_loadLevel";
Graphics::Texture*        splashScreenTexture;

namespace Lina::Editor
{
    ImFont*                             GUILayer::s_defaultFont     = nullptr;
    ImFont*                             GUILayer::s_bigFont         = nullptr;
    ImFont*                             GUILayer::s_iconFontSmall   = nullptr;
    ImFont*                             GUILayer::s_iconFontDefault = nullptr;
    std::map<const char*, EditorPanel*> GUILayer::s_editorPanels;
    float                               GUILayer::s_headerSize = 0.0f;
    Graphics::DrawParams                m_drawParameters;

    void GUILayer::Initialize()
    {
        Event::EventSystem::Get()->Connect<Event::EShutdown, &GUILayer::OnShutdown>(this);
        Event::EventSystem::Get()->Connect<Event::EPostRender, &GUILayer::OnPostRender>(this);

        // Listen to menu bar clicked events.
        Event::EventSystem::Get()->Connect<EMenuBarItemClicked, &GUILayer::DispatchMenuBarClickedAction>(this);

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

        s_iconFontDefault = io.Fonts->AddFontFromFileTTF("Resources/Editor/Fonts/FontAwesome/fa-solid-900.ttf", 18.0f, &icons_config, icons_rangesFA);

        icons_config.MergeMode = true;
        io.Fonts->AddFontFromFileTTF("Resources/Editor/Fonts/CustomIcons/icomoon.ttf", 18.0f, &icons_config, icons_rangesCUST);

        icons_config.GlyphOffset.y = 2.25f;
        icons_config.MergeMode     = false;
        s_iconFontSmall            = io.Fonts->AddFontFromFileTTF("Resources/Editor/Fonts/FontAwesome/fa-solid-900.ttf", 13.0f, &icons_config, icons_rangesFA);

        icons_config.MergeMode = true;
        io.Fonts->AddFontFromFileTTF("Resources/Editor/Fonts/CustomIcons/icomoon.ttf", 13.0f, &icons_config, icons_rangesCUST);

        s_bigFont     = io.Fonts->AddFontFromFileTTF("Resources/Editor/Fonts/MuktaMahee-Medium.ttf", 30, NULL);
        s_defaultFont = io.FontDefault;

        // Setup configuration flags.
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        ImGui::StyleColorsDark();
        //	io.ConfigViewportsNoAutoMerge = true;
        //	io.ConfigViewportsNoTaskBarIcon = true;

#ifdef LINA_GRAPHICS_OPENGL
        GLFWwindow* window = static_cast<GLFWwindow*>(Graphics::WindowBackend::Get()->GetNativeWindow());

        // Setup Platform/Renderer bindings
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init();
#else
        LINA_ERR("Undefined platform for IMGUI!");
#endif

        // Setup Dear ImGui style
        ImGuiStyle& style     = ImGui::GetStyle();
        ImVec4*     colors    = ImGui::GetStyle().Colors;
        style.FrameBorderSize = 1.0f;
        style.PopupBorderSize = 1.0f;
        // style.AntiAliasedFill = false;
        // style.WindowRounding = 0.0f;
        // style.TabRounding = 2.0f;
        // style.ChildRounding = 0.0f;
        // style.PopupRounding = 3.0f;
        // style.FrameRounding = 0.0f;
        // style.ScrollbarRounding = 5.0f;
        // style.FramePadding  = ImVec2(5, 5);
        // style.WindowPadding = ImVec2(0, 5);
        // style.ItemInnerSpacing = ImVec2(8, 4);
        // style.ItemInnerSpacing = ImVec2(5, 4);
        // style.GrabRounding = 6.0f;
        // style.GrabMinSize     = 6.0f;
        // style.ChildBorderSize = 0.0f;
        // style.TabBorderSize = 0.0f;
        // style.WindowBorderSize = 0.0f;
        // style.WindowMenuButtonPosition         = ImGuiDir_None;
        // colors[ImGuiCol_Text]                  = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        // colors[ImGuiCol_TextDisabled]          = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        // colors[ImGuiCol_WindowBg]              = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
        // colors[ImGuiCol_ChildBg]               = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
        // colors[ImGuiCol_PopupBg]               = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
        colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
        // colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        // colors[ImGuiCol_FrameBg]               = ImVec4(0.00f, 0.00f, 0.00f, 0.54f);
        // colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.45f, 0.28f, 0.46f, 1.00f);
        // colors[ImGuiCol_FrameBgActive]         = ImVec4(0.45f, 0.34f, 0.46f, 1.00f);
        // colors[ImGuiCol_TitleBg]               = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
        // colors[ImGuiCol_TitleBgActive]         = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
        // colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
        // colors[ImGuiCol_MenuBarBg]             = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
        // colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
        // colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
        // colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.23f, 0.23f, 0.23f, 1.00f);
        // colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
        // colors[ImGuiCol_CheckMark]             = ImVec4(0.45f, 0.28f, 0.46f, 1.00f);
        // colors[ImGuiCol_SliderGrab]            = ImVec4(0.45f, 0.28f, 0.46f, 1.00f);
        // colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.74f, 0.74f, 0.74f, 1.00f);
        colors[ImGuiCol_Button]        = ImVec4(0.186f, 0.186f, 0.186f, 1.000f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.35f, 0.49f, 0.62f, 1.00f);
        colors[ImGuiCol_ButtonActive]  = ImVec4(0.24f, 0.37f, 0.53f, 1.00f);
        colors[ImGuiCol_ButtonLocked]  = ImVec4(0.10f, 0.15f, 0.20f, 1.00f);
        colors[ImGuiCol_Toolbar]       = ImVec4(0.058f, 0.058f, 0.058f, 1.00f);
        // colors[ImGuiCol_Header]                = ImVec4(0.45f, 0.28f, 0.46f, 1.00f);
        // colors[ImGuiCol_HeaderHovered]         = ImVec4(0.46f, 0.34f, 0.47f, 1.00f);
        // colors[ImGuiCol_HeaderActive]          = ImVec4(0.47f, 0.39f, 0.47f, 1.00f);
        // colors[ImGuiCol_Separator]             = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
        // colors[ImGuiCol_SeparatorHovered]      = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
        // colors[ImGuiCol_SeparatorActive]       = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
        // colors[ImGuiCol_ResizeGrip]            = ImVec4(0.44f, 0.44f, 0.44f, 1.00f);
        // colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.58f, 0.58f, 0.58f, 1.00f);
        // colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.73f, 0.73f, 0.73f, 1.00f);
        // colors[ImGuiCol_Tab]                   = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
        // colors[ImGuiCol_TabHovered]            = ImVec4(0.24f, 0.25f, 0.26f, 1.00f);
        // colors[ImGuiCol_TabActive]             = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
        // colors[ImGuiCol_TabUnfocused]          = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
        // colors[ImGuiCol_TabUnfocusedActive]    = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
        // colors[ImGuiCol_DockingPreview]        = ImVec4(0.45f, 0.28f, 0.46f, 1.00f);
        // colors[ImGuiCol_DockingEmptyBg]        = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
        // colors[ImGuiCol_PlotLines]             = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
        // colors[ImGuiCol_PlotLinesHovered]      = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        // colors[ImGuiCol_PlotHistogram]         = ImVec4(0.69f, 0.15f, 0.29f, 1.00f);
        // colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
        // colors[ImGuiCol_TableHeaderBg]         = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
        // colors[ImGuiCol_TableBorderStrong]     = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
        // colors[ImGuiCol_TableBorderLight]      = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
        // colors[ImGuiCol_TableRowBg]            = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        // colors[ImGuiCol_TableRowBgAlt]         = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
        // colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.45f, 0.28f, 0.46f, 1.00f);
        // colors[ImGuiCol_DragDropTarget]        = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
        // colors[ImGuiCol_NavHighlight]          = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
        // colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        // colors[ImGuiCol_NavWindowingDimBg]     = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        // colors[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.00f, 0.00f, 0.00f, 0.61f);

        // colors[ImGuiCol_Icon]                  = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        // colors[ImGuiCol_TitleHeader]           = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
        // colors[ImGuiCol_TitleHeaderHover]      = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
        // colors[ImGuiCol_TitleHeaderPressed]    = ImVec4(0.09f, 0.09f, 0.09f, 1.00f);
        // colors[ImGuiCol_TitleHeaderBorder]     = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
        // colors[ImGuiCol_TitleHeaderDisabled]   = ImVec4(0.17f, 0.00f, 0.00f, 1.00f);

        ImPlot::GetStyle().AntiAliasedLines = true;

        s_setDockspaceLayout = true;

        m_drawParameters = Graphics::DrawParameterHelper::GetGUILayer();

        // Splash screen
        Graphics::WindowBackend* splashWindow = Graphics::WindowBackend::Get();
        const GLFWvidmode*       mode         = glfwGetVideoMode(glfwGetPrimaryMonitor());
        Vector2                  splashSize   = Vector2(720, 450);
        splashWindow->SetSize(splashSize);
        splashWindow->SetPosCentered(Vector2(0, 0));
        Event::EventSystem::Get()->Connect<Event::EResourceLoadUpdated, &GUILayer::OnResourceLoadUpdated>(this);
        splashScreenTexture = &Graphics::Texture::CreateTexture2D("Resources/Editor/Textures/SplashScreen.png", Graphics::SamplerParameters(), false, false, "");
        DrawSplashScreen();

        Engine::Get()->StartLoadingResources();
        m_toolbar.Initialize(ID_TOOLBAR);
        m_ecsPanel.Initialize(ID_ECS);
        m_headerPanel.Initialize(ID_HEADER);
        m_logPanel.Initialize(ID_LOG);
        m_profilerPanel.Initialize(ID_PROFILER);
        m_propertiesPanel.Initialize(ID_PROPERTIES);
        m_levelPanel.Initialize(ID_SCENE);
        m_resourcesPanel.Initialize(ID_RESOURCES);
        m_globalSettingsPanel.Initialize(ID_GLOBAL);

        // Imgui first frame initialization.
        // OnPostRender(Event::EPostRender());
    }

    void GUILayer::OnShutdown(const Event::EShutdown& ev)
    {
        // Cleanup
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        ImPlot::DestroyContext();
    }

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

        m_headerPanel.Draw();
        m_toolbar.Draw();
        // DrawCentralDockingSpace();

        // m_resourcesPanel.Draw();
        // m_ecsPanel.Draw();
        // m_levelPanel.Draw();
        // m_logPanel.Draw();
        // m_profilerPanel.Draw();
        // m_propertiesPanel.Draw();
        // m_globalSettingsPanel.Draw();
        // m_toolbar.DrawFooter();

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

    void GUILayer::DispatchMenuBarClickedAction(const EMenuBarItemClicked& event)
    {
        MenuBarItems item = event.m_item;

        // File
        if (item == MenuBarItems::NewProject)
        {
        }
        else if (item == MenuBarItems::LoadProject)
        {
        }
        else if (item == MenuBarItems::SaveProject)
        {
        }
        else if (item == MenuBarItems::PackageProject)
        {
            std::string fullPath = "";
            fullPath             = EditorUtility::SaveFile(".linabundle", Graphics::WindowBackend::Get()->GetNativeWindow());

            if (fullPath.compare("") != 0)
            {
                size_t      lastIndex  = fullPath.find_last_of("/");
                std::string folderPath = fullPath.substr(0, lastIndex);
                std::string fileName   = fullPath.substr(lastIndex + 1);
                Application::Get().PackageProject(folderPath, fileName);
            }
        }

        // Edit

        // View

        // Level
        else if (item == MenuBarItems::NewLevelData)
        {
            // Build a new level.
            Application::Get().InstallLevel(m_defaultLevel);
        }
        else if (item == MenuBarItems::SaveLevelData)
        {
            std::string fullPath = "";
            fullPath             = EditorUtility::SaveFile(".linaleveldata", Graphics::WindowBackend::Get()->GetNativeWindow());

            if (fullPath.compare("") != 0)
            {
                size_t      lastIndex  = fullPath.find_last_of("/");
                std::string folderPath = fullPath.substr(0, lastIndex);
                std::string fileName   = fullPath.substr(lastIndex + 1);
                Application::Get().SaveLevelData(folderPath, fileName);
            }
        }
        else if (item == MenuBarItems::LoadLevelData)
        {
            std::string fullPath = "";
            fullPath             = EditorUtility::OpenFile(".linaleveldata", Graphics::WindowBackend::Get()->GetNativeWindow());

            if (fullPath.compare("") != 0)
            {
                size_t      lastIndex  = fullPath.find_last_of("/");
                std::string folderPath = fullPath.substr(0, lastIndex);
                std::string fileName   = EditorUtility::RemoveExtensionFromFilename(fullPath.substr(lastIndex + 1));
                Application::Get().LoadLevelData(folderPath, fileName);
            }
        }

        // Panels.
        else if (item == MenuBarItems::ECSPanel)
            m_ecsPanel.Open();
        else if (item == MenuBarItems::ScenePanel)
            m_levelPanel.Open();
        else if (item == MenuBarItems::ResourcesPanel)
            m_resourcesPanel.Open();
        else if (item == MenuBarItems::PropertiesPanel)
            m_propertiesPanel.Open();
        else if (item == MenuBarItems::LogPanel)
            m_logPanel.Open();
        else if (item == MenuBarItems::GlobalSettingsPanel)
            m_globalSettingsPanel.Open();
        else if (item == MenuBarItems::ProfilerPanel)
            m_profilerPanel.Open();
        else if (item == MenuBarItems::ImGuiPanel)
            s_showIMGUIDemo = true;

        // Debug
        else if (item == MenuBarItems::DebugViewShadows)
            m_levelPanel.SetDrawMode(Editor::LevelPanel::DrawMode::ShadowMap);

        else if (item == MenuBarItems::DebugViewNormal)
            m_levelPanel.SetDrawMode(Editor::LevelPanel::DrawMode::FinalImage);

        // Objects

        else if (item == MenuBarItems::Cube)
            Graphics::RenderEngineBackend::Get()->GetModelNodeSystem()->CreateModelHierarchy(Graphics::Model::GetModel("Resources/Engine/Meshes/Primitives/Cube.fbx"));
        else if (item == MenuBarItems::Cylinder)
            Graphics::RenderEngineBackend::Get()->GetModelNodeSystem()->CreateModelHierarchy(Graphics::Model::GetModel("Resources/Engine/Meshes/Primitives/Cylinder.fbx"));
        else if (item == MenuBarItems::Capsule)
            Graphics::RenderEngineBackend::Get()->GetModelNodeSystem()->CreateModelHierarchy(Graphics::Model::GetModel("Resources/Engine/Meshes/Primitives/Capsule.fbx"));
        else if (item == MenuBarItems::Quad)
            Graphics::RenderEngineBackend::Get()->GetModelNodeSystem()->CreateModelHierarchy(Graphics::Model::GetModel("Resources/Engine/Meshes/Primitives/Quad.fbx"));
        else if (item == MenuBarItems::Sphere)
            Graphics::RenderEngineBackend::Get()->GetModelNodeSystem()->CreateModelHierarchy(Graphics::Model::GetModel("Resources/Engine/Meshes/Primitives/Sphere.fbx"));
        else if (item == MenuBarItems::Plane)
            Graphics::RenderEngineBackend::Get()->GetModelNodeSystem()->CreateModelHierarchy(Graphics::Model::GetModel("Resources/Engine/Meshes/Primitives/Plane.fbx"));
    }

    void GUILayer::Refresh()
    {
        m_ecsPanel.Refresh();
    }

    void GUILayer::OnResourceLoadUpdated(const Event::EResourceLoadUpdated& ev)
    {
        m_currentlyLoadingResource = ev.m_currentResource;
        m_percentage               = ev.m_percentage;
        DrawSplashScreen();
    }

    void GUILayer::DrawSplashScreen()
    {
        // Nf
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Setup wndow.
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(viewport->Size);

        // Draw window.
#pragma warning(disable : 4312)

        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::Begin("SplashScreen", NULL, ImGuiWindowFlags_NoDecoration);
        ImGui::GetWindowDrawList()->AddImage((void*)(splashScreenTexture->GetID()), ImVec2(0, 0), viewport->Size, ImVec2(0, 1), ImVec2(1, 0));
        ImGui::SetNextWindowPos(ImVec2(40, 310));
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

    void GUILayer::DrawCentralDockingSpace()
    {
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_NoCloseButton | ImGuiDockNodeFlags_NoWindowMenuButton;

        ImGuiWindowFlags windowFlags = 0;
        windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
        windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        ImGuiViewport* viewport      = ImGui::GetMainViewport();
        ImVec2         dockspaceSize = ImVec2(viewport->WorkSize.x, viewport->WorkSize.y - HEADER_HEIGHT - TOOLBAR_HEIGHT - FOOTER_HEIGHT - DOCKSPACE_OFFSET);
        ImVec2         dockspacePos  = ImVec2(viewport->WorkPos.x, viewport->WorkPos.y + HEADER_HEIGHT + TOOLBAR_HEIGHT + DOCKSPACE_OFFSET);
        ImGui::SetNextWindowPos(dockspacePos);
        ImGui::SetNextWindowSize(dockspaceSize);
        ImGui::Begin("Lina Engine", NULL, windowFlags);
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");

        ImGui::DockSpace(dockspace_id, ImVec2(0, 0), ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_NoCloseButton | ImGuiDockNodeFlags_NoWindowMenuButton);

        if (s_setDockspaceLayout)
        {
            s_setDockspaceLayout = false;
            Vector2 screenSize   = Graphics::WindowBackend::Get()->GetSize();
            ImGui::DockBuilderRemoveNode(dockspace_id);                            // Clear out existing layout
            ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace); // Add empty node
            ImGui::DockBuilderSetNodeSize(dockspace_id, ImVec2(screenSize.x, screenSize.y - FOOTER_HEIGHT));

            ImGuiID dock_main_id        = dockspace_id; // This variable will track the document node, however we are not using it here as we aren't docking anything into it.
            ImGuiID dock_id_prop        = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.15f, NULL, &dock_main_id);
            ImGuiID dock_id_propBottom  = ImGui::DockBuilderSplitNode(dock_id_prop, ImGuiDir_Down, 0.6f, NULL, &dock_id_prop);
            ImGuiID dock_id_right       = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.25f, NULL, &dock_main_id);
            ImGuiID dock_id_rightBottom = ImGui::DockBuilderSplitNode(dock_id_right, ImGuiDir_Down, 0.20f, NULL, &dock_id_right);
            ImGuiID dock_id_bottom      = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.45f, NULL, &dock_main_id);

            ImGui::DockBuilderDockWindow(ID_RESOURCES, dock_id_bottom);
            ImGui::DockBuilderDockWindow(ID_ECS, dock_id_prop);
            ImGui::DockBuilderDockWindow(ID_SCENE, dock_main_id);
            ImGui::DockBuilderDockWindow(ID_LOG, dock_id_rightBottom);
            ImGui::DockBuilderDockWindow(ID_GLOBAL, dock_id_rightBottom);
            ImGui::DockBuilderDockWindow(ID_PROPERTIES, dock_id_right);
            ImGui::DockBuilderFinish(dockspace_id);
        }
        ImGui::End();
    }

} // namespace Lina::Editor
