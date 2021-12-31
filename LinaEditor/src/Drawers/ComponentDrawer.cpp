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

    std::map<TypeID, bool>         m_componentFoldoutState;
    std::pair<ECS::Entity, TypeID> m_copyBuffer;

    template <typename Type> void Drawer_SetEnabled(ECS::Entity ent, bool enabled)
    {
        ECS::Registry::Get()->template get<Type>(ent).SetIsEnabled(enabled);

        
    }

    template <typename Type> Type& Drawer_Get(ECS::Entity entity)
    {
        return ECS::Registry::Get()->template get<Type>(entity);
    }

    template <typename Type> void Drawer_Add(ECS::Entity entity)
    {
        ECS::Registry::Get()->template emplace<Type>(entity);
    }

    template <typename Type> bool Drawer_Has(ECS::Entity entity)
    {
        return ECS::Registry::Get()->all_of<Type>(entity);
    }

    template <typename Type> void Drawer_Reset(ECS::Entity entity)
    {
        TypeID tid = GetTypeID<Type>();

        if (tid == GetTypeID<EntityDataComponent>())
        {
            EntityDataComponent& comp = ECS::Registry::Get()->get<EntityDataComponent>(entity);
            comp.SetLocalLocation(Vector3::Zero);
            comp.SetLocalRotation(Quaternion());
            comp.SetLocalScale(Vector3::One);
        }
        else
            ECS::Registry::Get()->template replace<Type>(entity, Type());
    }

    template <typename Type> void Drawer_Remove(ECS::Entity entity)
    {
        ECS::Registry::Get()->template remove<Type>(entity);
    }

    void Drawer_Copy(ECS::Entity entity, TypeID tid)
    {
        m_copyBuffer.first  = entity;
        m_copyBuffer.second = tid;
    }

    template <typename Type> void Drawer_Paste(ECS::Entity entity)
    {
        TypeID pastedTid = GetTypeID<Type>();

        if (pastedTid == m_copyBuffer.second)
        {
            if (m_copyBuffer.first != entt::null)
            {
                if (pastedTid == GetTypeID<EntityDataComponent>())
                {
                    EntityDataComponent& copy = ECS::Registry::Get()->get<EntityDataComponent>(m_copyBuffer.first);

                    auto& data = ECS::Registry::Get()->get<EntityDataComponent>(entity);
                    data.SetLocation(copy.GetLocation());
                    data.SetRotation(copy.GetRotation());
                    data.SetScale(copy.GetScale());
                }
                else
                {
                    Type* copy = ECS::Registry::Get()->try_get<Type>(m_copyBuffer.first);

                    if (copy != nullptr)
                    {
                        ECS::Registry::Get()->replace<Type>(entity, *copy);
                        Drawer_SetEnabled<Type>(entity, copy->GetIsEnabled());
                    }
                }
            }
        }

        // If we pasted a model renderer, it's data will be copied but we still need to
        // manually set the pasted model & materials on the renderer.
      // if (pastedTid == GetTypeID<ModelRendererComponent>())
      // {
      //     auto&                    mr        = ECS::Registry::Get()->get<ModelRendererComponent>(entity);
      //     std::string              modelPath = mr.GetModelPath();
      //     std::vector<std::string> materials = mr.GetMaterialPaths();
      //     if (Graphics::Model::ModelExists(modelPath))
      //     {
      //         mr.SetModel(entity, Graphics::Model::GetModel(modelPath));
      //
      //         for (int i = 0; i < materials.size(); i++)
      //         {
      //             if (Graphics::Material::MaterialExists(materials[i]))
      //                 mr.SetMaterial(entity, i, Graphics::Material::GetMaterial(materials[i]));
      //             else
      //                 mr.SetMaterial(entity, i, Graphics::Material::GetMaterial("Resources/Engine/Materials/DefaultLit.mat"));
      //         }
      //     }
      //     else
      //         mr.RemoveModel(entity);
      // }
    }

    template <typename Type> void Drawer_SetModel(ECS::Entity ent, Graphics::Model* model)
    {
        ECS::Registry::Get()->get<Type>(ent).SetModel(ent, *model);
    }

    template <typename Type> void Drawer_RemoveModel(ECS::Entity ent)
    {
        Type& comp = ECS::Registry::Get()->template get<Type>(ent);
        comp.RemoveModel(ent);
    }

    template <typename Type> void Drawer_SetMaterial(ECS::Entity ent, int materialIndex, const Graphics::Material& material)
    {
        Type& comp = ECS::Registry::Get()->template get<Type>(ent);
        comp.SetMaterial(ent, materialIndex, material);
    }

    template <typename Type> void Drawer_RemoveMaterial(ECS::Entity ent, int materialIndex)
    {
        Type& comp = ECS::Registry::Get()->template get<Type>(ent);
        comp.RemoveMaterial(ent, materialIndex);
    }

    template <typename Type> std::string Drawer_GetMaterialName(ECS::Entity ent, int index)
    {
        std::string modelPath = ECS::Registry::Get()->template get<Type>(ent).GetModelPath();
        return Graphics::Model::GetModel(modelPath).GetImportedMaterials()[index].m_name;
        ;
    }

    template <typename Type> void Drawer_ValueChanged(ECS::Entity ent, const char* label)
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

    template <typename Type> void Drawer_Debug(ECS::Entity ent)
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

    void Drawer_DebugPLight(ECS::Entity ent)
    {
        LINA_TRACE("Debug PLIGHT");
    }

    ComponentDrawer::ComponentDrawer()
    {
        uint8 defaultDrawFlags  = ComponentDrawFlags_None;
        uint8 meshRendererFlags = ComponentDrawFlags_NoCopy | ComponentDrawFlags_NoPaste | ComponentDrawFlags_NoRemove | ComponentDrawFlags_NoReset | ComponentDrawFlags_DisabledHeader;

        // Camera component
        entt::meta<CameraComponent>().data<&CameraComponent::m_isEnabled>("enabled"_hs);
        entt::meta<CameraComponent>().data<&CameraComponent::m_zFar>("zf"_hs).props(PROPS("Far", ComponentVariableType::DragFloat, "Far Plane"));
        entt::meta<CameraComponent>().data<&CameraComponent::m_zNear>("zn"_hs).props(PROPS("Near", ComponentVariableType::DragFloat, "Near Plane"));
        entt::meta<CameraComponent>().data<&CameraComponent::m_fieldOfView>("fov"_hs).props(PROPS("Fov", ComponentVariableType::DragFloat, "Field of View Angles"));
        entt::meta<CameraComponent>().data<&CameraComponent::m_clearColor>("cc"_hs).props(PROPS("Clear Color", ComponentVariableType::Color, "Background Clear Color"));
        RegisterComponentForEditor<CameraComponent>("Camera", ICON_FA_CAMERA, defaultDrawFlags, "View");

        entt::meta<MeshRendererComponent>().data<&MeshRendererComponent::m_isEnabled>("enabled"_hs);
        RegisterComponentForEditor<MeshRendererComponent>("Mesh", ICON_FA_BORDER_ALL, meshRendererFlags, "Rendering", false);

        // Dirlight
        entt::meta<DirectionalLightComponent>().data<&DirectionalLightComponent::m_isEnabled>("enabled"_hs);
        entt::meta<DirectionalLightComponent>().data<&DirectionalLightComponent::m_drawDebug>("debug"_hs).props(PROPS("Enable Debug", ComponentVariableType::Checkmark, "Enables debug drawing of light's reach."));
        entt::meta<DirectionalLightComponent>().data<&DirectionalLightComponent::m_shadowZFar>("szf"_hs).props(PROPS_DEP("Shadow Far", ComponentVariableType::DragFloat, "Far plane distance used in shadow rendering.", "castShadows"_hs));
        entt::meta<DirectionalLightComponent>().data<&DirectionalLightComponent::m_shadowZNear>("szn"_hs).props(PROPS_DEP("Shadow Near", ComponentVariableType::DragFloat, "Near plane distance used in shadow rendering.", "castShadows"_hs));
        entt::meta<DirectionalLightComponent>().data<&DirectionalLightComponent::m_shadowOrthoProjection>("so"_hs).props(PROPS_DEP("Shadow Projection", ComponentVariableType::Vector4, "Shadow projection matrix (ortho).", "castShadows"_hs));
        // entt::meta<DirectionalLightComponent>().data<&DirectionalLightComponent::m_castsShadows>("castShadows"_hs).props(PROPS("Cast Shadows", ComponentVariableType::Checkmark));
        entt::meta<DirectionalLightComponent>().data<&DirectionalLightComponent::m_intensity>("i"_hs).props(PROPS("Intensity", ComponentVariableType::DragFloat, ""));
        entt::meta<DirectionalLightComponent>().data<&DirectionalLightComponent::m_color>("cc"_hs).props(PROPS("Color", ComponentVariableType::Color, ""));
        RegisterComponentForEditor<DirectionalLightComponent>("Directional Light", ICON_FA_SUN, defaultDrawFlags, "Lights", true, false, true);

        // Spotlight
        entt::meta<SpotLightComponent>().data<&SpotLightComponent::m_isEnabled>("enabled"_hs);
        entt::meta<SpotLightComponent>().data<&SpotLightComponent::m_drawDebug>("debug"_hs).props(PROPS("Enable Debug", ComponentVariableType::Checkmark, "Enables debug drawing of light's reach."));
        // entt::meta<SpotLightComponent>().data<&SpotLightComponent::m_castsShadows>("castShadows"_hs).props(PROPS("Cast Shadows", ComponentVariableType::Checkmark));
        entt::meta<SpotLightComponent>().data<&SpotLightComponent::m_outerCutoff>("oc"_hs).props(PROPS("Outer Cutoff", ComponentVariableType::DragFloat, "The light will gradually dim from the edges of the cone defined by the Cutoff, to the cone defined by the Outer Cutoff."));
        entt::meta<SpotLightComponent>().data<&SpotLightComponent::m_cutoff>("cof"_hs).props(PROPS("Cutoff", ComponentVariableType::DragFloat, "The light will gradually dim from the edges of the cone defined by the Cutoff, to the cone defined by the Outer Cutoff."));
        entt::meta<SpotLightComponent>().data<&SpotLightComponent::m_distance>("dist"_hs).props(PROPS("Distance", ComponentVariableType::DragFloat, ""));
        entt::meta<SpotLightComponent>().data<&SpotLightComponent::m_intensity>("int"_hs).props(PROPS("Intensity", ComponentVariableType::DragFloat, ""));
        entt::meta<SpotLightComponent>().data<&SpotLightComponent::m_color>("c"_hs).props(PROPS("Color", ComponentVariableType::Color, ""));
        RegisterComponentForEditor<SpotLightComponent>("Spot Light", ICON_CS_SPOTLIGHT, defaultDrawFlags, "Lights", true, false, true);

        // Pointlight
        entt::meta<PointLightComponent>().data<&PointLightComponent::m_isEnabled>("enabled"_hs);
        entt::meta<PointLightComponent>().data<&PointLightComponent::m_drawDebug>("debug"_hs).props(PROPS("Enable Debug", ComponentVariableType::Checkmark, "Enables debug drawing of light's reach."));
        entt::meta<PointLightComponent>().data<&PointLightComponent::m_shadowFar>("sf"_hs).props(PROPS_DEP("Shadow Far", ComponentVariableType::DragFloat, "Far plane distance used in shadow rendering.", "castShadows"_hs));
        entt::meta<PointLightComponent>().data<&PointLightComponent::m_shadowNear>("sn"_hs).props(PROPS_DEP("Shadow Near", ComponentVariableType::DragFloat, "Near plane distance used in shadow rendering.", "castShadows"_hs));
        entt::meta<PointLightComponent>().data<&PointLightComponent::m_bias>("b"_hs).props(PROPS_DEP("Bias", ComponentVariableType::DragFloat, "Controls the amount of darkening.", "castShadows"_hs));
        entt::meta<PointLightComponent>().data<&PointLightComponent::m_castsShadows>("castShadows"_hs).props(PROPS("Cast Shadows", ComponentVariableType::Checkmark, ""));
        entt::meta<PointLightComponent>().data<&PointLightComponent::m_distance>("ds"_hs).props(PROPS("Distance", ComponentVariableType::DragFloat, ""));
        entt::meta<PointLightComponent>().data<&PointLightComponent::m_intensity>("i"_hs).props(PROPS("Intensity", ComponentVariableType::DragFloat, ""));
        entt::meta<PointLightComponent>().data<&PointLightComponent::m_color>("c"_hs).props(PROPS("Color", ComponentVariableType::Color, ""));
        RegisterComponentForEditor<PointLightComponent>("Point Light", ICON_FA_LIGHTBULB, defaultDrawFlags, "Lights", true, false, true);

        // Freelook
        entt::meta<FreeLookComponent>().data<&FreeLookComponent::m_isEnabled>("enabled"_hs);
        entt::meta<FreeLookComponent>().data<&FreeLookComponent::m_rotationSpeeds>("rs"_hs).props(PROPS("Rotation Speed", ComponentVariableType::Vector2, ""));
        entt::meta<FreeLookComponent>().data<&FreeLookComponent::m_movementSpeeds>("ms"_hs).props(PROPS("Movement Speed", ComponentVariableType::Vector2, ""));
        RegisterComponentForEditor<FreeLookComponent>("Freelook", ICON_FA_EYE, defaultDrawFlags, "View");

        // Model Renderer
        // auto idt = "matpath_arr"_hs;
        // std::string modelRendererTooltip = "Set true if you want to generate pivot entities at the center of each mesh.\n" + std::string("The center is calculated using the average of vertex positions, on all sub-meshes per mesh.");
        // entt::meta<ModelRendererComponent>().data<&ModelRendererComponent::m_isEnabled>("enabled"_hs);
        // entt::meta<ModelRendererComponent>().data<&ModelRendererComponent::m_materialPaths>(idt).props(PROPS("Materials", ComponentVariableType::MaterialArray, ""));
        // entt::meta<ModelRendererComponent>().data<&ModelRendererComponent::m_modelPath>("modpath"_hs).props(PROPS("Model", ComponentVariableType::ModelPath, ""));
        // entt::meta<ModelRendererComponent>().data<&ModelRendererComponent::m_generateMeshPivots>("pvts"_hs).props(PROPS("Generate Pivots", ComponentVariableType::Checkmark, modelRendererTooltip.c_str()));
        // entt::meta<ModelRendererComponent>().func<&Drawer_SetModel<ModelRendererComponent>, entt::as_ref_t>("setModel"_hs);
        // entt::meta<ModelRendererComponent>().func<&Drawer_GetMaterialName<ModelRendererComponent>, entt::as_ref_t>("getMaterialName"_hs);
        // entt::meta<ModelRendererComponent>().func<&Drawer_RemoveModel<ModelRendererComponent>, entt::as_ref_t>("removeModel"_hs);
        // entt::meta<ModelRendererComponent>().func<&Drawer_SetMaterial<ModelRendererComponent>, entt::as_ref_t>("setMaterial"_hs);
        // entt::meta<ModelRendererComponent>().func<&Drawer_RemoveMaterial<ModelRendererComponent>, entt::as_ref_t>("removeMaterial"_hs);
        // RegisterComponentForEditor<ModelRendererComponent>("Model Renderer", ICON_FA_CUBE, defaultDrawFlags, "Rendering", true, true);

        // Model Node
        std::string modelNodeTooltip = "Model node refers to a node in an imported model hierarchy. This is the main component used to draw a model's meshes.";
        entt::meta<ModelNodeComponent>().data<&ModelNodeComponent::m_isEnabled>("enabled"_hs);
        entt::meta<ModelNodeComponent>().data<&ModelNodeComponent::m_materialIDs>("mats"_hs).props(PROPS("Materials", ComponentVariableType::MaterialArray, ""));

        // Sprite renderer
        entt::meta<SpriteRendererComponent>().data<&SpriteRendererComponent::m_isEnabled>("enabled"_hs);
        entt::meta<SpriteRendererComponent>().data<&SpriteRendererComponent::m_materialPaths>("matpath"_hs).props(PROPS("Material", ComponentVariableType::MaterialPath, ""));
        RegisterComponentForEditor<SpriteRendererComponent>("Sprite", ICON_MD_TOYS, defaultDrawFlags, "Rendering");
    }

    void ComponentDrawer::Initialize()
    {
        Event::EventSystem::Get()->Connect<EComponentOrderSwapped, &ComponentDrawer::OnComponentOrderSwapped>(this);
        Event::EventSystem::Get()->Connect<ETransformPivotChanged, &ComponentDrawer::OnTransformPivotChanged>(this);
    }

    void ComponentDrawer::AddIDToDrawList(TypeID id)
    {
        // Add only if it doesn't exists.
        if (std::find(m_componentDrawList.begin(), m_componentDrawList.end(), id) == m_componentDrawList.end())
            m_componentDrawList.push_back(id);
    }

    void ComponentDrawer::SwapComponentOrder(TypeID id1, TypeID id2)
    {
        // Swap iterators.
        std::vector<TypeID>::iterator it1 = std::find(m_componentDrawList.begin(), m_componentDrawList.end(), id1);
        std::vector<TypeID>::iterator it2 = std::find(m_componentDrawList.begin(), m_componentDrawList.end(), id2);
        std::iter_swap(it1, it2);
    }

    void ComponentDrawer::OnComponentOrderSwapped(const EComponentOrderSwapped& ev)
    {
        SwapComponentOrder(ev.m_id1, ev.m_id2);
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
                auto        meta         = entt::resolve(tid);
                bool        hasComponent = meta.func("has"_hs).invoke({}, entity).cast<bool>();

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

    void ComponentDrawer::DrawEntityData(ECS::Entity entity, bool* transformFoldoutOpen, bool* physicsFoldoutOpen)
    {

        auto*                   ecs           = ECS::Registry::Get();
        auto*                   physicsEngine = Physics::PhysicsEngineBackend::Get();
        EntityDataComponent&    data          = ecs->get<EntityDataComponent>(entity);
        TypeID             entityDataTid = GetTypeID<EntityDataComponent>();
        TypeID             physicsTid    = GetTypeID<PhysicsComponent>();
        PhysicsComponent&       phy           = ecs->get<PhysicsComponent>(entity);
        Physics::SimulationType simType       = phy.m_simType;

        bool        disabled         = entity == ecs->GetEntity("Editor Camera");
        bool        disableTransform = (simType == Physics::SimulationType::Dynamic && !phy.m_isKinematic) || (simType == Physics::SimulationType::Static);
        std::string caretLabel       = "Transformation (Local)";

        if (simType == Physics::SimulationType::Dynamic)
            caretLabel += " (Physics: Dynamic)";
        else if (simType == Physics::SimulationType::Static)
            caretLabel += " (Physics: Static)";

        bool copied   = false;
        bool pasted   = false;
        bool resetted = false;

        if (disabled)
            ImGui::BeginDisabled();

        if (WidgetsUtility::ComponentHeader(entityDataTid, transformFoldoutOpen, caretLabel.c_str(), nullptr, nullptr, nullptr, disabled ? nullptr : &copied, disabled ? nullptr : &pasted, disabled ? nullptr : &resetted, false))
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

        if (copied)
            Drawer_Copy(entity, entityDataTid);

        if (pasted)
            Drawer_Paste<EntityDataComponent>(entity);

        if (resetted)
            Drawer_Reset<EntityDataComponent>(entity);

        copied = pasted = resetted = false;
        if (WidgetsUtility::ComponentHeader(entityDataTid, physicsFoldoutOpen, "Physics", nullptr, nullptr, nullptr, disabled ? nullptr : &copied, disabled ? nullptr : &pasted, disabled ? nullptr : &resetted, false))
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
            const std::string         currentMaterial = phy.m_material.m_value->GetPath();
            bool                      removed         = false;
            Physics::PhysicsMaterial* selected        = WidgetsUtility::PhysicsMaterialComboBox("##phyMat", currentMaterial, &removed);

            if (removed)
            {
                auto* mat = Resources::ResourceStorage::Get()->GetResource<Physics::PhysicsMaterial>("Resources/Engine/Physics/Materials/DefaultPhysicsMaterial.linaphymat");
                selected  = mat;
                physicsEngine->SetBodyMaterial(entity, mat);
            }

            if (selected != nullptr && phy.m_material.m_value->GetSID() != selected->GetSID())
            {
                physicsEngine->SetBodyMaterial(entity, selected);
                phy.m_material.m_sid = selected->GetSID();
                phy.m_material.m_value = selected;
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

#ifdef LINA_PHYSICS_BULLET
            // Draw other shape types.
#endif
            ImGui::SetCursorPosX(CURSOR_X_LABELS);
        }

        if (copied)
            Drawer_Copy(entity, entityDataTid);

        if (pasted)
            Drawer_Paste<PhysicsComponent>(entity);

        if (resetted)
        {
            phy.Reset();
            physicsEngine->SetBodySimulation(entity, phy.m_simType);
        }

        if (disabled)
            ImGui::EndDisabled();
    }

    void ComponentDrawer::DrawComponent(TypeID tid, ECS::Entity ent)
    {
        auto* ecs          = ECS::Registry::Get();
        auto  resolvedData = entt::resolve(tid);

        if (resolvedData)
        {
            auto            title     = resolvedData.prop("Title"_hs).value().cast<char*>();
            uint8           drawFlags = resolvedData.prop("DrawFlags"_hs).value().cast<uint8>();
            auto            icon      = resolvedData.prop("Icon"_hs).value().cast<char*>();
            auto            foldout   = resolvedData.prop("Foldout"_hs).value().cast<bool>();
            entt::meta_any& instance  = resolvedData.func("get"_hs).invoke({}, ent);

            bool drawToggle      = !(drawFlags & ComponentDrawFlags_NoToggle);
            bool drawRemove      = !(drawFlags & ComponentDrawFlags_NoRemove);
            bool drawCopy        = !(drawFlags & ComponentDrawFlags_NoCopy);
            bool drawPaste       = !(drawFlags & ComponentDrawFlags_NoPaste);
            bool drawReset       = !(drawFlags & ComponentDrawFlags_NoReset);
            bool disableHeader   = (drawFlags & ComponentDrawFlags_DisabledHeader);
            bool remove          = false;
            bool copy            = false;
            bool paste           = false;
            bool reset           = false;
            bool enabled         = resolvedData.data("enabled"_hs).get(instance).cast<bool>();
            bool enabledPrevious = enabled;
            WidgetsUtility::ComponentHeader(tid, &m_componentFoldoutState[tid], title, icon, drawToggle ? &enabled : nullptr, drawRemove ? &remove : nullptr, drawCopy ? &copy : nullptr, drawPaste ? &paste : nullptr, drawReset ? &reset : nullptr, true, disableHeader);

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

            if (m_componentFoldoutState[tid])
            {
                // Draw each reflected property in the component according to it's type.
                int         varCounter = 0;
                std::string varLabelID = "";
                for (auto data : resolvedData.data())
                {

                    auto labelProperty = data.prop("Label"_hs);
                    auto typeProperty  = data.prop("Type"_hs);
                    if (!labelProperty || !typeProperty)
                        continue;

                    const char*           label                     = labelProperty.value().cast<const char*>();
                    ComponentVariableType type                      = typeProperty.value().cast<ComponentVariableType>();
                    auto                  displayDependencyProperty = data.prop("DisplayDependency"_hs);
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

                    auto tooltipPropery = data.prop("Tooltip"_hs);
                    if (!tooltipPropery)
                    {
                        std::string tooltip = tooltipPropery.value().cast<std::string>();
                        if (ImGui::IsItemHovered() && tooltip.compare("") != 0)
                            WidgetsUtility::Tooltip(tooltip.c_str());
                    }

                    if (type == ComponentVariableType::DragFloat)
                    {
                        float       variable = data.get(instance).cast<float>();
                        const float prev     = variable;
                        WidgetsUtility::DragFloat(varLabelID.c_str(), nullptr, &variable);
                        data.set(instance, variable);

                        if (valueChangedCallback && prev != variable)
                            valueChangedCallback.invoke({}, ent, label);
                    }
                    else if (type == ComponentVariableType::DragInt)
                    {
                        int       variable = data.get(instance).cast<int>();
                        const int prev     = variable;
                        WidgetsUtility::DragInt(varLabelID.c_str(), nullptr, &variable);
                        data.set(instance, variable);

                        if (valueChangedCallback && prev != variable)
                            valueChangedCallback.invoke({}, ent, label);
                    }
                    else if (type == ComponentVariableType::Vector2)
                    {
                        Vector2       variable = data.get(instance).cast<Vector2>();
                        const Vector2 prev     = variable;
                        WidgetsUtility::DragVector2(varLabelID.c_str(), &variable.x);
                        data.set(instance, variable);

                        if (valueChangedCallback && prev != variable)
                            valueChangedCallback.invoke({}, ent, label);
                    }
                    else if (type == ComponentVariableType::Vector3)
                    {
                        Vector3       variable = data.get(instance).cast<Vector3>();
                        const Vector3 prev     = variable;
                        WidgetsUtility::DragVector3(varLabelID.c_str(), &variable.x);
                        data.set(instance, variable);

                        if (valueChangedCallback && prev != variable)
                            valueChangedCallback.invoke({}, ent, label);
                    }
                    else if (type == ComponentVariableType::Vector4)
                    {
                        Vector4       variable = data.get(instance).cast<Vector4>();
                        const Vector4 prev     = variable;
                        WidgetsUtility::DragVector4(varLabelID.c_str(), &variable.x);
                        data.set(instance, variable);

                        if (valueChangedCallback && prev != variable)
                            valueChangedCallback.invoke({}, ent, label);
                    }
                    else if (type == ComponentVariableType::Color)
                    {
                        Color       variable = data.get(instance).cast<Color>();
                        const Color prev     = variable;
                        WidgetsUtility::ColorButton(varLabelID.c_str(), &variable.r);
                        data.set(instance, variable);

                        if (valueChangedCallback && prev != variable)
                            valueChangedCallback.invoke({}, ent, label);
                    }
                    else if (type == ComponentVariableType::Checkmark)
                    {
                        bool       variable = data.get(instance).cast<bool>();
                        const bool prev     = variable;
                        ImGui::Checkbox(varLabelID.c_str(), &variable);
                        data.set(instance, variable);

                        if (valueChangedCallback && prev != variable)
                            valueChangedCallback.invoke({}, ent, label);
                    }
                    else if (type == ComponentVariableType::ModelPath)
                    {
                        std::string       modelPath     = data.get(instance).cast<std::string>();
                        const std::string prev          = modelPath;
                        bool              removed       = false;
                        Graphics::Model*  selectedModel = WidgetsUtility::ModelComboBox(varLabelID.c_str(), StringID(modelPath.c_str()).value(), &removed);

                        // Select model.
                        if (selectedModel)
                            resolvedData.func("setModel"_hs).invoke({}, ent, selectedModel);

                        // Remove Model
                        if (removed || reset)
                            resolvedData.func("removeModel"_hs).invoke({}, ent);

                        // Mesh drag & drop.
                        if (ImGui::BeginDragDropTarget())
                        {
                            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(RESOURCES_MOVEMESH_ID))
                            {
                                IM_ASSERT(payload->DataSize == sizeof(StringIDType));

                                auto& model = Graphics::Model::GetModel(*(StringIDType*)payload->Data);
                                resolvedData.func("setModel"_hs).invoke({}, ent, model);
                            }
                            ImGui::EndDragDropTarget();
                        }

                        if (valueChangedCallback && prev != modelPath)
                            valueChangedCallback.invoke({}, ent, label);
                    }
                    else if (type == ComponentVariableType::MaterialArray)
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

                            const std::string   cboxID           = "##modRendMat " + std::to_string(i);
                            bool                removed          = false;
                            Graphics::Material* selectedMaterial = WidgetsUtility::MaterialComboBox(cboxID.c_str(), materials[i], &removed);

                            if (selectedMaterial != nullptr)
                                resolvedData.func("setMaterial"_hs).invoke({}, ent, i, *selectedMaterial);

                            if (removed)
                                resolvedData.func("removeMaterial"_hs).invoke({}, ent, i);

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
