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

#include "Drawers/ClassDrawer.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "ECS/Components/EntityDataComponent.hpp"
#include "ECS/Components/PhysicsComponent.hpp"
#include "Core/PhysicsBackend.hpp"
#include <entt/meta/factory.hpp>
#include <entt/meta/meta.hpp>
#include <entt/meta/node.hpp>
#include <entt/meta/resolve.hpp>
#include "IconsFontAwesome5.h"

using namespace entt::literals;

namespace Lina::Editor
{
    void ClassDrawer::DrawClass(TypeID tid, bool drawHeader)
    {
        auto& resolved = entt::resolve(tid);

        if (resolved)
        {
            if (drawHeader)
            {
            }
        }
    }

    void ClassDrawer::DrawEntityData(ECS::Entity entity)
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
} // namespace Lina::Editor