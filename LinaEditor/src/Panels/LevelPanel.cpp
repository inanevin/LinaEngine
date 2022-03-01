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
#include "Panels/LevelPanel.hpp"

#include "Core/Application.hpp"
#include "Core/CommonECS.hpp"
#include "Core/CommonInput.hpp"
#include "Core/EditorApplication.hpp"
#include "Core/EditorCommon.hpp"
#include "Core/GUILayer.hpp"
#include "Core/InputBackend.hpp"
#include "Core/InputMappings.hpp"
#include "Core/RenderEngineBackend.hpp"
#include "ECS/Components/CameraComponent.hpp"
#include "ECS/Components/EntityDataComponent.hpp"
#include "ECS/Components/MeshRendererComponent.hpp"
#include "ECS/Components/ModelRendererComponent.hpp"
#include "EventSystem/GraphicsEvents.hpp"
#include "EventSystem/LevelEvents.hpp"
#include "IconsFontAwesome5.h"
#include "Log/Log.hpp"
#include "Math/Math.hpp"
#include "Physics/Raycast.hpp"
#include "Utility/UtilityFunctions.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "imgui/imgui.h"

#include <imgui/imguizmo/ImGuizmo.h>

static Matrix gridLineMatrix = Matrix::Identity();
static Matrix modelMatrix    = Matrix::Identity();
#define GRID_SIZE 1000

namespace Lina::Editor
{
    ImGuizmo::OPERATION GetOperationFromInt(int i)
    {
        switch (i)
        {
        case 0:
            return ImGuizmo::OPERATION::TRANSLATE;
        case 1:
            return ImGuizmo::OPERATION::ROTATE;
        case 2:
            return ImGuizmo::OPERATION::SCALE;
        case 3:
            return ImGuizmo::OPERATION::BOUNDS;
        default:
            return ImGuizmo::OPERATION::TRANSLATE;
        }
    }
    void LevelPanel::Initialize(const char* id, const char* icon)
    {
        EditorPanel::Initialize(id, icon);
        Event::EventSystem::Get()->Connect<EEntityUnselected, &LevelPanel::Unselected>(this);
        Event::EventSystem::Get()->Connect<EEntitySelected, &LevelPanel::EntitySelected>(this);
        Event::EventSystem::Get()->Connect<Event::ELevelUninstalled, &LevelPanel::LevelUninstalled>(this);
        Event::EventSystem::Get()->Connect<ETransformGizmoChanged, &LevelPanel::OnTransformGizmoChanged>(this);
        Event::EventSystem::Get()->Connect<ETransformPivotChanged, &LevelPanel::OnTransformPivotChanged>(this);
        Event::EventSystem::Get()->Connect<EShortcut, &LevelPanel::OnShortcut>(this);
        Event::EventSystem::Get()->Connect<Event::EKeyCallback, &LevelPanel::OnKeyCallback>(this);
        Event::EventSystem::Get()->Connect<Event::EMouseButtonCallback, &LevelPanel::OnMouseButtonCallback>(this);

        m_shouldShowGizmos      = true;
        m_isTransformModeGlobal = true;
        m_transformOperation    = 0;
    }

    void LevelPanel::Draw()
    {
        ImGuizmo::BeginFrame();

        if (m_show)
        {
            ImGuiWindowFlags emptyChildFlags = 0 | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings;

            Graphics::RenderEngineBackend* renderEngine = Graphics::RenderEngineBackend::Get();
            m_windowFlags |= ImGuiWindowFlags_NoScrollbar;

            if (Begin())
            {
                WidgetsUtility::FramePaddingY(0.0f);

                const ImVec2 sceneWindowPos  = WidgetsUtility::GetWindowPosWithContentRegion();
                const ImVec2 sceneWindowSize = WidgetsUtility::GetWindowSizeWithContentRegion();

                // Resize engine display.
                float windowAspect = sceneWindowSize.x / sceneWindowSize.y;

                if (renderEngine->GetCameraSystem()->GetAspectRatio() != windowAspect)
                    renderEngine->GetCameraSystem()->SetAspectRatio(windowAspect);

                ImGui::BeginChild("finalImage");

                m_isFocused = ImGui::IsWindowFocused();
                m_isHovered = ImGui::IsWindowHovered();

                if (m_isHovered && (ImGui::IsMouseClicked(ImGuiMouseButton_Left) || ImGui::IsMouseClicked(ImGuiMouseButton_Right) || ImGui::IsMouseClicked(ImGuiMouseButton_Middle)))
                {
                    ImGui::SetWindowFocus();
                }
#pragma warning(disable : 4312) // ImTextureID requires a void* conversion.

                // Draw final image.
                ImVec2 imageRectMin = sceneWindowPos;
                ImVec2 imageRectMax = ImVec2(sceneWindowPos.x + sceneWindowSize.x, sceneWindowPos.y + sceneWindowSize.y);

                if (m_drawMode == DrawMode::FinalImage)
                    ImGui::GetWindowDrawList()->AddImage((void*)(renderEngine->GetFinalImage()), imageRectMin, imageRectMax, ImVec2(0, 1), ImVec2(1, 0));
                else if (m_drawMode == DrawMode::ShadowMap)
                    ImGui::GetWindowDrawList()->AddImage((void*)(renderEngine->GetShadowMapImage()), imageRectMin, imageRectMax, ImVec2(0, 1), ImVec2(1, 0));

                if (Engine::Get()->GetPlayMode())
                {
                    if (m_borderAlpha < 1.0f)
                        m_borderAlpha = Math::Lerp(m_borderAlpha, 1.1f, Engine::Get()->GetRawDelta() * 1.2f);

                    ImGui::GetWindowDrawList()->AddRect(imageRectMin, ImVec2(imageRectMax.x, imageRectMax.y - 4), ImGui::ColorConvertFloat4ToU32(ImVec4(0.33f, 0.54f, 0.78f, m_borderAlpha)), 0, 0, 5);
                }
                else
                {
                    if (m_borderAlpha != 0.0f)
                        m_borderAlpha = 0.0f;
                }

                const ImVec2 windowPos    = ImGui::GetWindowPos();
                const ImVec2 windowSize   = ImGui::GetWindowSize();
                const ImRect confineSpace = ImRect(windowPos, ImVec2(windowPos.x + windowSize.x, windowPos.y + windowSize.y));
                WidgetsUtility::TransformOperationTools("##levelpanel_transformops", confineSpace);
                WidgetsUtility::PlayOperationTools("##levelpanel_playops", confineSpace);

                // Draw gizmos.
                ImGuiIO& io = ImGui::GetIO();
                ImGuizmo::Enable(true);
                ImGuizmo::SetOrthographic(false);
                ImGuizmo::SetDrawlist();
                ImGuizmo::SetRect(imageRectMin.x, imageRectMin.y, imageRectMax.x - imageRectMin.x, imageRectMax.y - imageRectMin.y);
                ImGui::PushClipRect(imageRectMin, imageRectMax, false);
                ProcessInput();
                DrawGizmos();

                // Show a warning box if no camera is available.
                if (renderEngine->GetCameraSystem()->GetActiveCameraComponent() == nullptr)
                {
                    ImVec2 size = ImVec2(240, 100);
                    ImVec2 pos  = ImVec2(sceneWindowPos.x + sceneWindowSize.x / 2.0f - (size.x / 2.0f), sceneWindowPos.y + sceneWindowSize.y / 2.0f - (size.y / 2.0f));
                    ImGui::SetNextWindowPos(pos);
                    ImGui::SetNextWindowBgAlpha(0.8f);
                    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 4);
                    ImGui::PushFont(GUILayer::Get()->GetBigFont());
                    ImGui::BeginChild("##scenepanel_nocam", size, false, emptyChildFlags);
                    const char* noCamAvab        = "No camera available!";
                    const char* latestViewMatrix = "(Using latest view matrix)";
                    ImVec2      windowSize       = ImGui::GetWindowSize();
                    ImVec2      textSize1        = ImGui::CalcTextSize(noCamAvab);
                    ImVec2      textSize2        = ImGui::CalcTextSize(latestViewMatrix);
                    ImGui::SetCursorPosX((windowSize.x - textSize1.x) * 0.5f);
                    ImGui::SetCursorPosY((windowSize.y - textSize1.y) * 0.5f - 10);
                    ImGui::Text("No camera available!");
                    ImGui::SetCursorPosX((windowSize.x - textSize2.x) * 0.5f);
                    ImGui::SetCursorPosY((windowSize.y - textSize2.y) * 0.5f + 10);
                    ImGui::Text(latestViewMatrix);
                    ImGui::PopFont();
                    ImGui::PopStyleVar();
                    ImGui::EndChild();
                }

                ImGui::EndChild();

                // Model drag & drop.
                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(RESOURCES_MOVEMODEL_ID))
                    {
                        IM_ASSERT(payload->DataSize == sizeof(StringIDType));
                        renderEngine->GetModelNodeSystem()->CreateModelHierarchy(Resources::ResourceStorage::Get()->GetResource<Graphics::Model>(*(StringIDType*)payload->Data));
                    }
                    ImGui::EndDragDropTarget();
                }

                WidgetsUtility::PopStyleVar();
                End();
            }
        }
    }

    void LevelPanel::EntitySelected(const EEntitySelected& ev)
    {
        m_selectedEntity = ev.m_entity;
    }

    void LevelPanel::Unselected(const EEntityUnselected& ev)
    {
        m_selectedEntity = entt::null;
    }

    void LevelPanel::LevelUninstalled(const Event::ELevelUninstalled& ev)
    {
        Unselected(EEntityUnselected());
    }

    void LevelPanel::ProcessInput()
    {
        if (ImGui::IsWindowHovered() || ImGui::IsWindowFocused())
        {
            // Mouse picking
            if (Input::InputEngineBackend::Get()->GetMouseButtonDown(0))
            {
                auto*                     reg       = ECS::Registry::Get();
                auto*                     rend      = Graphics::RenderEngineBackend::Get();
                ECS::Entity               editorCam = EditorApplication::Get()->GetCameraSystem().GetEditorCamera();
                ECS::EntityDataComponent& camData   = reg->get<ECS::EntityDataComponent>(editorCam);
                Vector2                   windowPos = Vector2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
                Vector2                   mousePos  = Vector2(ImGui::GetMousePos().x - windowPos.x, ImGui::GetMousePos().y - windowPos.y);

                const Vector3 camLocation = camData.GetLocation();
                const Vector3 clickPos    = Graphics::RenderEngineBackend::Get()->GetCameraSystem()->ScreenToWorldCoordinates(Vector3(mousePos.x, mousePos.y, 500.0f));
                const Vector3 rayDir      = (clickPos - camLocation).Normalized();

                reg->each([reg, camLocation, rayDir, editorCam](auto entity) {
                    if (entity != editorCam)
                    {
                        // ECS::EntityDataComponent& entityData = reg->get<ECS::EntityDataComponent>(entity);
                        // ECS::PhysicsComponent& entityPhy = reg->get<ECS::PhysicsComponent>(entity);
                        // Physics::HitInfo hitInfo = Physics::RaycastPose(camLocation, rayDir, entityData.GetLocation(), entityData.GetBoundsHalfExtents(), 500.0f);
                        //
                        // if (hitInfo.m_hitCount > 0)
                        //{
                        //	Event::EventSystem::Get()->Trigger<EEntitySelected>(EEntitySelected{ entity });
                        //	return;
                        //}
                    }
                });
            }
        }
    }

    void LevelPanel::DrawGizmos()
    {
        if (Engine::Get()->GetPlayMode() || !m_shouldShowGizmos)
            return;

        Graphics::RenderEngineBackend* renderEngine = Graphics::RenderEngineBackend::Get();
        ECS::Entity                    editorCam    = EditorApplication::Get()->GetCameraSystem().GetEditorCamera();
        Matrix&                        view         = renderEngine->GetCameraSystem()->GetViewMatrix();
        Matrix&                        projection   = renderEngine->GetCameraSystem()->GetProjectionMatrix();
        auto*                          reg          = ECS::Registry::Get();

        // Handle entity transformation manipulation.
        if (m_selectedEntity != entt::null)
        {
            ECS::EntityDataComponent& data = reg->get<ECS::EntityDataComponent>(m_selectedEntity);
            ECS::PhysicsComponent&    phy  = reg->get<ECS::PhysicsComponent>(m_selectedEntity);

            glm::mat4 object = data.ToMatrix();

            bool useDisabled = (phy.GetSimType() == Physics::SimulationType::Static) || (phy.GetSimType() == Physics::SimulationType::Dynamic && !phy.GetIsKinematic());
            ImGuizmo::SetCanUse(!useDisabled);
            ImGuizmo::SetThicknessMultiplier(1.0f);
            ImGuizmo::SetLineLengthMultiplier(1.0f);
            ImGuizmo::EnablePlanes(true);

            float          matrixTranslation[3], matrixRotation[3], matrixScale[3];
            ImGuizmo::MODE gizmoMode = m_isTransformModeGlobal ? ImGuizmo::MODE::WORLD : ImGuizmo::MODE::LOCAL;
            ImGuizmo::Manipulate(&view[0][0], &projection[0][0], GetOperationFromInt(m_transformOperation), gizmoMode, &object[0][0]);
            ImGuizmo::DecomposeMatrixToComponents(&object[0][0], matrixTranslation, matrixRotation, matrixScale);

            if (ImGuizmo::IsUsing())
            {
                glm::vec3 rot           = data.GetRotationAngles();
                glm::vec3 deltaRotation = glm::vec3(matrixRotation[0], matrixRotation[1], matrixRotation[2]) - rot;
                data.SetRotationAngles(rot + deltaRotation);
            }

            data.SetLocation(Vector3(matrixTranslation[0], matrixTranslation[1], matrixTranslation[2]));
            data.SetScale(Vector3(matrixScale[0], matrixScale[1], matrixScale[2]));

            // Draw the selected entities bounding box.
            if (m_shouldShowBounds)
            {
                std::vector<Vector3> boundingBoxPosition;
                std::vector<Vector3> boundingBoxHalfSize;
                if (renderEngine->GetFrustumSystem()->GetAllBoundsInEntity(m_selectedEntity, boundingBoxPosition, boundingBoxHalfSize))
                {
                    for (int i = 0; i < boundingBoxPosition.size(); i++)
                        Event::EventSystem::Get()->Trigger<Event::EDrawBox>(Event::EDrawBox{boundingBoxPosition[i], boundingBoxHalfSize[i], Color::Red, 2.5f});
                }

            }
        }

        // Draw scene orientation gizmo
        if (editorCam != entt::null)
        {
            ImGuizmo::SetCanUse(false);
            ImGuizmo::SetThicknessMultiplier(0.8f);
            ImGuizmo::SetLineLengthMultiplier(0.5f);
            ImGuizmo::EnablePlanes(false);
            Transformation            sceneOrientationTransform;
            ECS::EntityDataComponent& data       = reg->get<ECS::EntityDataComponent>(editorCam);
            const Vector3             camLoc     = data.GetLocation();
            sceneOrientationTransform.m_location = ECS::CameraSystem::ViewportToWorldCoordinates(Vector3(0.95f, 0.9f, 2.0f));
            sceneOrientationTransform.m_rotation = Quaternion::LookAt(sceneOrientationTransform.m_location, Vector3(camLoc.x, camLoc.y, camLoc.z + 300), Vector3::Up);
            glm::mat4 sceneOrientation           = sceneOrientationTransform.ToMatrix();
            ImGuizmo::Manipulate(&view[0][0], &projection[0][0], ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::MODE::WORLD, &sceneOrientation[0][0]);
        }

        // ImGuizmo::DrawGrid(&view[0][0], &projection[0][0], &gridLineMatrix[0][0], GRID_SIZE);
    }

    void LevelPanel::OnTransformGizmoChanged(const ETransformGizmoChanged& ev)
    {
        m_transformOperation = ev.m_currentOperation;
    }

    void LevelPanel::OnTransformPivotChanged(const ETransformPivotChanged& ev)
    {
        m_isTransformModeGlobal = ev.m_isGlobal;
    }

    void LevelPanel::OnShortcut(const EShortcut& ev)
    {
        if (ev.m_name.compare("playmode") == 0)
        {
            if (Engine::Get()->GetPlayMode())
            {
                Engine::Get()->SetPlayMode(false);
                Input::InputEngineBackend::Get()->SetCursorMode(Input::CursorMode::Visible);
                // unconfine mouse
            }
            else
            {
                Engine::Get()->SetPlayMode(true);
                Input::InputEngineBackend::Get()->SetCursorMode(Input::CursorMode::Disabled);
                // confine mouse
            }
        }
    }

    void LevelPanel::OnMouseButtonCallback(const Event::EMouseButtonCallback& ev)
    {
        if (Engine::Get()->GetPlayMode() && m_isFocused && ev.m_action == Input::InputAction::Pressed && ev.m_button == LINA_MOUSE_1)
        {
            Input::InputEngineBackend::Get()->SetCursorMode(Input::CursorMode::Disabled);
            // confine
        }
    }

    void LevelPanel::OnKeyCallback(const Event::EKeyCallback& ev)
    {
        if (Engine::Get()->GetPlayMode())
        {
            if (ev.m_action == Input::InputAction::Pressed && ev.m_key == LINA_KEY_ESCAPE)
            {
                Input::CursorMode currentMode = Input::InputEngineBackend::Get()->GetCursorMode();

                if (currentMode == Input::CursorMode::Visible)
                {
                    Engine::Get()->SetPlayMode(false);
                }
                else
                {
                    Input::InputEngineBackend::Get()->SetCursorMode(Input::CursorMode::Visible);
                    // unconfine
                }
            }
        }
        if (!m_isFocused)
            return;

        if (ev.m_action == Input::InputAction::Pressed && ev.m_key == LINA_KEY_Q)
            Event::EventSystem::Get()->Trigger<ETransformGizmoChanged>(ETransformGizmoChanged{0});
        if (ev.m_action == Input::InputAction::Pressed && ev.m_key == LINA_KEY_E)
            Event::EventSystem::Get()->Trigger<ETransformGizmoChanged>(ETransformGizmoChanged{1});
        if (ev.m_action == Input::InputAction::Pressed && ev.m_key == LINA_KEY_R)
            Event::EventSystem::Get()->Trigger<ETransformGizmoChanged>(ETransformGizmoChanged{2});
        if (ev.m_action == Input::InputAction::Pressed && ev.m_key == LINA_KEY_T)
            Event::EventSystem::Get()->Trigger<ETransformPivotChanged>(ETransformPivotChanged{!m_isTransformModeGlobal});
    }

} // namespace Lina::Editor
