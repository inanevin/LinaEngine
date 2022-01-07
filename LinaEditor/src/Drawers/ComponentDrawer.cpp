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

#include "Drawers/ComponentDrawer.hpp"

#include "Core/Application.hpp"
#include "Core/CustomFontIcons.hpp"
#include "Core/EditorCommon.hpp"
#include "ECS/Components/CameraComponent.hpp"
#include "ECS/Components/EntityDataComponent.hpp"
#include "ECS/Components/FreeLookComponent.hpp"
#include "ECS/Components/LightComponent.hpp"
#include "ECS/Components/MeshRendererComponent.hpp"
#include "ECS/Components/ModelNodeComponent.hpp"
#include "ECS/Components/ModelRendererComponent.hpp"
#include "ECS/Components/SpriteRendererComponent.hpp"
#include "Resources/ResourceStorage.hpp"
#include "IconsFontAwesome5.h"
#include "IconsMaterialDesign.h"
#include "Memory/Memory.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/Model.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "imgui/imguizmo/ImGuizmo.h"
#include "imgui/imgui.h"

using namespace ECS;
using namespace Editor;

namespace Lina::Editor
{
    using namespace entt::literals;

    template <typename Type>
    void Drawer_ValueChanged(ECS::Entity ent, const char* label)
    {
        TypeID tid = GetTypeID<Type>();

        if (tid == GetTypeID<ModelRendererComponent>())
        {
            // Generate pivots changed.
            if (std::string(label).compare("Generate Pivots") == 0)
            {
                ModelRendererComponent& mr = ECS::Registry::Get()->get<ModelRendererComponent>(ent);
                mr.RefreshHierarchy(ent);
            }
        }
    }

    template <typename Type>
    void Drawer_Debug(ECS::Entity ent)
    {
        auto tid = GetTypeID<Type>();

        if (tid == GetTypeID<PointLightComponent>())
        {
            ECS::EntityDataComponent& data   = ECS::Registry::Get()->get<ECS::EntityDataComponent>(ent);
            ECS::PointLightComponent& pLight = ECS::Registry::Get()->get<ECS::PointLightComponent>(ent);
            Vector3                   end1   = data.GetLocation() + (pLight.m_distance * data.GetRotation().GetRight());
            Vector3                   end2   = data.GetLocation() + (-pLight.m_distance * data.GetRotation().GetRight());
            Vector3                   end3   = data.GetLocation() + (pLight.m_distance * data.GetRotation().GetForward());
            Vector3                   end4   = data.GetLocation() + (-pLight.m_distance * data.GetRotation().GetForward());
            Graphics::RenderEngineBackend::Get()->DrawLine(data.GetLocation(), end1, Color::Red, 1.4f);
            Graphics::RenderEngineBackend::Get()->DrawLine(data.GetLocation(), end2, Color::Red, 1.4f);
            Graphics::RenderEngineBackend::Get()->DrawLine(data.GetLocation(), end3, Color::Red, 1.4f);
            Graphics::RenderEngineBackend::Get()->DrawLine(data.GetLocation(), end4, Color::Red, 1.4f);
        }
        else if (tid == GetTypeID<SpotLightComponent>())
        {
            ECS::EntityDataComponent& data   = ECS::Registry::Get()->get<ECS::EntityDataComponent>(ent);
            ECS::SpotLightComponent&  sLight = ECS::Registry::Get()->get<ECS::SpotLightComponent>(ent);
            Vector3                   end1   = data.GetLocation() + (sLight.m_distance * data.GetRotation().GetForward());
            Graphics::RenderEngineBackend::Get()->DrawLine(data.GetLocation(), end1, Color::Red, 1.4f);
        }
        else if (tid == GetTypeID<DirectionalLightComponent>())
        {
            ECS::EntityDataComponent& data = ECS::Registry::Get()->get<ECS::EntityDataComponent>(ent);
            Vector3                   dir  = Vector3::Zero - data.GetLocation();
            Vector3                   end1 = data.GetLocation() + dir;
            Graphics::RenderEngineBackend::Get()->DrawLine(data.GetLocation(), end1, Color::Red, 1.4f);
        }
    }

    void ComponentDrawer::Initialize()
    {
        Event::EventSystem::Get()->Connect<EComponentOrderSwapped, &ComponentDrawer::OnComponentOrderSwapped>(this);
        Event::EventSystem::Get()->Connect<ETransformPivotChanged, &ComponentDrawer::OnTransformPivotChanged>(this);
    }

    void ComponentDrawer::OnComponentOrderSwapped(const EComponentOrderSwapped& ev)
    {
        std::vector<TypeID>::iterator it1 = std::find(m_componentDrawList.begin(), m_componentDrawList.end(), ev.m_id1);
        std::vector<TypeID>::iterator it2 = std::find(m_componentDrawList.begin(), m_componentDrawList.end(), ev.m_id2);
        std::iter_swap(it1, it2);
    }

    void ComponentDrawer::ClearDrawList()
    {
        m_componentDrawList.clear();
    }

    AddComponentMap ComponentDrawer::GetCurrentAddComponentMap(ECS::Entity entity)
    {
        AddComponentMap map;

        for (auto& category : m_addComponentMap)
        {
            for (auto& pair : category.second)
            {
                TypeID tid          = pair.second;
                auto   meta         = entt::resolve(tid);
                bool   hasComponent = meta.func("has"_hs).invoke({}, entity).cast<bool>();

                if (!hasComponent)
                {
                    map[category.first].push_back(std::make_pair(pair.first, tid));
                }
            }
        }

        return map;
    }

    void ComponentDrawer::PushComponentToDraw(TypeID tid, ECS::Entity ent)
    {
        if (std::find(m_componentDrawList.begin(), m_componentDrawList.end(), tid) == m_componentDrawList.end())
            m_componentDrawList.push_back(tid);
    }

    void ComponentDrawer::DrawAllComponents(ECS::Entity ent)
    {
        for (auto tid : m_componentDrawList)
            DrawComponent(tid, ent);
    }

    void ComponentDrawer::OnTransformPivotChanged(const ETransformPivotChanged& ev)
    {
        m_isTransformPivotGlobal = ev.m_isGlobal;
    }

    void ComponentDrawer::DrawEntityData(ECS::Entity entity)
    {
        auto&  data             = ECS::Registry::Get()->get<ECS::EntityDataComponent>(entity);
        auto&  phy              = ECS::Registry::Get()->get<ECS::PhysicsComponent>(entity);
        TypeID dataTid          = GetTypeID<ECS::EntityDataComponent>();
        TypeID physicsTid       = GetTypeID<ECS::PhysicsComponent>();
        bool   disableTransform = (phy.GetSimType() == Physics::SimulationType::Dynamic && !phy.GetIsKinematic()) || (phy.GetSimType() == Physics::SimulationType::Static);
        auto*  physicsEngine    = Physics::PhysicsEngineBackend::Get();

        bool copied          = false;
        bool pasted          = false;
        bool resetted        = false;
        bool dataFoldoutOpen = WidgetsUtility::ComponentHeader(dataTid, "Entity Data Component", ICON_FA_DATABASE, nullptr, nullptr, &copied, &pasted, &resetted);

        auto resolved = entt::resolve<ECS::EntityDataComponent>();

        if (copied)
            resolved.func("copy"_hs).invoke({}, entity, dataTid);

        if (pasted)
            resolved.func("paste"_hs).invoke({}, entity);

        if (resetted)
            resolved.func("reset"_hs).invoke({}, entity);

        if (dataFoldoutOpen)
        {
            WidgetsUtility::PropertyLabel("Location");

            if (ImGui::IsItemHovered())
            {
                const std::string tooltipData = "Global: " + data.GetLocation().ToString();
                WidgetsUtility::Tooltip(tooltipData.c_str());
            }

            if (disableTransform)
                ImGui::BeginDisabled();

            Vector3 location = data.GetLocalLocation();
            WidgetsUtility::DragVector3("##loc", &location.x);
            data.SetLocalLocation(location);

            WidgetsUtility::PropertyLabel("Rotation");
            if (ImGui::IsItemHovered())
            {
                const std::string tooltipData = "Global: " + data.GetRotation().ToString();
                WidgetsUtility::Tooltip(tooltipData.c_str());
            }

            Vector3 rot = data.GetLocalRotationAngles();
            WidgetsUtility::DragVector3("##rot", &rot.x);
            data.SetLocalRotationAngles(rot);

            WidgetsUtility::PropertyLabel("Scale");
            if (ImGui::IsItemHovered())
            {
                const std::string tooltipData = "Global: " + data.GetScale().ToString();
                WidgetsUtility::Tooltip(tooltipData.c_str());
            }

            Vector3 scale = data.GetLocalScale();
            WidgetsUtility::DragVector3("##scale", &scale.x);
            data.SetLocalScale(scale);

            if (disableTransform)
                ImGui::EndDisabled();
        }

        copied                  = false;
        pasted                  = false;
        resetted                = false;
        bool physicsFoldoutOpen = WidgetsUtility::ComponentHeader(physicsTid, "Physics Component", ICON_FA_APPLE_ALT, nullptr, nullptr, &copied, &pasted, &resetted);

        if (copied)
            resolved.func("copy"_hs).invoke({}, entity, physicsTid);

        if (pasted)
            resolved.func("paste"_hs).invoke({}, entity);

        if (resetted)
            resolved.func("reset"_hs).invoke({}, entity);

        if (physicsFoldoutOpen)
        {
            WidgetsUtility::PropertyLabel("Simulation");
            if (ImGui::IsItemHovered())
                WidgetsUtility::Tooltip("Simulation type determines how the object will behave in physics world. \nNone = Object is not physically simulated. \nDynamic = Object will be an active rigidbody. You can switch between kinematic & non-kinematic to determine \nwhether the transformation "
                                        "will be user-controlled or fully-simulated. \nStatic = Object will be simulated, but can not move during the gameplay. ");

            Physics::SimulationType currentSimType  = phy.m_simType;
            Physics::SimulationType selectedSimType = (Physics::SimulationType)WidgetsUtility::SimulationTypeComboBox("##simType", (int)phy.m_simType);
            if (selectedSimType != currentSimType)
            {
                physicsEngine->SetBodySimulation(entity, selectedSimType);
            }

            if (selectedSimType == Physics::SimulationType::Dynamic)
            {
                WidgetsUtility::PropertyLabel("Kinematic");
                if (ImGui::IsItemHovered())
                    WidgetsUtility::Tooltip("Kinematic bodies are physically simulated and affect other bodies around them, but are not affected by other forces or collision.");

                const bool currentKinematic = phy.m_isKinematic;
                ImGui::Checkbox("##kinematic", &phy.m_isKinematic);
                if (currentKinematic != phy.m_isKinematic)
                {
                    physicsEngine->SetBodyKinematic(entity, phy.m_isKinematic);
                }
            }

            WidgetsUtility::PropertyLabel("Mass");
            const float currentMass = phy.m_mass;
            WidgetsUtility::DragFloat("##mass", nullptr, &phy.m_mass);
            if (phy.m_mass != currentMass)
            {
                physicsEngine->SetBodyMass(entity, phy.m_mass);
            }

            WidgetsUtility::PropertyLabel("Physics Material");
            const std::string currentMaterial = phy.m_material.m_value->GetPath();
            StringIDType      selected        = WidgetsUtility::ResourceSelectionPhysicsMaterial(&phy.m_material);

            if (selected != 0 && phy.m_material.m_value->GetSID() != selected)
            {
                auto* mat = Resources::ResourceStorage::Get()->GetResource<Physics::PhysicsMaterial>(selected);
                physicsEngine->SetBodyMaterial(entity, mat);
                phy.m_material.m_sid   = mat->GetSID();
                phy.m_material.m_value = mat;
            }

            WidgetsUtility::PropertyLabel("Shape");
            const Physics::CollisionShape currentShape = phy.m_collisionShape;
            phy.m_collisionShape                       = (Physics::CollisionShape)WidgetsUtility::CollisionShapeComboBox("##collision", (int)phy.m_collisionShape);
            if (phy.m_collisionShape != currentShape)
            {
                physicsEngine->SetBodyCollisionShape(entity, phy.m_collisionShape);
                // f (phy.m_collisionShape == Physics::CollisionShape::ConvexMesh)
                //
                //	ECS::ModelRendererComponent* mr = ecs->try_get<ECS::ModelRendererComponent>(entity);
                //
                //	if (mr != nullptr)
                //	{
                //		phy.m_attachedModelID = mr->m_modelID;
                //		physicsEngine->SetBodyCollisionShape(entity, phy.m_collisionShape);
                //
                //	}
                //	else
                //	{
                //		phy.m_collisionShape = currentShape;
                //		LINA_ERR("This entity does not contain a mesh renderer.");
                //	}
                //
            }
            if (phy.m_collisionShape == Physics::CollisionShape::Box)
            {
                WidgetsUtility::PropertyLabel("Half Extents");
                const Vector3 currentExtents = phy.m_halfExtents;
                WidgetsUtility::DragVector3("##halfextents", &phy.m_halfExtents.x);
                if (currentExtents != phy.m_halfExtents)
                {
                    physicsEngine->SetBodyHalfExtents(entity, phy.m_halfExtents);
                }
            }
            else if (phy.m_collisionShape == Physics::CollisionShape::Sphere)
            {
                WidgetsUtility::PropertyLabel("Radius");
                const float currentRadius = phy.m_radius;
                WidgetsUtility::DragFloat("##radius", nullptr, &phy.m_radius);
                if (currentRadius != phy.m_radius)
                {
                    physicsEngine->SetBodyRadius(entity, phy.m_radius);
                }
            }
            else if (phy.m_collisionShape == Physics::CollisionShape::Capsule)
            {
                WidgetsUtility::PropertyLabel("Radius");
                const float currentRadius = phy.m_radius;
                WidgetsUtility::DragFloat("##radius", nullptr, &phy.m_radius);
                if (currentRadius != phy.m_radius)
                {
                    physicsEngine->SetBodyRadius(entity, phy.m_radius);
                }

                WidgetsUtility::PropertyLabel("Half Height");
                const float currentHeight = phy.m_capsuleHalfHeight;
                WidgetsUtility::DragFloat("##height", nullptr, &phy.m_capsuleHalfHeight);
                if (currentHeight != phy.m_capsuleHalfHeight)
                {
                    physicsEngine->SetBodyHeight(entity, phy.m_capsuleHalfHeight);
                }
            }
        }
    }

    void ComponentDrawer::DrawComponent(TypeID tid, ECS::Entity ent)
    {

        auto* ecs          = ECS::Registry::Get();
        auto  resolvedData = entt::resolve(tid);

        if (resolvedData)
        {
            auto            title    = resolvedData.prop("Title"_hs).value().cast<const char*>();
            auto            icon     = resolvedData.prop("Icon"_hs).value().cast<const char*>();
            entt::meta_any& instance = resolvedData.func("get"_hs).invoke({}, ent);

            bool remove          = false;
            bool copy            = false;
            bool paste           = false;
            bool reset           = false;
            bool enabled         = resolvedData.data("m_isEnabled"_hs).get(instance).cast<bool>();
            bool enabledPrevious = enabled;

            bool foldoutOpen = WidgetsUtility::ComponentHeader(tid, title, icon, &enabled, &remove, &copy, &paste, &reset);

            if (enabled != enabledPrevious)
                resolvedData.func("setEnabled"_hs).invoke({}, ent, enabled);

            if (remove)
            {
                resolvedData.func("remove"_hs).invoke({}, ent);

                for (std::vector<TypeID>::iterator it = m_componentDrawList.begin(); it != m_componentDrawList.end(); it++)
                {
                    if (*it == tid)
                    {
                        m_componentDrawList.erase(it);
                        break;
                    }
                }
                return;
            }

            if (reset)
                resolvedData.func("reset"_hs).invoke({}, ent);

            if (copy)
                resolvedData.func("copy"_hs).invoke({}, ent, tid);

            if (paste)
                resolvedData.func("paste"_hs).invoke({}, ent);

            auto valueChangedCallback = resolvedData.func("valueChanged"_hs);

            if (foldoutOpen)
            {
                // Draw each reflected property in the component according to it's type.
                int         varCounter = 0;
                std::string varLabelID = "";
                for (auto data : resolvedData.data())
                {

                    auto labelProperty = data.prop("Title"_hs);
                    auto typeProperty  = data.prop("Type"_hs);

                    if (!labelProperty || !typeProperty)
                        continue;

                    const char* label                     = labelProperty.value().cast<const char*>();
                    std::string type                      = std::string(typeProperty.value().cast<const char*>());
                    auto        displayDependencyProperty = data.prop("Depends"_hs);

                    if (displayDependencyProperty)
                    {
                        entt::hashed_string displayDependencyHash = displayDependencyProperty.value().cast<entt::hashed_string>();
                        auto                dependantMember       = resolvedData.data(displayDependencyHash);

                        if (dependantMember)
                        {
                            bool dependantMemberValue = dependantMember.get(instance).cast<bool>();
                            if (!dependantMemberValue)
                                continue;
                        }
                    }

                    varLabelID = "##_" + std::string(title) + std::to_string(varCounter);
                    WidgetsUtility::PropertyLabel(label);

                    auto tooltipProperty = data.prop("Tooltip"_hs);
                    if (tooltipProperty)
                    {
                        std::string tooltip = std::string(tooltipProperty.value().cast<const char*>());
                        if (ImGui::IsItemHovered() && tooltip.compare("") != 0)
                            WidgetsUtility::Tooltip(tooltip.c_str());
                    }

                    if (type.compare("Float") == 0)
                    {
                        float       variable = data.get(instance).cast<float>();
                        const float prev     = variable;
                        WidgetsUtility::DragFloat(varLabelID.c_str(), nullptr, &variable);
                        data.set(instance, variable);

                        if (valueChangedCallback && prev != variable)
                            valueChangedCallback.invoke({}, ent, label);
                    }
                    else if (type.compare("Int") == 0)
                    {
                        int       variable = data.get(instance).cast<int>();
                        const int prev     = variable;
                        WidgetsUtility::DragInt(varLabelID.c_str(), nullptr, &variable);
                        data.set(instance, variable);

                        if (valueChangedCallback && prev != variable)
                            valueChangedCallback.invoke({}, ent, label);
                    }
                    else if (type.compare("Vector2") == 0)
                    {
                        Vector2       variable = data.get(instance).cast<Vector2>();
                        const Vector2 prev     = variable;
                        WidgetsUtility::DragVector2(varLabelID.c_str(), &variable.x);
                        data.set(instance, variable);

                        if (valueChangedCallback && prev != variable)
                            valueChangedCallback.invoke({}, ent, label);
                    }
                    else if (type.compare("Vector3") == 0)
                    {
                        Vector3       variable = data.get(instance).cast<Vector3>();
                        const Vector3 prev     = variable;
                        WidgetsUtility::DragVector3(varLabelID.c_str(), &variable.x);
                        data.set(instance, variable);

                        if (valueChangedCallback && prev != variable)
                            valueChangedCallback.invoke({}, ent, label);
                    }
                    else if (type.compare("Vector4") == 0)
                    {
                        Vector4       variable = data.get(instance).cast<Vector4>();
                        const Vector4 prev     = variable;
                        WidgetsUtility::DragVector4(varLabelID.c_str(), &variable.x);
                        data.set(instance, variable);

                        if (valueChangedCallback && prev != variable)
                            valueChangedCallback.invoke({}, ent, label);
                    }
                    else if (type.compare("Color") == 0)
                    {
                        Color       variable = data.get(instance).cast<Color>();
                        const Color prev     = variable;
                        WidgetsUtility::ColorButton(varLabelID.c_str(), &variable.r);
                        data.set(instance, variable);

                        if (valueChangedCallback && prev != variable)
                            valueChangedCallback.invoke({}, ent, label);
                    }
                    else if (type.compare("Bool") == 0)
                    {
                        bool       variable = data.get(instance).cast<bool>();
                        const bool prev     = variable;
                        ImGui::Checkbox(varLabelID.c_str(), &variable);
                        data.set(instance, variable);

                        if (valueChangedCallback && prev != variable)
                            valueChangedCallback.invoke({}, ent, label);
                    }
                    else if (type.compare("MaterialArray") == 0)
                    {
                        ImGui::NewLine();

                        std::vector<std::string> materials = data.get(instance).cast<std::vector<std::string>>();

                        for (int i = 0; i < materials.size(); i++)
                        {
                            // Material selection.
                            char matPathC[128] = "";
                            strcpy_s(matPathC, materials[i].c_str());

                            // Draw material name
                            std::string materialName = resolvedData.func("getMaterialName"_hs).invoke({}, ent, i).cast<std::string>();
                            WidgetsUtility::PropertyLabel(materialName.c_str());

                            //   const std::string   cboxID           = "##modRendMat " + std::to_string(i);
                            //   bool                removed          = false;
                            //   Graphics::Material* selectedMaterial = WidgetsUtility::MaterialComboBox(cboxID.c_str(), materials[i], &removed);
                            //
                            //   if (selectedMaterial != nullptr)
                            //       resolvedData.func("setMaterial"_hs).invoke({}, ent, i, *selectedMaterial);
                            //
                            //   if (removed)
                            //       resolvedData.func("removeMaterial"_hs).invoke({}, ent, i);
                            //
                            // // Material drag & drop.
                            // if (ImGui::BeginDragDropTarget())
                            // {
                            //     if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(RESOURCES_MOVEMATERIAL_ID))
                            //     {
                            //         IM_ASSERT(payload->DataSize == sizeof(StringIDType));
                            //
                            //         auto& mat = Graphics::Material::GetMaterial(*(StringIDType*)payload->Data);
                            //         resolvedData.func("setMaterial"_hs).invoke({}, ent, i, mat);
                            //     }
                            //     ImGui::EndDragDropTarget();
                            // }
                        }
                    }

                    varCounter++;
                    varLabelID.clear();
                }

                // Check if the target data contains a debug property,
                // If so, and if the property is set to true,
                // Call the debug function if exists.
                auto debugProperty = resolvedData.data("debug"_hs);
                if (debugProperty)
                {
                    bool isDebugEnabled = debugProperty.get(instance).cast<bool>();

                    if (isDebugEnabled)
                    {
                        auto debugFunc = resolvedData.func("drawDebug"_hs);

                        if (debugFunc)
                            debugFunc.invoke({}, ent);
                    }
                }
            }
        }
    }

} // namespace Lina::Editor
