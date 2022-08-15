/*
Class: GUILayer

This class is pushed as an overlay layer to the render engine and is responsible for drawing the editor.
It inits panels, drawers etc. and is the main bridge of communication between editor components.

*/

#ifndef GUILAYER_HPP
#define GUILAYER_HPP

#include "Panels/EntitiesPanel.hpp"
#include "Panels/SystemsPanel.hpp"
#include "Panels/GlobalSettingsPanel.hpp"
#include "Panels/HeaderPanel.hpp"
#include "Panels/LevelPanel.hpp"
#include "Panels/LogPanel.hpp"
#include "Panels/MainToolbarPanel.hpp"
#include "Panels/ProfilerPanel.hpp"
#include "Panels/PropertiesPanel.hpp"
#include "Panels/ResourcesPanel.hpp"
#include "Panels/ProgressPanel.hpp"
#include "Panels/PreviewPanel.hpp"
#include "Panels/TextEditorPanel.hpp"
#include "Core/ShortcutManager.hpp"
#include "World/DefaultLevel.hpp"
#include "Panels/ResourceSelectorPanel.hpp"
#include "Data/Vector.hpp"

struct ImFont;

namespace Lina
{
    namespace World
    {
        class Level;
    }

    namespace Resources
    {
        class ResourceStorage;
    }

    namespace Event
    {
        struct EShutdown;
        struct EPostRender;
        struct EResourceProgressUpdated;
    } // namespace Event
} // namespace Lina

namespace Lina::Editor
{
    class EditorApplication;

    class GUILayer
    {

    public:
        GUILayer()  = default;
        ~GUILayer() = default;

        inline static GUILayer* Get()
        {
            return s_guiLayer;
        }

        void  Initialize();
        void  OnShutdown(const Event::EShutdown& ev);
        void  OnPostRender(const Event::EPostRender&);
        float GetDPIScale();

        inline ImFont* GetDefaultFont()
        {
            return m_defaultFont;
        }
        inline ImFont* GetBigFont()
        {
            return m_bigFont;
        }
        inline ImFont* GetMediumFont()
        {
            return m_mediumFont;
        }
        inline ImFont* GetIconFontSmall()
        {
            return m_iconFontSmall;
        }
        inline ImFont* GetIconFontDefault()
        {
            return m_iconFontDefault;
        }
        inline ImFont* GetTextEditorFont()
        {
            return m_textEditorFont;
        }
        inline const char* GetLinaLogoIcon()
        {
            return m_linaLogoIcon;
        }
        inline Vector2 GetDefaultWindowPadding()
        {
            return m_defaultWindowPadding;
        }
        inline Vector2 GetDefaultFramePadding()
        {
            return m_defaultFramePadding;
        }

        inline ShortcutManager& GetShortcutManager()
        {
            return m_shortcutManager;
        }

        // Menu bar item callback from header panel.
        void OnMenuBarElementClicked(const EMenuBarElementClicked& event);

        inline LevelPanel& GetLevelPanel()
        {
            return m_levelPanel;
        }
        inline PreviewPanel& GetPreviewPanel()
        {
            return m_previewPanel;
        }
        inline TextEditorPanel& GetTextEditorPanel()
        {
            return m_textEditorPanel;
        }
        inline ResourceSelectorPanel& GetResourceSelector()
        {
            return m_resourceSelectorPanel;
        }

        float                               m_headerSize = 0.0f;
        float                               m_footerSize = 20.0f;
        Map<const char*, const char*>  m_windowIconMap;
        Map<const char*, EditorPanel*> m_editorPanels;

    private:
        void DrawSplashScreen();
        void OnResourceLoadUpdated(const Event::EResourceProgressUpdated& ev);

        void DrawFPSCounter(int corner = 0);

    private:
        friend class EditorApplication;
        static GUILayer* s_guiLayer;

        Vector2                     m_defaultWindowPadding = Vector2(8, 8);
        Vector2                     m_defaultFramePadding  = Vector2(8, 2);
        const char*                 m_linaLogoIcon         = nullptr;
        ImFont*                     m_defaultFont          = nullptr;
        ImFont*                     m_bigFont              = nullptr;
        ImFont*                     m_mediumFont           = nullptr;
        ImFont*                     m_iconFontSmall        = nullptr;
        ImFont*                     m_iconFontDefault      = nullptr;
        ImFont*                     m_textEditorFont       = nullptr;
        ShortcutManager             m_shortcutManager;
        EntitiesPanel               m_entitiesPanel;
        SystemsPanel                m_systemsPanel;
        ResourcesPanel              m_resourcesPanel;
        LevelPanel                  m_levelPanel;
        PropertiesPanel             m_propertiesPanel;
        LogPanel                    m_logPanel;
        HeaderPanel                 m_headerPanel;
        ProfilerPanel               m_profilerPanel;
        GlobalSettingsPanel         m_globalSettingsPanel;
        MainToolbarPanel            m_toolbar;
        ResourceSelectorPanel       m_resourceSelectorPanel;
        ProgressPanel               m_progressPanel;
        PreviewPanel                m_previewPanel;
        TextEditorPanel             m_textEditorPanel;
        Resources::ResourceStorage* m_storage                  = nullptr;
        String                 m_currentlyLoadingResource = "";
        float                       percentage               = 0.0f;
        bool                        m_shouldDrawSplash         = false;
        bool                        m_shouldDrawProgressPanel  = false;
        Vector2                     m_minMonitorPos            = Vector2::Zero;
        Vector2                     m_maxMonitorPos            = Vector2::Zero;
        Vector2                     m_minMonitorWorkArea       = Vector2::Zero;
        Vector2                     m_maxMonitorWorkArea       = Vector2::Zero;
    };
} // namespace Lina::Editor

#endif
