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

#include "Core/EditorGUIManager.hpp"
#include "Core/EditorCommon.hpp"
#include "GUI/GUI.hpp"
#include "EventSystem/EventSystem.hpp"
#include "EventSystem/GraphicsEvents.hpp"
#include "EventSystem/ResourceEvents.hpp"
#include "EventSystem/MainLoopEvents.hpp"
#include "EventSystem/LevelEvents.hpp"
#include "Resource/Core/ResourceManager.hpp"
#include "Graphics/Resource/Font.hpp"
#include "Graphics/Resource/Texture.hpp"
#include "Graphics/Core/GUIBackend.hpp"
#include "GUI/Drawable.hpp"
#include "GUI/DockArea.hpp"
#include "Graphics/Core/WindowManager.hpp"
#include "Graphics/PipelineObjects/Swapchain.hpp"
#include "Panels/TopPanel.hpp"
#include "Graphics/Core/RenderEngine.hpp"
#include "Graphics/Core/SurfaceRenderer.hpp"

// Debug
#include "Input/Core/InputEngine.hpp"
#include "World/Core/World.hpp"
#include "Graphics/Resource/Texture.hpp"
#include "Graphics/Resource/Model.hpp"
#include "Graphics/Components/CameraComponent.hpp"
#include "Math/Math.hpp"
#include "Core/Time.hpp"

namespace Lina::Editor
{
    void EditorGUIManager::Initialize(const EngineSubsystems& subsys)
    {
        m_subsys     = subsys;
        m_nunitoFont = Resources::ResourceManager::Get()->GetResource<Graphics::Font>("Resources/Editor/Fonts/NunitoSans.ttf");
        m_rubikFont  = Resources::ResourceManager::Get()->GetResource<Graphics::Font>("Resources/Editor/Fonts/Rubik-Regular.ttf");

        m_rubikFont->GenerateFont(false, 12, 13);
        m_nunitoFont->GenerateFont(false, 12);

        m_subsys.renderEngine->GetGUIBackend()->UploadAllFontTextures();

        // Scan Icons folder & buffer all icons into a texture atlas.
        Vector<String> icons = Utility::GetFolderContents("Resources/Editor/Icons");
        m_iconTexture        = TexturePacker::PackFilesOrdered(m_subsys.renderEngine->GetGPUUploader(), icons, 2000, m_packedIcons);
        const StringID sid   = TO_SIDC("LINA_ENGINE_EDITOR_ICONPACK");
        m_iconTexture->ChangeSID(sid);
        m_iconTexture->SetUserManaged(true);
        Resources::ResourceManager::Get()->GetCache<Graphics::Texture>()->AddResource(sid, m_iconTexture);

        CreateImmediateGUI();

        // Events
        Event::EventSystem::Get()->Connect<Event::ETick, &EditorGUIManager::OnTick>(this);
        Event::EventSystem::Get()->Connect<Event::ESyncData, &EditorGUIManager::OnSyncData>(this);

        // then bottom dock area.
        m_mainDockArea = new DockArea(m_subsys);
        // m_mainDockArea->m_swapchain  = mainSwapchain;

        m_topPanel = new TopPanel(m_subsys);
        m_topPanel->Initialize();
        // m_topPanel->m_swapchain = mainSwapchain;
    }

    void EditorGUIManager::Shutdown()
    {
        m_topPanel->Shutdown();
        delete m_topPanel;

        for (auto d : m_additionalDockAreas)
        {
            d->Shutdown();
            delete d;
        }

        // m_topPanel.Shutdown();
        Event::EventSystem::Get()->Disconnect<Event::ETick>(this);
        Event::EventSystem::Get()->Disconnect<Event::ESyncData>(this);
        Resources::ResourceManager::Get()->UnloadUserManaged<Graphics::Texture>(m_iconTexture);
    }

    void EditorGUIManager::OnSyncData(const Event::ESyncData& ev)
    {
        for (auto& d : m_additionalDockAreas)
            d->SyncData();

        Vector<DockArea*> toDestroy;
        for (auto d : m_additionalDockAreas)
        {
            if (d->ShouldDestroy())
                toDestroy.push_back(d);
        }

        for (auto d : toDestroy)
        {
            d->Shutdown();
            delete d;
            m_additionalDockAreas.erase(std::remove(m_additionalDockAreas.begin(), m_additionalDockAreas.end(), d), m_additionalDockAreas.end());
        }

        FindHoveredSwapchain();
    }

    void EditorGUIManager::FindHoveredSwapchain()
    {
        auto*                        windowManager = m_subsys.renderEngine->GetWindowManager();
        const auto&                  windowData    = m_subsys.renderEngine->GetWindowData();
        int                          biggestZOrder = 0;
        Vector<Graphics::Swapchain*> hoveredSwapchains;
        m_topMostSwapchainID = 0;

        for (const auto& [sid, wd] : windowData)
        {
            auto       swp     = wd.swapchain;
            const Rect swpRect = Rect(swp->pos, swp->size);

            if (Input::InputEngine::Get()->IsPointInRect(Input::InputEngine::Get()->GetMousePositionAbs(), swpRect))
                hoveredSwapchains.push_back(swp);

            const int zOrder = windowManager->GetWindowZOrder(swp->swapchainID);
            if (zOrder >= biggestZOrder)
            {
                biggestZOrder        = zOrder;
                m_topMostSwapchainID = swp->swapchainID;
            }
        }

        const uint32 hoveredSwapchainsSize = static_cast<uint32>(hoveredSwapchains.size());

        if (hoveredSwapchainsSize == 0)
            m_hoveredSwapchainID = 0;
        else if (hoveredSwapchainsSize == 1)
            m_hoveredSwapchainID = hoveredSwapchains[0]->swapchainID;
        else
        {
            // Verify hover by checking z order
            StringID biggestWindow = 0;
            for (auto& s : hoveredSwapchains)
            {
                const int zOrder = windowManager->GetWindowZOrder(s->swapchainID);
                if (zOrder >= biggestZOrder)
                {
                    biggestZOrder = zOrder;
                    biggestWindow = s->swapchainID;
                }
            }
            m_hoveredSwapchainID = biggestWindow;
        }

        if (m_hoveredSwapchainID == 0)
        {
            const auto& mainSwp = Graphics::Backend::Get()->GetMainSwapchain();
            const Rect  r       = Rect(mainSwp.pos, mainSwp.size);
            if (Input::InputEngine::Get()->IsPointInRect(Input::InputEngine::Get()->GetMousePositionAbs(), r))
                m_hoveredSwapchainID = mainSwp.swapchainID;
        }
    }

    void EditorGUIManager::CreateImmediateGUI()
    {
        m_guis.push_back(ImmediateGUI());
        auto& gui   = m_guis[m_guis.size() - 1];
        auto& theme = gui.m_theme;

        gui.m_iconTexture = GetIconTexture()->GetSID();

        for (auto& pi : m_packedIcons)
        {
            ThemeIconData id;
            id.topLeft                     = pi.uvTL;
            id.bottomRight                 = pi.uvBR;
            id.size                        = pi.size;
            theme.m_icons[TO_SID(pi.name)] = id;
        }

        // Setup style
        const Color dark0 = Color(14.0f, 14.0f, 14.0f, 255.0f, true);
        const Color dark1 = Color(20.0f, 20.0f, 20.0f, 255.0f, true);
        const Color dark2 = Color(25.0f, 25.0f, 25.0f, 255.0f, true);

        const Color light0  = Color(35.0f, 35.0f, 35.0f, 255.0f, true);
        const Color light1  = Color(40.0f, 40.0f, 40.0f, 255.0f, true);
        const Color light15 = Color(55, 55, 55.0f, 255.0f, true);
        const Color light2  = Color(60.0f, 60.0f, 60.0f, 255.0f, true);
        const Color light3  = Color(80.0f, 80.0f, 80.0f, 255.0f, true);
        const Color light5  = Color(190.0f, 190.0f, 190.0f, 255.0f, true);

        theme.m_fonts[ThemeFont::Default]       = m_rubikFont->GetHandle(12);
        theme.m_fonts[ThemeFont::PopupMenuText] = m_rubikFont->GetHandle(13);

        theme.m_colors[ThemeColor::Error]                   = Color(160.0f, 30.0f, 30.0f, 255.0f, true);
        theme.m_colors[ThemeColor::Warn]                    = Color(60.0f, 60.0f, 30.0f, 255.0f, true);
        theme.m_colors[ThemeColor::Check]                   = Color(30.0f, 60.0f, 30.0f, 255.0f, true);
        theme.m_colors[ThemeColor::TextColor]               = Color::White;
        theme.m_colors[ThemeColor::Dark0]                   = dark0;
        theme.m_colors[ThemeColor::Light3]                  = light3;
        theme.m_colors[ThemeColor::Light5]                  = light5;
        theme.m_colors[ThemeColor::TopPanelBackground]      = dark1;
        theme.m_colors[ThemeColor::Window]                  = dark2;
        theme.m_colors[ThemeColor::DefaultBorderColor]      = Color(5.0f, 5.0f, 5.0f, 200.0f, true);
        theme.m_colors[ThemeColor::PopupBG]                 = light15;
        theme.m_colors[ThemeColor::PopupBorderColor]        = Color::Black;
        theme.m_colors[ThemeColor::ButtonBackground]        = light1;
        theme.m_colors[ThemeColor::ButtonHovered]           = light2;
        theme.m_colors[ThemeColor::ButtonPressed]           = light0;
        theme.m_colors[ThemeColor::ButtonBorder]            = dark0;
        theme.m_colors[ThemeColor::ButtonIconTint]          = Color(1, 1, 1, 1);
        theme.m_colors[ThemeColor::Highlight]               = Color(142, 34, 34, 255, true);
        theme.m_colors[ThemeColor::MenuBarPopupBG]          = light0;
        theme.m_colors[ThemeColor::MenuBarPopupBorderColor] = light1;
        theme.m_colors[ThemeColor::DockArea]                = dark1;
        theme.m_colors[ThemeColor::DockAreaBorder]          = light1;
        theme.m_colors[ThemeColor::AppBorder]               = light1;

        theme.m_properties[ThemeProperty::AAEnabled]          = LinaVG::Config.aaEnabled ? 1.0f : 0.0f;
        theme.m_properties[ThemeProperty::WindowItemPaddingX] = 12;
        theme.m_properties[ThemeProperty::WindowItemPaddingY] = 12;
        theme.m_properties[ThemeProperty::WindowItemSpacingX] = 12;
        theme.m_properties[ThemeProperty::WindowItemSpacingY] = 12;
        theme.m_properties[ThemeProperty::WindowRounding]     = 0.0f;

        theme.m_properties[ThemeProperty::ButtonRounding]        = 0.2f;
        theme.m_properties[ThemeProperty::ButtonBorderThickness] = 1.0f;
        theme.m_properties[ThemeProperty::ButtonIconFit]         = 0.75f;
        theme.m_properties[ThemeProperty::ButtonTextFit]         = 0.35f;

        theme.m_properties[ThemeProperty::PopupRounding]               = 0.0f;
        theme.m_properties[ThemeProperty::PopupBorderThickness]        = 1.0f;
        theme.m_properties[ThemeProperty::MenuBarPopupBorderThickness] = 1.0f;
        theme.m_properties[ThemeProperty::MenuBarItemsTooltipSpacing]  = 24.0f;
    }

    Drawable* EditorGUIManager::GetContentFromPanelRequest(EditorPanel panel)
    {
        if (panel == EditorPanel::Entities)
        {
        }
        return nullptr;
    }

    void EditorGUIManager::OnTick(const Event::ETick& ev)
    {
    
        // Adjust
        const Vector2 display   = m_subsys.renderEngine->GetScreen().DisplayResolution();
        const Vector2 screen    = m_subsys.renderEngine->GetScreen().Size();
        m_topPanel->m_rect.size = Vector2(screen.x, display.y * 0.084f);
        m_mainDockArea->m_rect  = Rect(Vector2(0, m_topPanel->m_rect.size.y), Vector2(screen.x, screen.y - m_topPanel->m_rect.size.y));

        // Get windows & make sure enough immediate guis available.
        const auto& windows          = m_subsys.renderEngine->GetWindowData();
        const int   totalWindowCount = static_cast<int>(windows.size());
        while (static_cast<int>(m_guis.size()) < totalWindowCount)
            CreateImmediateGUI();

        Vector<Graphics::WindowData> windowDatas;

        for (auto [sid, wd] : windows)
            windowDatas.push_back(wd);

        // Draw guis async.
        Taskflow tf;
        tf.for_each_index(0, static_cast<int>(windowDatas.size()), 1, [&](int i) {
            auto& wd = windowDatas[i];

            m_subsys.renderEngine->GetGUIBackend()->SetSwapchain(i, wd.swapchain);
            m_guis[i].m_currentSwaphchain  = wd.swapchain;
            m_guis[i].m_isSwapchainHovered = wd.swapchain->swapchainID == m_hoveredSwapchainID;
            m_guis[i].m_threadNumber       = i;

            if (wd.sid == LINA_MAIN_SWAPCHAIN_ID)
            {
                m_topPanel->SetSwapchain(wd.swapchain);
                m_mainDockArea->SetSwapchain(wd.swapchain);
                m_topPanel->m_gui     = &m_guis[0];
                m_mainDockArea->m_gui = &m_guis[0];
                m_topPanel->Draw();
                m_mainDockArea->Draw();
            }
            else
            {
                m_additionalDockAreas[i - 1]->m_gui = &m_guis[i - 1];
                m_additionalDockAreas[i - 1]->SetSwapchain(wd.swapchain);
                m_additionalDockAreas[i - 1]->Draw();
            }
        });

        JobSystem::Get()->GetMainExecutor().RunAndWait(tf);

        if (Input::InputEngine::Get()->GetKeyDown(LINA_KEY_R))
        {
            LaunchPanel(EditorPanel::Level);
        }

        if (Input::InputEngine::Get()->GetKeyDown(LINA_KEY_T))
        {
            LaunchPanel(EditorPanel::Entities);
        }

        if (Input::InputEngine::Get()->GetKeyDown(LINA_KEY_U))
        {
            LaunchPanel(EditorPanel::Global);
        }

        if (Input::InputEngine::Get()->GetKeyDown(LINA_KEY_O))
        {
            LaunchPanel(EditorPanel::Resources);
        }

        if (Input::InputEngine::Get()->GetKeyDown(LINA_KEY_K))
        {
            LaunchPanel(EditorPanel::Properties);
        }
    }

    static int  ctr = 0;
    static bool tgl = 0;

    void EditorGUIManager::LaunchPanel(EditorPanel panel)
    {
        // ? check if we should preserve some layout for the panel
        // receive last pos & size.
        const Vector2  lastPos   = panel == EditorPanel::Level ? Vector2(100, 100) : Vector2(500, 100);
        const Vector2  lastSize  = Vector2(500, 500);
        const String   panelName = "panel";
        const StringID sid       = TO_SID(panelName);

        const Bitmask16 mask = Graphics::RendererMask::RM_RenderGUI;

        if (tgl)
        {
            tgl = false;
            DestroyDockArea(sid);
            return;
        }

        SimpleAction act;

        act.Action = [this, lastSize, panelName, lastPos]() {
            Graphics::SurfaceRenderer* renderer = new Graphics::SurfaceRenderer();
            renderer->SetRenderMask(Graphics::RendererMask::RM_RenderGUI);
            m_subsys.renderEngine->CreateChildWindow(panelName, lastPos, lastSize, renderer);
            DockArea* area     = new DockArea(m_subsys);
            area->m_rect       = Rect(Vector2i::Zero, lastSize);
            area->m_swapchain  = renderer->GetSwapchain();
            area->m_detached   = true;
            area->m_guiManager = this;
            // area->m_content.push_back(GetContentFromPanelRequest(req.panelType));
            m_additionalDockAreas.push_back(area);
            tgl = true;
        };
        m_subsys.renderEngine->AddToActionSyncQueue(act);
    }
    void EditorGUIManager::DestroyDockArea(StringID sid)
    {
        SimpleAction act;
        act.Action = [this, sid]() {
            m_additionalDockAreas.erase(linatl::find_if(m_additionalDockAreas.begin(), m_additionalDockAreas.end(), [sid](DockArea* da) { return da->m_swapchain->swapchainID == sid; }));
            m_subsys.renderEngine->DestroyChildWindow(sid);
            tgl = false;
        };
        m_subsys.renderEngine->AddToActionSyncQueue(act);
    }
} // namespace Lina::Editor
