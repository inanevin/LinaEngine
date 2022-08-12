/*
Class: ModelRendererComponent



Timestamp: 12/8/2021 12:27:21 PM
*/

#pragma once

#ifndef ModelRendererComponent_HPP
#define ModelRendererComponent_HPP

// Headers here.
#include "Core/CommonECS.hpp"
#include "ECS/Component.hpp"
#include "Math/Matrix.hpp"
#include "Utility/StringId.hpp"

#include "Data/Serialization/StringSerialization.hpp"
#include <Data/Serialization/VectorSerialization.hpp>

namespace Lina
{
    namespace World
    {
        class Level;
    }

    namespace Graphics
    {
        class Model;
        class ModelNode;
        class Material;
    } // namespace Graphics
} // namespace Lina

namespace Lina::Editor
{
    class ComponentDrawer;
}

namespace Lina::ECS
{
    class ModelNodeSystem;

    struct ModelRendererComponent : public Component
    {

        void SetModel(ECS::Entity parent, Graphics::Model& model);
        void RemoveModel(ECS::Entity parent);
        void SetMaterial(ECS::Entity parent, int materialIndex, const Graphics::Material& material);
        void RemoveMaterial(ECS::Entity parent, int materialIndex);
        bool GetGenerateMeshPivots()
        {
            return m_generateMeshPivots;
        }
        void         RefreshHierarchy(ECS::Entity parent);
        StringID GetModelID()
        {
            return m_modelID;
        }
        String GetModelPath()
        {
            return m_modelPath;
        }
        Vector<String> GetMaterialPaths()
        {
            return m_materialPaths;
        }

    private:
        void ProcessNode(ECS::Entity parent, const Matrix& parentTransform, Graphics::ModelNode& node, Graphics::Model& model, bool isRoot = false);
        void AddMeshRenderer(ECS::Entity targetEntity, const Vector<int>& meshIndexes, Graphics::Model& model);

    private:
        friend class cereal::access;
        friend class ECS::ModelNodeSystem;
        friend class World::Level;
        friend class Editor::ComponentDrawer;

        StringID             m_modelID         = 0;
        String              m_modelPath       = "";
        String              m_modelParamsPath = "";
        Vector<String> m_materialPaths;
        int                      m_materialCount      = -1;
        bool                     m_generateMeshPivots = false;

        template <class Archive> void serialize(Archive& archive)
        {
            archive(m_modelPath, m_modelParamsPath, m_materialCount, m_materialPaths, m_isEnabled, m_generateMeshPivots);
        }
    };
} // namespace Lina::ECS

#endif
