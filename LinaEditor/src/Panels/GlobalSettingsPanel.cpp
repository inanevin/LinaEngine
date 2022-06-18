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

#include "Panels/GlobalSettingsPanel.hpp"

#include "Core/Application.hpp"
#include "Core/EditorCommon.hpp"
#include "Core/RenderEngine.hpp"
#include "EventSystem/LevelEvents.hpp"
#include "IconsFontAwesome5.h"
#include "Rendering/Material.hpp"
#include "Rendering/RenderSettings.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "World/Level.hpp"
#include "Drawers/ClassDrawer.hpp"
#include "Core/EditorSettings.hpp"
#include "Core/EditorApplication.hpp"
#include "imgui/imgui.h"

namespace Lina::Editor
{
#define CURSORPOS_X_LABELS     12
#define CURSORPOS_XPERC_VALUES 0.30f

    void GlobalSettingsPanel::Initialize(const char* id, const char* icon)
    {
        EditorPanel::Initialize(id, icon);
        m_show = true;
    }

    void GlobalSettingsPanel::Draw()
    {
        if (m_show)
        {
            if (Begin())
            {

                auto*              currentLevel               = World::Level::GetCurrent();
                const StringIDType sidBefore                  = currentLevel->m_skyboxMaterial.m_sid;
                bool               anySettingsChanged         = false;
                auto&              engineSettings             = Engine::Get()->GetEngineSettings();
               
                if (ClassDrawer::DrawClass(GetTypeID<World::Level>(), entt::forward_as_meta(*currentLevel), true))
                {
                    if (currentLevel->m_skyboxMaterial.m_value == nullptr)
                    {
                        Graphics::Material* defaultSkybox      = Graphics::RenderEngine::Get()->GetDefaultSkyboxMaterial();
                        currentLevel->m_skyboxMaterial.m_value = defaultSkybox;
                        currentLevel->m_skyboxMaterial.m_sid   = defaultSkybox->GetSID();
                    }

                    Graphics::RenderEngine::Get()->SetSkyboxMaterial(currentLevel->m_skyboxMaterial.m_value);
                    Graphics::RenderEngine::Get()->GetLightingSystem()->SetAmbientColor(currentLevel->m_ambientColor);
                    anySettingsChanged = true;
                }

                if (ClassDrawer::DrawClass(GetTypeID<Graphics::RenderSettings>(), entt::forward_as_meta(Engine::Get()->GetEngineSettings().GetRenderSettings()), true))
                {
                    anySettingsChanged = true;
                    Graphics::RenderEngine::Get()->UpdateRenderSettings();
                }

                if (ClassDrawer::DrawClass(GetTypeID<EditorSettings>(), entt::forward_as_meta(EditorApplication::Get()->GetEditorSettings()), true))
                {
                    Resources::SaveArchiveToFile<EditorSettings>("editor.linasettings", EditorApplication::Get()->GetEditorSettings());
                }

                if (anySettingsChanged)
                {
                    Resources::SaveArchiveToFile<EngineSettings>("engine.linasettings", Engine::Get()->GetEngineSettings());

                }

                End();
            }
        }
    }

} // namespace Lina::Editor
