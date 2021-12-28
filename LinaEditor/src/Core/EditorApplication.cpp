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

#include "Core/EditorApplication.hpp"

#include "Core/EditorCommon.hpp"
#include "Core/Engine.hpp"
#include "Core/GUILayer.hpp"
#include "Core/RenderEngineBackend.hpp"
#include "ECS/Components/CameraComponent.hpp"
#include "ECS/Components/EntityDataComponent.hpp"
#include "ECS/Components/FreeLookComponent.hpp"
#include "ECS/Systems/FreeLookSystem.hpp"
#include "EventSystem/LevelEvents.hpp"
#include "EventSystem/MainLoopEvents.hpp"
#include "Log/Log.hpp"
#include "Panels/ECSPanel.hpp"

using namespace ECS;

namespace Lina::Editor
{
    EditorApplication* EditorApplication::s_editorApplication = nullptr;

    void EditorApplication::Initialize()
    {
        LINA_TRACE("[Initialize] -> Editor Application ({0})", typeid(*this).name());

        s_editorApplication = this;
        m_guiLayer.Initialize();

        Event::EventSystem::Get()->Connect<Event::ELevelInitialized, &EditorApplication::LevelInitialized>(this);
        Event::EventSystem::Get()->Connect<Event::EPlayModeChanged, &EditorApplication::PlayModeChanged>(this);
        m_editorCameraSystem.Initialize(m_guiLayer.GetLevelPanel());
        m_editorCameraSystem.SystemActivation(true);

        Engine::Get()->AddToMainPipeline(m_editorCameraSystem);
    }

    void EditorApplication::Refresh()
    {
        m_guiLayer.Refresh();
    }

    void EditorApplication::LevelInitialized(const Event::ELevelInitialized& ev)
    {
        Registry* ecs = ECS::Registry::Get();

        auto singleView = ecs->view<ECS::EntityDataComponent>();

        if (ecs->GetEntity(EDITOR_CAMERA_NAME) == entt::null)
        {
            Entity              editorCamera = ecs->CreateEntity(EDITOR_CAMERA_NAME);
            EntityDataComponent cameraTransform;
            CameraComponent     cameraComponent;
            FreeLookComponent   freeLookComponent;
            ecs->emplace<CameraComponent>(editorCamera, cameraComponent);
            ecs->emplace<FreeLookComponent>(editorCamera, freeLookComponent);
            Graphics::RenderEngineBackend::Get()->GetCameraSystem()->SetActiveCamera(editorCamera);
            m_editorCameraSystem.SetEditorCamera(editorCamera);
            Refresh();
        }
        else
        {
            Entity editorCamera = ecs->GetEntity(EDITOR_CAMERA_NAME);
            Graphics::RenderEngineBackend::Get()->GetCameraSystem()->SetActiveCamera(editorCamera);
            ecs->get<FreeLookComponent>(editorCamera).SetIsEnabled(true);
            m_editorCameraSystem.SetEditorCamera(editorCamera);
        }
    }

    void EditorApplication::PlayModeChanged(const Event::EPlayModeChanged& playMode)
    {
        bool enabled = playMode.m_playMode;

        Registry* ecs          = ECS::Registry::Get();
        Entity    editorCamera = ecs->GetEntity(EDITOR_CAMERA_NAME);

        if (editorCamera != entt::null)
        {
            if (enabled)
            {
                if (Graphics::RenderEngineBackend::Get()->GetCameraSystem()->GetActiveCamera() == editorCamera)
                    Graphics::RenderEngineBackend::Get()->GetCameraSystem()->SetActiveCamera(entt::null);

                ecs->get<FreeLookComponent>(editorCamera).SetIsEnabled(false);
                m_editorCameraSystem.SystemActivation(false);
            }
            else
            {
                Graphics::RenderEngineBackend::Get()->GetCameraSystem()->SetActiveCamera(editorCamera);
                ecs->get<FreeLookComponent>(editorCamera).SetIsEnabled(true);
                m_editorCameraSystem.SystemActivation(true);
            }
        }
    }

} // namespace Lina::Editor
