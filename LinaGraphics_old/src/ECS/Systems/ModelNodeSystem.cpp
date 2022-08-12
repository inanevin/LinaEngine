#include "ECS/Systems/ModelNodeSystem.hpp"

#include "Animation/Skeleton.hpp"
#include "Core/RenderDevice.hpp"
#include "Core/RenderEngine.hpp"
#include "ECS/Components/EntityDataComponent.hpp"
#include "ECS/Components/ModelNodeComponent.hpp"
#include "ECS/Components/ModelRendererComponent.hpp"
#include "ECS/Registry.hpp"
#include "EventSystem/EventSystem.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/Model.hpp"
#include "Rendering/RenderConstants.hpp"
#include "Utility/UtilityFunctions.hpp"

namespace Lina::ECS
{
    void ModelNodeSystem::Initialize(const String& name, ApplicationMode appMode)
    {
        System::Initialize(name);
        m_appMode      = appMode;
        m_renderEngine = Graphics::RenderEngine::Get();
        m_renderDevice = m_renderEngine->GetRenderDevice();
    }

    void ModelNodeSystem::ConstructEntityHierarchy(Entity entity, Matrix& parentTransform, Graphics::Model* model, Graphics::ModelNode* node)
    {
        auto* ecs = ECS::Registry::Get();

        const auto&         meshes     = node->GetMeshes();
        Graphics::Material* defaultMat = Graphics::RenderEngine::Get()->GetDefaultLitMaterial();

        if (meshes.size() > 0)
        {
            ModelNodeComponent& nodeComponent = ecs->emplace<ModelNodeComponent>(entity);
            nodeComponent.m_nodeIndex         = node->m_nodeIndexInParentHierarchy;
            nodeComponent.m_model.m_sid       = model->GetSID();
            nodeComponent.m_model.m_value     = model;

            for (uint32 i = 0; i < meshes.size(); i++)
            {
                nodeComponent.m_materials.emplace_back();
                nodeComponent.m_materials.back().m_sid   = defaultMat->GetSID();
                nodeComponent.m_materials.back().m_value = defaultMat;
                nodeComponent.m_materialsNames.emplace_back();
                nodeComponent.m_materialsNames.back() = model->GetImportedMaterials()[meshes[i]->GetMaterialSlotIndex()].m_name;
            }
        }

        for (uint32 i = 0; i < node->m_children.size(); i++)
        {
            const String childName   = node->m_children[i]->m_name;
            Entity            childEntity = ecs->CreateEntity(childName);
            auto&             data        = ecs->get<ECS::EntityDataComponent>(childEntity);
            Matrix            mat         = parentTransform * node->m_localTransform;
            data.SetTransformation(mat, false);

            if (model->GetAssetData()->m_generatePivots)
            {

                Entity pivotEntity = ecs->CreateEntity(childName + "_pvt");
                ecs->AddChildToEntity(entity, pivotEntity);

                const Vector3 vertexOffset   = node->m_children[i]->m_totalVertexCenter * data.GetScale();
                const Vector3 offsetAddition = data.GetRotation().GetForward() * vertexOffset.z +
                                               data.GetRotation().GetRight() * vertexOffset.x +
                                               data.GetRotation().GetUp() * vertexOffset.y;

                auto& pivotParentData = ecs->get<ECS::EntityDataComponent>(pivotEntity);
                pivotParentData.SetLocation(data.GetLocation() + offsetAddition);
                pivotParentData.SetRotation(data.GetRotation());
                ecs->AddChildToEntity(pivotEntity, childEntity);
            }
            else
            {
                ecs->AddChildToEntity(entity, childEntity);
            }

            ConstructEntityHierarchy(childEntity, data.ToMatrix(), model, node->m_children[i]);
        }
    }

    void ModelNodeSystem::CreateModelHierarchy(Graphics::Model* model)
    {
        Graphics::ModelNode* root             = model->GetRootNode();
        const String    parentEntityName = Utility::GetFileWithoutExtension(Utility::GetFileNameOnly(model->GetPath()));
        Entity               parentEntity     = ECS::Registry::Get()->CreateEntity(parentEntityName);
        auto&                data             = ECS::Registry::Get()->get<ECS::EntityDataComponent>(parentEntity);
        ConstructEntityHierarchy(parentEntity, data.ToMatrix(), model, root);
    }

    static float t = 0.0f;

    void ModelNodeSystem::UpdateComponents(float delta)
    {
        auto* ecs  = ECS::Registry::Get();
        auto  view = ecs->view<EntityDataComponent, ModelNodeComponent>();
        m_poolSize = 0;

        for (auto entity : view)
        {
            ModelNodeComponent& nodeComponent = view.get<ModelNodeComponent>(entity);
            auto&               data          = view.get<EntityDataComponent>(entity);

            if (!nodeComponent.GetIsEnabled() || !data.GetIsEnabled() || nodeComponent.m_culled)
                continue;

            auto* model = nodeComponent.m_model.m_value;
            if (model == nullptr)
                continue;

            auto* node = model->GetAllNodes()[nodeComponent.m_nodeIndex];
            if (node == nullptr)
                continue;

            auto& meshes = node->GetMeshes();

            const Matrix finalMatrix = data.ToMatrix();

            for (uint32 i = 0; i < meshes.size(); i++)
            {
                auto*  mesh         = meshes[i];
                uint32 materialSlot = mesh->GetMaterialSlotIndex();

                // Check if material exists.
                const StringID materialSID = nodeComponent.m_materials[i].m_sid;
                if (!Resources::ResourceStorage::Get()->Exists<Graphics::Material>(materialSID))
                    continue;

                m_poolSize++;

                // Render the material & vertex array.
                Graphics::Material* mat = nodeComponent.m_materials[i].m_value;

                if (mat->GetSurfaceType() == Graphics::MaterialSurfaceType::Opaque)
                    RenderOpaque(mesh->GetVertexArray(), Graphics::Skeleton(), mat, finalMatrix);
                else
                {
                    float priority = (m_renderEngine->GetCameraSystem()->GetCameraLocation() - data.GetLocation()).MagnitudeSqrt();
                    RenderTransparent(mesh->GetVertexArray(), Graphics::Skeleton(), mat, finalMatrix, priority);
                }
            }
        }
    }

    void ModelNodeSystem::RenderOpaque(Graphics::VertexArray& vertexArray, Graphics::Skeleton& skeleton, Graphics::Material* material, const Matrix& transformIn)
    {
        // Render commands basically add the necessary
        // draw data into the maps/lists etc.
        Graphics::BatchDrawData drawData;
        drawData.m_vertexArray = &vertexArray;
        drawData.m_material    = material;
        m_opaqueRenderBatch[drawData].m_models.push_back(transformIn);

        if (skeleton.IsLoaded())
        {
        }
    }

    void ModelNodeSystem::RenderTransparent(Graphics::VertexArray& vertexArray, Graphics::Skeleton& skeleton, Graphics::Material* material, const Matrix& transformIn, float priority)
    {
        // Render commands basically add the necessary
        // draw data into the maps/lists etc.
        Graphics::BatchDrawData drawData;
        drawData.m_vertexArray = &vertexArray;
        drawData.m_material    = material;
        drawData.m_distance    = priority;

        Graphics::BatchModelData modelData;
        modelData.m_models.push_back(transformIn);

        if (skeleton.IsLoaded())
        {

        }
    }

    void ModelNodeSystem::FlushModelNode(Graphics::ModelNode* node, Matrix& parentMatrix, Graphics::DrawParams& params, Graphics::Material* overrideMaterial)
    {
        auto&  meshes      = node->m_meshes;
        Matrix modelMatrix = parentMatrix;

        for (auto* mesh : meshes)
        {
            Graphics::VertexArray& vertexArray = mesh->GetVertexArray();

            // Update the buffer w/ each transform.
            vertexArray.UpdateBuffer(7, &modelMatrix[0][0], 1 * sizeof(Matrix));
            auto* mat = overrideMaterial == nullptr ? m_renderEngine->GetDefaultLitMaterial() : overrideMaterial;

            mat->SetBool(UF_BOOL_SKINNED, false);

            m_renderEngine->UpdateShaderData(mat);
            m_renderDevice->Draw(vertexArray.GetID(), params, (uint32)1, vertexArray.GetIndexCount(), false);
        }

        for (auto* child : node->m_children)
            FlushModelNode(child, modelMatrix, params, overrideMaterial);
    }

    void ModelNodeSystem::FlushOpaque(Graphics::DrawParams& drawParams, Graphics::Material* overrideMaterial, bool completeFlush)
    {
        // When flushed, all the data is delegated to the render device to do the actual
        // drawing. Then the data is cleared if complete flush is requested.

        for (Map<Graphics::BatchDrawData, Graphics::BatchModelData>::iterator it = m_opaqueRenderBatch.begin(); it != m_opaqueRenderBatch.end(); ++it)
        {
            // Get references.
            Graphics::BatchDrawData   drawData      = it->first;
            Graphics::BatchModelData& modelData     = it->second;
            size_t                    numTransforms = modelData.m_models.size();
            if (numTransforms == 0)
                continue;

            Graphics::VertexArray* vertexArray = drawData.m_vertexArray;
            Matrix*                models      = &modelData.m_models[0];

            // Get the material for drawing, object's own material or overriden material.
            Graphics::Material* mat = overrideMaterial == nullptr ? drawData.m_material : overrideMaterial;

            m_renderEngine->GetReflectionSystem()->SetReflectionsOnMaterial(mat, models->GetTranslation());

            // Update the buffer w/ each transform.
            vertexArray->UpdateBuffer(7, models, numTransforms * sizeof(Matrix));

            if (modelData.m_boneTransformations.size() == 0)
                mat->SetBool(UF_BOOL_SKINNED, false);
            else
                mat->SetBool(UF_BOOL_SKINNED, true);

            for (int i = 0; i < modelData.m_boneTransformations.size(); i++)
                mat->SetMatrix4(String(UF_BONE_MATRICES) + "[" + TO_STRING(i) + "]", modelData.m_boneTransformations[i]);

            m_renderEngine->UpdateShaderData(mat);

            m_renderDevice->Draw(vertexArray->GetID(), drawParams, (uint32)numTransforms, vertexArray->GetIndexCount(), false);

            // Clear the buffer.
            if (completeFlush)
            {
                modelData.m_models.clear();
                modelData.m_boneTransformations.clear();
            }
        }
    }

    void ModelNodeSystem::FlushTransparent(Graphics::DrawParams& drawParams, Graphics::Material* overrideMaterial, bool completeFlush)
    {
        // When flushed, all the data is delegated to the render device to do the actual
        // drawing. Then the data is cleared if complete flush is requested.

        // Empty out the queue
        while (!m_transparentRenderBatch.empty())
        {
            BatchPair pair = m_transparentRenderBatch.top();

            Graphics::BatchDrawData&  drawData      = std::get<0>(pair);
            Graphics::BatchModelData& modelData     = std::get<1>(pair);
            size_t                    numTransforms = modelData.m_models.size();
            if (numTransforms == 0)
                continue;

            Graphics::VertexArray* vertexArray = drawData.m_vertexArray;
            Matrix*                models      = &modelData.m_models[0];

            // Get the material for drawing, object's own material or overriden material.
            Graphics::Material* mat = overrideMaterial == nullptr ? drawData.m_material : overrideMaterial;

            // Draw call.
            // Update the buffer w/ each transform.
            vertexArray->UpdateBuffer(7, models, numTransforms * sizeof(Matrix));

            if (modelData.m_boneTransformations.size() == 0)
                mat->SetBool(UF_BOOL_SKINNED, false);
            else
                mat->SetBool(UF_BOOL_SKINNED, true);

            for (int i = 0; i < modelData.m_boneTransformations.size(); i++)
                mat->SetMatrix4(String(UF_BONE_MATRICES) + "[" + TO_STRING(i) + "]", modelData.m_boneTransformations[i]);
            m_renderEngine->UpdateShaderData(mat);
            m_renderDevice->Draw(vertexArray->GetID(), drawParams, (uint32)numTransforms, vertexArray->GetIndexCount(), false);

            // Clear the buffer.
            if (completeFlush)
            {
                modelData.m_models.clear();
                modelData.m_boneTransformations.clear();
            }

            m_transparentRenderBatch.pop();
        }
    }

} // namespace Lina::ECS
