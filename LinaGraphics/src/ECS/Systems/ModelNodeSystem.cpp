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

#include "ECS/Systems/ModelNodeSystem.hpp"

#include "Animation/Skeleton.hpp"
#include "Core/RenderDeviceBackend.hpp"
#include "Core/RenderEngineBackend.hpp"
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
    void ModelNodeSystem::Initialize(const std::string& name, ApplicationMode appMode)
    {
        System::Initialize(name);
        m_appMode      = appMode;
        m_renderEngine = Graphics::RenderEngineBackend::Get();
        m_renderDevice = m_renderEngine->GetRenderDevice();
    }

    void ModelNodeSystem::ConstructEntityHierarchy(Entity entity, Matrix& parentTransform, Graphics::Model* model, Graphics::ModelNode* node)
    {
        auto* ecs = ECS::Registry::Get();

        const auto&         meshes     = node->GetMeshes();
        Graphics::Material* defaultMat = Graphics::RenderEngineBackend::Get()->GetDefaultLitMaterial();

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
            }
        }

        for (uint32 i = 0; i < node->m_children.size(); i++)
        {
            const std::string childName   = node->m_children[i]->m_name;
            Entity            childEntity = ecs->CreateEntity(childName);
            auto&             data        = ecs->get<ECS::EntityDataComponent>(childEntity);
            Matrix            mat         = parentTransform * node->m_localTransform;
            data.SetTransformation(mat, false);

            if (model->GetAssetData()->m_enitiesHasPivots)
            {

                Vector3 totalLocalOffset = Vector3::Zero;
                auto&   childMeshes      = node->m_children[i]->GetMeshes();

                for (auto& mesh : childMeshes)
                    totalLocalOffset += mesh->GetVertexCenter();

                Entity pivotEntity = ecs->CreateEntity(childName + "_pvt");
                ecs->AddChildToEntity(entity, pivotEntity);

                const Vector3 vertexOffset   = totalLocalOffset * data.GetScale();
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
        const std::string    parentEntityName = Utility::GetFileWithoutExtension(Utility::GetFileNameOnly(model->GetPath()));
        Entity               parentEntity     = ECS::Registry::Get()->CreateEntity(parentEntityName);
        auto&                data             = ECS::Registry::Get()->get<ECS::EntityDataComponent>(parentEntity);
        ConstructEntityHierarchy(parentEntity, data.ToMatrix(), model, root);
    }

    static float t = 0.0f;

    void ModelNodeSystem::UpdateComponents(float delta)
    {
        auto* ecs = ECS::Registry::Get();

        auto view  = ecs->view<EntityDataComponent, ModelNodeComponent>();
        m_poolSize = (int)view.size_hint();
        t += 0.016f;

        if (t > 3.0f)
        {
            t = -1000;
            // Graphics::Model& model = Graphics::Model::GetModel(StringID("Resources/Sandbox/Target/RicochetTarget.fbx").value());
            // CreateModelHierarchy(model);
        }
        for (auto entity : view)
        {
            ModelNodeComponent& nodeComponent = view.get<ModelNodeComponent>(entity);
            auto&               data          = view.get<EntityDataComponent>(entity);

            if (!nodeComponent.GetIsEnabled() || !data.GetIsEnabled())
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
                const StringIDType materialSID = nodeComponent.m_materials[i].m_sid;
                if (!Resources::ResourceStorage::Get()->Exists<Graphics::Material>(materialSID))
                    continue;

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

    void ModelNodeSystem::FlushModelNode(Graphics::ModelNode* node, Graphics::DrawParams& params, Graphics::Material* overrideMaterial)
    {
        auto& meshes = node->m_meshes;

        for (auto* mesh : meshes)
        {
            Graphics::VertexArray& vertexArray = mesh->GetVertexArray();
            Matrix                 models      = Matrix::Identity();

            // Update the buffer w/ each transform.
            vertexArray.UpdateBuffer(7, &models[0][0], 1 * sizeof(Matrix));
            auto* mat = overrideMaterial == nullptr ? m_renderEngine->GetDefaultLitMaterial() : overrideMaterial;

            mat->SetBool(UF_BOOL_SKINNED, false);

            m_renderEngine->UpdateShaderData(mat);
            m_renderDevice->Draw(vertexArray.GetID(), params, (uint32)1, vertexArray.GetIndexCount(), false);
        }

        for (auto* child : node->m_children)
            FlushModelNode(child, params, overrideMaterial);
    }

    void ModelNodeSystem::FlushOpaque(Graphics::DrawParams& drawParams, Graphics::Material* overrideMaterial, bool completeFlush)
    {
        // When flushed, all the data is delegated to the render device to do the actual
        // drawing. Then the data is cleared if complete flush is requested.

        for (std::map<Graphics::BatchDrawData, Graphics::BatchModelData>::iterator it = m_opaqueRenderBatch.begin(); it != m_opaqueRenderBatch.end(); ++it)
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

            // Update the buffer w/ each transform.
            vertexArray->UpdateBuffer(7, models, numTransforms * sizeof(Matrix));

            if (modelData.m_boneTransformations.size() == 0)
                mat->SetBool(UF_BOOL_SKINNED, false);
            else
                mat->SetBool(UF_BOOL_SKINNED, true);

            for (int i = 0; i < modelData.m_boneTransformations.size(); i++)
                mat->SetMatrix4(std::string(UF_BONE_MATRICES) + "[" + std::to_string(i) + "]", modelData.m_boneTransformations[i]);

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
                mat->SetMatrix4(std::string(UF_BONE_MATRICES) + "[" + std::to_string(i) + "]", modelData.m_boneTransformations[i]);
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
