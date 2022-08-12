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
#include "ECS/Components/ReflectionAreaComponent.hpp"
#include "ECS/Components/SpriteRendererComponent.hpp"
#include "Resources/ResourceStorage.hpp"
#include "IconsFontAwesome5.h"
#include "IconsMaterialDesign.h"
#include "Memory/Memory.hpp"
#include "Rendering/Material.hpp"
#include "Drawers/ClassDrawer.hpp"
#include "Rendering/Model.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "EventSystem/GraphicsEvents.hpp"
#include "imgui/imguizmo/ImGuizmo.h"
#include "imgui/imgui.h"

using namespace ECS;

namespace Lina::Editor
{
    using namespace entt::literals;

    void Drawer_Debug(TypeID tid, ECS::Entity ent)
    {
        if (tid == GetTypeID<PointLightComponent>())
        {
            ECS::EntityDataComponent& data   = ECS::Registry::Get()->get<ECS::EntityDataComponent>(ent);
            ECS::PointLightComponent& pLight = ECS::Registry::Get()->get<ECS::PointLightComponent>(ent);
            Event::EventSystem::Get()->Trigger<Event::EDrawSphere>(Event::EDrawSphere{data.GetLocation(), pLight.m_distance, Color::Red, 1.4f});
        }
        else if (tid == GetTypeID<SpotLightComponent>())
        {
            ECS::EntityDataComponent& data   = ECS::Registry::Get()->get<ECS::EntityDataComponent>(ent);
            ECS::SpotLightComponent&  sLight = ECS::Registry::Get()->get<ECS::SpotLightComponent>(ent);
            Vector3                   end1   = data.GetLocation() + (sLight.m_distance * data.GetRotation().GetForward());
            Graphics::RenderEngine::Get()->DrawLine(data.GetLocation(), end1, Color::Red, 1.4f);
        }
        else if (tid == GetTypeID<DirectionalLightComponent>())
        {
            ECS::EntityDataComponent& data = ECS::Registry::Get()->get<ECS::EntityDataComponent>(ent);
            Vector3                   dir  = Vector3::Zero - data.GetLocation();
            Vector3                   end1 = data.GetLocation() + dir;
            Graphics::RenderEngine::Get()->DrawLine(data.GetLocation(), end1, Color::Red, 1.4f);
        }
        else if (tid == GetTypeID<ReflectionAreaComponent>())
        {
            ECS::EntityDataComponent&     data = ECS::Registry::Get()->get<ECS::EntityDataComponent>(ent);
            ECS::ReflectionAreaComponent& area = ECS::Registry::Get()->get<ECS::ReflectionAreaComponent>(ent);

            if (area.m_isLocal)
            {
                Event::EventSystem::Get()->Trigger<Event::EDrawBox>(Event::EDrawBox{data.GetLocation(), area.m_halfExtents, Color::Red, 3.4f});
            }

            Graphics::RenderEngine::Get()->DrawIcon(data.GetLocation(), HashedString("Resources/Editor/Textures/Icons/ReflectionIcon.png").value(), 2);
        }
        else if (tid == GetTypeID<CameraComponent>())
        {
            CameraComponent& camComp = ECS::Registry::Get()->get<CameraComponent>(ent);
        }
    }

    void ComponentDrawer::Initialize()
    {
        Event::EventSystem::Get()->Connect<EComponentOrderSwapped, &ComponentDrawer::OnComponentOrderSwapped>(this);
        Event::EventSystem::Get()->Connect<ETransformPivotChanged, &ComponentDrawer::OnTransformPivotChanged>(this);
    }

    void ComponentDrawer::OnComponentOrderSwapped(const EComponentOrderSwapped& ev)
    {
        Vector<TypeID>::iterator it1 = std::find(m_componentDrawList.begin(), m_componentDrawList.end(), ev.m_id1);
        Vector<TypeID>::iterator it2 = std::find(m_componentDrawList.begin(), m_componentDrawList.end(), ev.m_id2);
        std::iter_swap(it1, it2);
    }

    void ComponentDrawer::ClearDrawList()
    {
        m_componentDrawList.clear();
    }

    AddComponentMap ComponentDrawer::GetCurrentAddComponentMap(ECS::Entity entity)
    {
        AddComponentMap map;
        auto*           reg = ECS::Registry::Get();

        for (auto& store : reg->storage())
        {
            TypeID tid  = store.first;
            auto   meta = entt::resolve(tid);

            auto hasFunc = meta.func("has"_hs);
            if (!hasFunc)
                continue;

            bool hasComponent = hasFunc.invoke({}, entity).cast<bool>();
            if (hasComponent)
                continue;

            auto categoryProp = meta.prop("Category"_hs);

            String categoryStr = "";
            if (categoryProp)
                categoryStr = String(categoryProp.value().cast<const char*>());
            else
                categoryStr = "Default";

            auto titleProp = meta.prop("Title"_hs);
            if (!titleProp) continue;
            const char* title = titleProp.value().cast<const char*>();

            auto canAddProp = meta.prop("CanAddComponent"_hs);

            if (canAddProp)
            {
                const char* canAdd = canAddProp.value().cast<const char*>();

                if (String(canAdd).compare("1") == 0)
                    map[categoryStr].push_back(std::make_pair(String(title), tid));
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
        auto*  physicsEngine    = Physics::PhysicsEngine::Get();

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
                const String tooltipData = "Global: " + data.GetLocation().ToString();
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
                const String tooltipData = "Global: " + data.GetRotation().ToString();
                WidgetsUtility::Tooltip(tooltipData.c_str());
            }

            Vector3 rot = data.GetLocalRotationAngles();
            WidgetsUtility::DragVector3("##rot", &rot.x);
            data.SetLocalRotationAngles(rot);

            WidgetsUtility::PropertyLabel("Scale");
            if (ImGui::IsItemHovered())
            {
                const String tooltipData = "Global: " + data.GetScale().ToString();
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
            const String currentMaterial = phy.m_material.m_value->GetPath();
            StringID      selected        = WidgetsUtility::ResourceSelectionPhysicsMaterial("entity_phy_mat", &phy.m_material);

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

                for (Vector<TypeID>::iterator it = m_componentDrawList.begin(); it != m_componentDrawList.end(); it++)
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
                Map<entt::meta_data, StringID> materials;

                // Draw each reflected property in the component according to it's type.
                for (auto data : resolvedData.data())
                {
                    auto labelProperty = data.prop("Title"_hs);
                    auto typeProperty  = data.prop("Type"_hs);

                    if (!labelProperty || !typeProperty)
                        continue;

                    auto        label    = labelProperty.value().cast<const char*>();
                    String category = String(data.prop("Category"_hs).value().cast<const char*>());

                    ClassDrawer::AddPropertyToDrawList(category, data);
                }

                ClassDrawer::FlushDrawList(title, resolvedData, instance);

                // Check if the target data contains a debug property,
                // If so, and if the property is set to true,
                // Call the debug function if exists.
                auto debugProperty = resolvedData.data("m_drawDebug"_hs);
                if (debugProperty)
                {
                    if (debugProperty.get(instance).cast<bool>())
                        Drawer_Debug(tid, ent);
                }
            }
        }
    }

} // namespace Lina::Editor
