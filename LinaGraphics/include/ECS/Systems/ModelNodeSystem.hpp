/*
Class: ModelNodeSystem

Responsible for adding all the mesh renderers into a pool which is then
flushed to draw those renderers' data by the RenderEngine.

Timestamp: 4/27/2019 5:38:44 PM
*/

#pragma once

#ifndef ModelNodeSystem_HPP
#define ModelNodeSystem_HPP

#include "Core/CommonApplication.hpp"
#include "Core/CommonECS.hpp"
#include "Core/RenderEngineFwd.hpp"
#include "ECS/System.hpp"
#include "Math/Matrix.hpp"

#include "Data/Map.hpp"
#include <queue>

namespace Lina
{

    namespace Graphics
    {
        class Material;
        class Skeleton;
        class VertexArray;
        struct DrawParams;

        struct BatchDrawData
        {
            Graphics::VertexArray* m_vertexArray;
            Graphics::Material*    m_material;
            float                  m_distance;
        };

        struct BatchModelData
        {
            Vector<Matrix> m_models;
            Vector<Matrix> m_boneTransformations;
        };

        class ModelNode;
        class Model;
    } // namespace Graphics
} // namespace Lina

namespace Lina::ECS
{
    struct EntityDataComponent;

    class ModelNodeSystem : public System
    {

    public:
        typedef std::tuple<Graphics::BatchDrawData, Graphics::BatchModelData> BatchPair;

        struct BatchComparison
        {
            bool const operator()(const BatchPair& lhs, const BatchPair& rhs) const
            {
                return std::get<0>(lhs).m_distance < std::get<0>(rhs).m_distance;
            }
        };

        struct BatchDrawDataComp
        {
            bool const operator()(const Graphics::BatchDrawData& lhs, const Graphics::BatchDrawData& rhs) const
            {
                return std::tie(lhs.m_vertexArray, lhs.m_material) < std::tie(rhs.m_vertexArray, rhs.m_material);
            }
        };

        ModelNodeSystem()          = default;
        virtual ~ModelNodeSystem() = default;

        void         Initialize(const String& name, ApplicationMode appMode);
        virtual void UpdateComponents(float delta) override;

        /// <summary>
        /// Creates a full entity hierarchy based on the given model, automatically fills the parent-child relationships and model node components.
        /// </summary>
        /// <param name="model"></param>
        void CreateModelHierarchy(Graphics::Model* model);

        /// <summary>
        /// Pushes an object into the opaque render stack.
        /// </summary>
        void RenderOpaque(Graphics::VertexArray& vertexArray, Graphics::Skeleton& skeleton, Graphics::Material* material, const Matrix& transformIn);

        /// <summary>
        /// Pushes the given vertex array into the transparent render stack.
        /// </summary>
        void RenderTransparent(Graphics::VertexArray& vertexArray, Graphics::Skeleton& skeleton, Graphics::Material* material, const Matrix& transformIn, float priority);

        /// <summary>
        /// Draws all objects in the opaque stack.
        /// </summary>
        void FlushOpaque(Graphics::DrawParams& drawParams, Graphics::Material* overrideMaterial = nullptr, bool completeFlush = true);

        /// <summary>
        /// Draws all objects in the transparent stack.
        /// </summary>
        void FlushTransparent(Graphics::DrawParams& drawParams, Graphics::Material* overrideMaterial = nullptr, bool completeFlush = true);

        /// <summary>
        /// Draws a single model given the root node.
        /// </summary>
        void FlushModelNode(Graphics::ModelNode* node, Matrix& parentMatrix, Graphics::DrawParams& params, Graphics::Material* overrideMaterial = nullptr);

    private:
        void ConstructEntityHierarchy(Entity entity, Matrix& parentTransform, Graphics::Model* model, Graphics::ModelNode* node);

    private:
        Graphics::RenderDevice* m_renderDevice = nullptr;
        Graphics::RenderEngine* m_renderEngine = nullptr;
        ApplicationMode         m_appMode      = ApplicationMode::Editor;
        // Map & queue to see the list of same vertex array & textures to compress them into single draw call.
        Map<Graphics::BatchDrawData, Graphics::BatchModelData, BatchDrawDataComp> m_opaqueRenderBatch;
        std::priority_queue<BatchPair, Vector<BatchPair>, BatchComparison>        m_transparentRenderBatch;
    };
} // namespace Lina::ECS

#endif
