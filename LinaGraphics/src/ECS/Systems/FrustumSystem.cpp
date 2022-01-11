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

#include "ECS/Systems/FrustumSystem.hpp"

#include "ECS/Components/EntityDataComponent.hpp"
#include "ECS/Components/SpriteRendererComponent.hpp"
#include "ECS/Components/ModelNodeComponent.hpp"
#include "ECS/Registry.hpp"
#include "Rendering/Mesh.hpp"
#include "Math/Vector.hpp"
#include "Math/Math.hpp"
#include "EventSystem/GraphicsEvents.hpp"

namespace Lina::ECS
{
    void FrustumSystem::Initialize(const std::string& name)
    {
        System::Initialize(name);
    }

    void FrustumSystem::UpdateComponents(float delta)
    {
    }

    bool FrustumSystem::GetEntityBounds(Entity ent, Vector3& boundsPosition, Vector3& boundsHalfExtent)
    {
        auto* ecs = ECS::Registry::Get();

        ModelNodeComponent* mn = ecs->try_get<ModelNodeComponent>(ent);

        if (mn != nullptr)
        {
            Graphics::Model* model = mn->m_model.m_value;

            if (model != nullptr)
            {
                Graphics::ModelNode* node = model->GetAllNodes()[mn->m_nodeIndex];

                if (node != nullptr)
                {

                    EntityDataComponent& data           = ecs->get<EntityDataComponent>(ent);
                    const Vector3        entityLocation = data.GetLocation();
                    const Vector3        vertexOffset   = node->GetTotalVertexCenter() * data.GetScale();
                    Quaternion           objectRot      = data.GetRotation();
                    const Vector3        offsetAddition = objectRot.GetForward() * vertexOffset.z + objectRot.GetRight() * vertexOffset.x + objectRot.GetUp() * vertexOffset.y;
                    boundsPosition                      = entityLocation + offsetAddition;
 
                    std::vector<Vector3> boundsPositions = node->GetAABB().m_positions;

                    Vector3 totalMax = Vector3(-1000, -1000, -1000);
                    Vector3 totalMin = Vector3(1000, 1000, 1000);

                    for (auto& p : boundsPositions)
                    {
                        p = objectRot.GetRotated(p);
                        p = p * data.GetScale();

                        if (p.x > totalMax.x)
                            totalMax.x = p.x;
                        if (p.x < totalMin.x)
                            totalMin.x = p.x;

                        if (p.y > totalMax.y)
                            totalMax.y = p.y;
                        if (p.y < totalMin.y)
                            totalMin.y = p.y;

                        if (p.z > totalMax.z)
                            totalMax.z = p.z;
                        if (p.z < totalMin.z)
                            totalMin.z = p.z;
                    }

                    boundsHalfExtent = (totalMax - totalMin) / 2.0f * data.GetScale();
                    return true;
                }
            }
        }

        SpriteRendererComponent* sr = ecs->try_get<SpriteRendererComponent>(ent);
        if (sr != nullptr)
        {
            EntityDataComponent& data           = ecs->get<EntityDataComponent>(ent);
            const Vector3        entityLocation = data.GetLocation();
            boundsPosition                      = entityLocation;
            boundsHalfExtent                    = Vector3(0.5f, 0.5f, 0.5f) * data.GetScale() * data.GetRotation();
            return true;
        }

        return false;
    }

    bool FrustumSystem::GetAllBoundsInEntity(Entity ent, std::vector<Vector3>& boundsPositions, std::vector<Vector3>& boundsHalfExtents)
    {
        auto*               ecs = ECS::Registry::Get();
        ModelNodeComponent* mn  = ecs->try_get<ModelNodeComponent>(ent);

        if (mn != nullptr)
        {
            boundsPositions.emplace_back();
            boundsHalfExtents.emplace_back();
            GetEntityBounds(ent, boundsPositions.back(), boundsHalfExtents.back());
        }

        SpriteRendererComponent* sr = ecs->try_get<SpriteRendererComponent>(ent);

        if (sr != nullptr)
        {
            boundsPositions.emplace_back();
            boundsHalfExtents.emplace_back();
            GetEntityBounds(ent, boundsPositions.back(), boundsHalfExtents.back());
        }

        auto& data = ecs->get<ECS::EntityDataComponent>(ent);

        for (auto& child : data.m_children)
        {
            GetAllBoundsInEntity(child, boundsPositions, boundsHalfExtents);
        }

        return boundsPositions.size() != 0;
    }

    void FrustumSystem::TransformAABB(Vector3& aabbLoc, Vector3& aabbHalfExtents, const Vector3& vertexCenter, const std::vector<Vector3>& aabbPositions, const Vector3& objectLoc, const Quaternion& objectRot, const Vector3& objectScale)
    {
        // TODO: better incorporation of object rotation.
        const Vector3 entityLocation = objectLoc;
        const Vector3 vertexOffset   = vertexCenter * objectScale;
        const Vector3 offsetAddition = objectRot.GetForward() * vertexOffset.z + objectRot.GetRight() * vertexOffset.x + objectRot.GetUp() * vertexOffset.y;

        aabbLoc = entityLocation + offsetAddition;

        std::vector<Vector3> boundsPositions = aabbPositions;

        Vector3 totalMax = Vector3(-1000, -1000, -1000);
        Vector3 totalMin = Vector3(1000, 1000, 1000);
        for (auto& p : boundsPositions)
        {
            p = p * objectScale * objectRot;

            if (p.x > totalMax.x)
                totalMax.x = p.x;
            else if (p.x < totalMin.x)
                totalMin.x = p.x;

            if (p.y > totalMax.y)
                totalMax.y = p.y;
            else if (p.y < totalMin.y)
                totalMin.y = p.y;

            if (p.z > totalMax.z)
                totalMax.z = p.z;
            else if (p.z < totalMin.z)
                totalMin.z = p.z;
        }

        aabbHalfExtents = (totalMax - totalMin) / 2.0f;
    }
} // namespace Lina::ECS
