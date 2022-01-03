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

static ImGuizmo::OPERATION currentTransformGizmoOP   = ImGuizmo::OPERATION::TRANSLATE;
static ImGuizmo::MODE      currentTransformGizmoMode = ImGuizmo::MODE::WORLD;
static Matrix              gridLineMatrix            = Matrix::Identity();
static Matrix              modelMatrix               = Matrix::Identity();
static ImVec2              previousWindowSize;
#define GRID_SIZE 1000

namespace Lina::Editor
{
    bool levelPanelFirstRun = false;

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

        m_shouldShowGizmos = true;
    }

    void LevelPanel::Draw()
    {
        ImGuizmo::BeginFrame();

        if (m_show)
        {
            ImGuiWindowFlags emptyChildFlags = 0 | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings;

            Graphics::RenderEngineBackend* renderEngine = Graphics::RenderEngineBackend::Get();

            if (Begin())
            {
                WidgetsUtility::FramePaddingY(0.0f);

                ImVec2 sceneWindowPos  = WidgetsUtility::GetWindowPosWithContentRegion();
                ImVec2 sceneWindowSize = WidgetsUtility::GetWindowSizeWithContentRegion();

                // Set Focus
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) || ImGui::IsMouseClicked(ImGuiMouseButton_Right))
                {
                    ImVec2 min = ImVec2(sceneWindowPos.x, sceneWindowPos.y);
                    ImVec2 max = ImVec2(min.x + sceneWindowSize.x, min.y + sceneWindowSize.y);

                    if (ImGui::IsMouseHoveringRect(min, max))
                        m_isFocused = true;
                    else
                        m_isFocused = false;
                }

                ImGui::BeginChild("finalImage");

                // Get game viewport aspect.
                Vector2 vpSize = renderEngine->GetScreenSize();
                float   aspect = (float)vpSize.x / (float)vpSize.y;

                // Resize engine display.
                if ((sceneWindowSize.x != previousWindowSize.x || sceneWindowSize.y != previousWindowSize.y))
                {
                    Graphics::RenderEngineBackend::Get()->SetScreenDisplay(Vector2i(0, 0), Vector2i((unsigned int)(sceneWindowSize.x), (unsigned int)(sceneWindowSize.y)));
                    previousWindowSize = sceneWindowSize;
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

                /// <summary>
                /// Scene Settings - window for buttons.
                /// </summary>
                ImVec2 settingsPos  = ImVec2(sceneWindowPos.x + 5, sceneWindowPos.y + 5);
                ImVec2 settingsSize = ImVec2(65, 40);
                ImGui::SetNextWindowPos(settingsPos);
                ImGui::SetNextWindowBgAlpha(0.4f);
                ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 4.0f);
                ImGui::BeginChild("##scenePanel_settings", settingsSize, false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
                const float cursorPos = ImGui::GetCursorPosY();

                const ImVec2 buttonSize = ImVec2(25, 25);
                float        cursorPosY = settingsSize.y / 2.0f - buttonSize.y / 2.0f;
                ImGui::SetCursorPosX(5.0f);
                ImGui::SetCursorPosY(cursorPosY);

                if (WidgetsUtility::CustomToggle("##scenepanel_camsettings", buttonSize, m_shouldShowCameraSettings, nullptr, ICON_FA_CAMERA, 3.0f, "Camera Settings"))
                    m_shouldShowCameraSettings = !m_shouldShowCameraSettings;

                ImGui::SameLine();
                ImGui::SetCursorPosY(cursorPosY);

                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.1f, 0.6f, 0.1f, 1.0f));
                if (WidgetsUtility::CustomToggle("##scenepanel_gizmos", buttonSize, m_shouldShowGizmos, nullptr, ICON_FA_BACON, 3.0f, "Gizmos"))
                    m_shouldShowGizmos = !m_shouldShowGizmos;
                ImGui::PopStyleColor();
                ImGui::EndChild();
                ImGui::PopStyleVar();

                /// <summary>
                /// Camera settings pop-up window.
                /// </summary>
                if (m_shouldShowCameraSettings)
                {
                    // Smoothly animate window size
                    if (m_cameraSettingsWindowYMultiplier < 1.0f)
                    {
                        m_cameraSettingsWindowYMultiplier = Math::Lerp(m_cameraSettingsWindowYMultiplier, 1.1f, Engine::Get()->GetRawDelta() * 6.0f);
                        m_cameraSettingsWindowYMultiplier = Math::Clamp(m_cameraSettingsWindowYMultiplier, 0.0f, 1.0f);
                    }

                    ImVec2 cameraSettingsPos  = ImVec2(settingsPos.x, settingsPos.y + settingsSize.y);
                    ImVec2 cameraSettingsSize = ImVec2(210, 60 * m_cameraSettingsWindowYMultiplier);
                    ImGui::SetNextWindowPos(cameraSettingsPos);
                    ImGui::SetNextWindowBgAlpha(0.5f);
                    ImGui::BeginChild("##scenePanel_cameraSettings", cameraSettingsSize, false, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoScrollbar);
                    float cursorPosLabels = 12;
                    WidgetsUtility::IncrementCursorPosY(6);
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 0));
                    ImGui::SetCursorPosX(cursorPosLabels);
                    WidgetsUtility::PropertyLabel("Camera Speed");
                    ImGui::SameLine();

                    float cursorPosValues = ImGui::CalcTextSize("Camera Speed").x + 24;
                    ImGui::SetCursorPosX(cursorPosValues);
                    ImGui::SetNextItemWidth(100);
                    ImGui::SliderFloat("##editcamspd", &m_editorCameraSpeed, 0.0f, 1.0f);
                    ImGui::SetCursorPosX(cursorPosLabels);
                    WidgetsUtility::PropertyLabel("Multiplier");
                    ImGui::SameLine();
                    ImGui::SetCursorPosX(cursorPosValues);
                    ImGui::SetNextItemWidth(100);
                    ImGui::DragFloat("##editcammultip", &m_editorCameraSpeedMultiplier, 1.0f, 0.0f, 20.0f);
                    ImGui::PopStyleVar();
                    EditorApplication::Get()->GetCameraSystem().SetCameraSpeedMultiplier(m_editorCameraSpeed * m_editorCameraSpeedMultiplier);
                    ImGui::EndChild();

                    if (ImGui::IsWindowHovered())
                    {
                        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                        {
                            if (!ImGui::IsMouseHoveringRect(cameraSettingsPos, ImVec2(cameraSettingsPos.x + cameraSettingsSize.x, cameraSettingsPos.y + cameraSettingsSize.y)))
                                m_shouldShowCameraSettings = false;
                        }
                    }
                }
                else
                {
                    if (m_cameraSettingsWindowYMultiplier != 0.0f)
                        m_cameraSettingsWindowYMultiplier = 0.0f;
                }

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

                if (!levelPanelFirstRun)
                {
                    levelPanelFirstRun = true;
                    Graphics::RenderEngineBackend::Get()->SetScreenDisplay(Vector2i(0, 0), Vector2i((unsigned int)(sceneWindowSize.x), (unsigned int)(sceneWindowSize.y)));
                }

                // Model drag & drop.
                // if (ImGui::BeginDragDropTarget())
                // {
                //     if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(RESOURCES_MOVEMESH_ID))
                //     {
                //         IM_ASSERT(payload->DataSize == sizeof(uint32));
                //
                //         auto* ecs    = ECS::Registry::Get();
                //         auto& model  = Graphics::Model::GetModel(*(uint32*)payload->Data);
                //         auto  entity = ecs->CreateEntity(Utility::GetFileNameOnly(model.GetPath()));
                //         auto& mr     = ecs->emplace<ECS::ModelRendererComponent>(entity);
                //         mr.SetModel(entity, model);
                //
                //         auto& mat = Graphics::Material::GetMaterial("Resources/Engine/Materials/DefaultLit.linamat");
                //
                //         for (int i = 0; i < model.GetImportedMaterials().size(); i++)
                //             mr.SetMaterial(entity, i, mat);
                //     }
                //     ImGui::EndDragDropTarget();
                // }

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

            float matrixTranslation[3], matrixRotation[3], matrixScale[3];
            ImGuizmo::Manipulate(&view[0][0], &projection[0][0], currentTransformGizmoOP, currentTransformGizmoMode, &object[0][0]);
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
            if (m_selectedEntity != editorCam)
            {
                Vector3 boundingBoxPosition = Vector3::Zero;
                Vector3 boundingBoxHalfSize = Vector3::Zero;
                if (renderEngine->GetFrustumSystem()->GetEntityBounds(m_selectedEntity, boundingBoxPosition, boundingBoxHalfSize))
                    Event::EventSystem::Get()->Trigger<Event::EDrawBox>(Event::EDrawBox{boundingBoxPosition, boundingBoxHalfSize, Color::Red, 2.0f});
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
        if (ev.m_currentGizmo == 0)
            currentTransformGizmoOP = ImGuizmo::TRANSLATE;
        else if (ev.m_currentGizmo == 1)
            currentTransformGizmoOP = ImGuizmo::ROTATE;
        else if (ev.m_currentGizmo == 2)
            currentTransformGizmoOP = ImGuizmo::SCALE;
    }

    void LevelPanel::OnTransformPivotChanged(const ETransformPivotChanged& ev)
    {
        if (ev.m_isGlobal)
            currentTransformGizmoMode = ImGuizmo::MODE::WORLD;
        else
            currentTransformGizmoMode = ImGuizmo::MODE::LOCAL;
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
            Event::EventSystem::Get()->Trigger<ETransformPivotChanged>(ETransformPivotChanged{currentTransformGizmoMode == ImGuizmo::LOCAL});
    }

} // namespace Lina::Editor
