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
#include "EventSystem/WindowEvents.hpp"
#include "Log/Log.hpp"
#include "Panels/EntitiesPanel.hpp"
#include "Widgets/WidgetsUtility.hpp"

using namespace ECS;

namespace Lina::Editor
{
    EditorApplication*     EditorApplication::s_editorApplication = nullptr;
    ECS::FreeLookComponent freeLookCopy;
    Vector3                editorCameraLocationCopy;
    Quaternion             editorCameraRotationCopy;
    ECS::CameraComponent   cameraCopy;

    EditorApplication::~EditorApplication()
    {
        for (auto& [sid, buffer] : m_previewBuffers)
        {
            delete buffer.m_renderBuffer;
            delete buffer.m_rtTexture;
            delete buffer.m_rt;
        }
    }

    void EditorApplication::Initialize()
    {
        LINA_TRACE("[Initialize] -> Editor Application ({0})", typeid(*this).name());

        s_editorApplication   = this;
        m_guiLayer.s_guiLayer = &m_guiLayer;
        m_guiLayer.Initialize();
        Event::EventSystem::Get()->Connect<Event::ELevelInstalled, &EditorApplication::OnLevelInstalled>(this);
        Event::EventSystem::Get()->Connect<Event::EPlayModeChanged, &EditorApplication::PlayModeChanged>(this);
        Event::EventSystem::Get()->Connect<Event::EPreSerializingLevel, &EditorApplication::OnPreSerializingLevel>(this);
        Event::EventSystem::Get()->Connect<Event::ESerializedLevel, &EditorApplication::OnSerializedLevel>(this);
        Event::EventSystem::Get()->Connect<Event::EWindowResized, &EditorApplication::OnWindowResized>(this);
        Event::EventSystem::Get()->Connect<Event::EResourceLoadCompleted, &EditorApplication::OnResourceLoadCompleted>(this);
        Event::EventSystem::Get()->Connect<Event::EResourceReloaded, &EditorApplication::OnResourceReloaded>(this);
        Event::EventSystem::Get()->Connect<Event::EResourcePathUpdated, &EditorApplication::OnResourcePathUpdated>(this);

        m_editorCameraSystem.Initialize("Editor Camera System", m_guiLayer.GetLevelPanel());
        m_editorCameraSystem.SystemActivation(true);

        Engine::Get()->AddToMainPipeline(m_editorCameraSystem);
    }

    void EditorApplication::OnLevelInstalled(const Event::ELevelInstalled& ev)
    {
        Registry* ecs = ECS::Registry::Get();

        auto singleView = ecs->view<ECS::EntityDataComponent>();

        Entity              editorCamera = ecs->CreateEntity(EDITOR_CAMERA_NAME);
        EntityDataComponent cameraTransform;
        CameraComponent     cameraComponent;
        FreeLookComponent   freeLookComponent;
        ecs->emplace<CameraComponent>(editorCamera, cameraComponent);
        ecs->emplace<FreeLookComponent>(editorCamera, freeLookComponent);
        Graphics::RenderEngineBackend::Get()->GetCameraSystem()->SetActiveCamera(editorCamera);
        m_editorCameraSystem.SetEditorCamera(editorCamera);

        // When the first level is loaded, iterate all model/material/shader resources & take snapshots for each.
        if (!m_snapshotsTaken)
        {
            m_snapshotsTaken    = true;
            auto* storage       = Resources::ResourceStorage::Get();
            auto& modelCache    = storage->GetCache<Graphics::Model>();
            auto& materialCache = storage->GetCache<Graphics::Material>();
            auto* sphere        = storage->GetResource<Graphics::Model>("Resources/Engine/Meshes/Primitives/Sphere.fbx");

            for (auto& [sid, ptr] : modelCache)
                TakeModelSnapshot(sid, sid, (StringIDType)0);

            for (auto& [sid, ptr] : materialCache)
                TakeModelSnapshot(sid, sphere->GetSID(), sid);
        }
    }

    void EditorApplication::OnPreSerializingLevel(const Event::EPreSerializingLevel& ev)
    {
        // Copy the components of editor camera and delete it so that it's not serialized along with the level
        auto*  ecs               = ECS::Registry::Get();
        Entity editorCamera      = ecs->GetEntity(EDITOR_CAMERA_NAME);
        cameraCopy               = ecs->get<ECS::CameraComponent>(editorCamera);
        auto& data               = ecs->get<ECS::EntityDataComponent>(editorCamera);
        editorCameraLocationCopy = data.GetLocation();
        editorCameraRotationCopy = data.GetRotation();
        freeLookCopy             = ecs->get<ECS::FreeLookComponent>(editorCamera);
        ecs->DestroyEntity(editorCamera);
    }

    void EditorApplication::OnSerializedLevel(const Event::ESerializedLevel& ev)
    {
        // Re-crate the editor camera from copied contents.
        auto*  ecs          = ECS::Registry::Get();
        Entity editorCamera = ecs->CreateEntity(EDITOR_CAMERA_NAME);
        auto&  data         = ecs->get<ECS::EntityDataComponent>(editorCamera);
        data.SetLocation(editorCameraLocationCopy);
        data.SetRotation(editorCameraRotationCopy);
        ecs->emplace<ECS::FreeLookComponent>(editorCamera, freeLookCopy);
        ecs->emplace<ECS::CameraComponent>(editorCamera, cameraCopy);
        Graphics::RenderEngineBackend::Get()->GetCameraSystem()->SetActiveCamera(editorCamera);
        m_editorCameraSystem.SetEditorCamera(editorCamera);
    }

    void EditorApplication::OnWindowResized(const Event::EWindowResized& ev)
    {
        // resize all snapshot buffers.

        auto*          renderEngine = Graphics::RenderEngineBackend::Get();
        auto*          renderDevice = renderEngine->GetRenderDevice();
        auto           params       = renderEngine->GetPrimaryRTParams();
        const Vector2i screenSize   = renderEngine->GetScreenSize();

        for (auto& [sid, buffer] : m_previewBuffers)
        {
            renderDevice->ResizeRTTexture(buffer.m_rtTexture->GetID(), screenSize, params.m_textureParams.m_internalPixelFormat, params.m_textureParams.m_pixelFormat);
        }
    }

    void EditorApplication::OnResourceLoadCompleted(const Event::EResourceLoadCompleted& ev)
    {
        // Only for resources loaded after the initial bulk-load.
        // Prepare editor camera, add a new buffer for the resource, take & store a snapshot, reset the editor camera.
        if (m_snapshotsTaken && ev.m_tid == GetTypeID<Graphics::Model>())
        {
            TakeModelSnapshot(ev.m_sid, ev.m_sid, (StringIDType)0);
        }
        if (m_snapshotsTaken && ev.m_tid == GetTypeID<Graphics::Material>())
        {
            auto* storage = Resources::ResourceStorage::Get();
            auto* sphere  = storage->GetResource<Graphics::Model>("Resources/Engine/Meshes/Primitives/Sphere.fbx");
            TakeModelSnapshot(ev.m_sid, sphere->GetSID(), ev.m_sid);
        }
    }

    void EditorApplication::OnResourceReloaded(const Event::EResourceReloaded& ev)
    {
        Event::EResourceLoadCompleted load;
        load.m_sid = ev.m_sid;
        load.m_tid = ev.m_tid;
        OnResourceLoadCompleted(load);
    }

    void EditorApplication::OnResourcePathUpdated(const Event::EResourcePathUpdated& ev)
    {
        if (m_previewBuffers.find(ev.m_previousStringID) != m_previewBuffers.end())
        {
            m_previewBuffers[ev.m_newStringID] = m_previewBuffers[ev.m_previousStringID];
            m_previewBuffers.erase(ev.m_previousStringID);
        }
    }

    void EditorApplication::TakeModelSnapshot(StringIDType bufferSid, StringIDType modelSid, StringIDType materialSid)
    {
        WidgetsUtility::SaveEditorCameraBeforeSnapshot(1.0f);
        WidgetsUtility::SetEditorCameraForSnapshot();
        auto* target       = AddSnapshotBuffer(bufferSid);
        auto* renderEngine = Graphics::RenderEngineBackend::Get();
        auto* overrideMat  = materialSid == 0 ? nullptr : Resources::ResourceStorage::Get()->GetResource<Graphics::Material>(materialSid);
        renderEngine->RenderModelPreview(Resources::ResourceStorage::Get()->GetResource<Graphics::Model>(modelSid), Matrix::Identity(), target, overrideMat);
        WidgetsUtility::ResetEditorCamera();
    }

    Graphics::RenderTarget* EditorApplication::AddSnapshotBuffer(StringIDType sid)
    {
        if (m_previewBuffers.find(sid) != m_previewBuffers.end())
            return m_previewBuffers[sid].m_rt;

        // We create a new buffer for each model resource, each buffer holds a render target, render buffer and a corresponding texture.
        auto& buffer                                      = m_previewBuffers[sid];
        buffer.m_renderBuffer                             = new Graphics::RenderBuffer();
        buffer.m_rtTexture                                = new Graphics::Texture();
        buffer.m_rt                                       = new Graphics::RenderTarget();
        auto*                             renderEngine    = Graphics::RenderEngineBackend::Get();
        const Vector2i                    screenSize      = renderEngine->GetScreenSize();
        const Graphics::SamplerParameters primaryRTParams = renderEngine->GetPrimaryRTParams();
        buffer.m_rtTexture->ConstructRTTexture(screenSize, primaryRTParams, false);
        buffer.m_renderBuffer->Construct(Graphics::RenderBufferStorage::STORAGE_DEPTH, screenSize);
        buffer.m_rt->Construct(*buffer.m_rtTexture, Graphics::TextureBindMode::BINDTEXTURE_TEXTURE2D, Graphics::FrameBufferAttachment::ATTACHMENT_COLOR, Graphics::FrameBufferAttachment::ATTACHMENT_DEPTH, buffer.m_renderBuffer->GetID());
        return buffer.m_rt;
    }

    uint32 EditorApplication::GetSnapshotTexture(StringIDType sid)
    {
        return m_previewBuffers[sid].m_rtTexture->GetID();
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
