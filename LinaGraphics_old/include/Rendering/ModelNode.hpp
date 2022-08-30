/*
Class: ModelNode



Timestamp: 12/24/2021 9:00:02 PM
*/

#pragma once

#ifndef ModelNode_HPP
#define ModelNode_HPP

// Headers here.
#include "Math/Matrix.hpp"
#include "Utility/StringId.hpp"
#include "Resources/ResourceHandle.hpp"
#include "Rendering/Material.hpp"
#include "Math/AABB.hpp"
#include <memory>
#include "Data/Vector.hpp"

struct aiNode;
struct aiScene;

namespace Lina
{
    namespace ECS
    {
        class ModelNodeSystem;
        class FrustumSystem;
    }
} // namespace Lina

namespace Lina::Graphics
{
    class Mesh;
    class Model;
    class ModelLoader;

    class ModelNode
    {

    public:
        ModelNode(){};
        ~ModelNode();

        void                             FillNodeHierarchy(const aiNode* node, const aiScene* scene, Model* parentModel);
        inline const Vector<Mesh*>& GetMeshes() const
        {
            return m_meshes;
        }
        inline const String& GetName() const
        {
            return m_name;
        }
        inline const Vector<ModelNode*>& GetChildren() const
        {
            return m_children;
        }
        inline int GetIndexInParentHierarchy()
        {
            return m_nodeIndexInParentHierarchy;
        }
        inline Vector3 GetTotalVertexCenter()
        {
            return m_totalVertexCenter;
        }
        inline AABB& GetAABB()
        {
            return m_aabb;
        }

    private:
        void Clear()
        {
            m_name           = "";
            m_localTransform = Matrix();
            m_children.clear();
        }

    private:
        friend class ECS::ModelNodeSystem;
        friend class ECS::FrustumSystem;
        friend class Graphics::ModelLoader;
        friend class cereal::access;

        int                     m_nodeIndexInParentHierarchy = 0;
        Vector<Mesh*>      m_meshes;
        String             m_name              = "";
        Vector3                 m_totalVertexCenter = Vector3::Zero;
        AABB                    m_aabb;
        Matrix                  m_localTransform;
        Vector<ModelNode*> m_children;
    };
} // namespace Lina::Graphics

#endif
