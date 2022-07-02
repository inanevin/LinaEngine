/*
Class: FrustumSystem



Timestamp: 12/24/2021 12:59:35 AM
*/

#pragma once

#ifndef FrustumSystem_HPP
#define FrustumSystem_HPP

// Headers here.
#include "Core/RenderEngineFwd.hpp"
#include "Core/CommonECS.hpp"
#include "ECS/System.hpp"
#include "Math/AABB.hpp"

namespace Lina
{
    class Vector3;
    class Quaternion;

    namespace Graphics
    {
        class Model;
        class ModelNode;
    } // namespace Graphics
} // namespace Lina

namespace Lina::ECS
{
    class FrustumSystem : public System
    {

    public:
        FrustumSystem()  = default;
        ~FrustumSystem() = default;

        virtual void Initialize(const String& name) override;
        virtual void UpdateComponents(float delta);

        /// <summary>
        /// Sets the bounds position & half extent based on given model node.
        /// </summary>
        void GetAABBInModelNode(Graphics::ModelNode* node, Vector3& outPosition, Vector3& outHalfExtent, const Vector3& location, const Quaternion& rot, const Vector3& scale);

        /// <summary>
        /// Sets the positions & half extents from all nodes in the given model.
        /// </summary>
        void GetAABBsInModel(Graphics::Model* model, Vector<Vector3>& outPositions, Vector<Vector3>& outHalfExtents, const Vector3& location, const Quaternion& rot, const Vector3& scale);

        /// <summary>
        /// Sets the bounds position & half extent based on given entity's transformation
        /// </summary>
        bool GetEntityBounds(Entity ent, Vector3& boundsPosition, Vector3& boundsHalfExtent);

        /// <summary>
        /// Sets the positions & extents based on all bounds on entity hierarchy.
        /// </summary>
        bool GetAllBoundsInEntity(Entity ent, Vector<Vector3>& boundsPositions, Vector<Vector3>& boundsHalfExtents);

    private:
        Graphics::RenderEngine* m_renderEngine = nullptr;
    };
} // namespace Lina::ECS

#endif
