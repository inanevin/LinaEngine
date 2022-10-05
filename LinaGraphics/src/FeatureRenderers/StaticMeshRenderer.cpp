/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-] [Inan Evin]

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

#include "FeatureRenderers/StaticMeshRenderer.hpp"
#include "Components/ModelNodeComponent.hpp"
#include "Core/Renderer.hpp"
#include "Core/RenderEngine.hpp"
#include "Resource/Mesh.hpp"
#include "Resource/Material.hpp"
#include "Resource/ModelNode.hpp"
#include "Utility/Vulkan/VulkanUtility.hpp"
#include "Core/World.hpp"

namespace Lina::Graphics
{
    void StaticMeshRenderer::Initialize()
    {
    }

    void StaticMeshRenderer::Shutdown()
    {
    }

    void StaticMeshRenderer::BatchRenderables(const Vector<RenderableComponent*>& renderables)
    {
        PROFILER_FUNC(PROFILER_THREAD_RENDER);
        m_batches.clear();

        for (const auto& r : renderables)
        {
            if (r->GetType() != RenderableType::RenderableStaticMesh)
                continue;

            Vector<MeshMaterialPair> pairs = r->GetMeshMaterialPairs();

            for (auto& p : pairs)
            {
                IndirectBatch* batch = FindInBatches(p);

                if (batch != nullptr)
                    batch->entityIDs.push_back(r->GetEntity()->GetID());
                else
                {
                    IndirectBatch newBatch;
                    newBatch.meshAndMaterial.material = p.material;
                    newBatch.meshAndMaterial.mesh     = p.mesh;
                    newBatch.entityIDs.push_back(r->GetEntity()->GetID());
                    m_batches.push_back(newBatch);
                }
            }
        }

        int a = 5;
    }

    void StaticMeshRenderer::RecordDrawCommands(CommandBuffer& cmd)
    {
        for (auto& batch : m_batches)
        {
            Material* mat      = batch.meshAndMaterial.material;
            Mesh* mesh = batch.meshAndMaterial.mesh;

            auto&     pipeline = mat->GetShaderHandle().value->GetPipeline();
            pipeline.Bind(cmd, PipelineBindPoint::Graphics);

            auto&          renderer = RenderEngine::Get()->GetLevelRenderer();
            DescriptorSet& descSet  = renderer.GetGlobalSet();
            DescriptorSet& objSet   = renderer.GetObjectSet();
            DescriptorSet& txtSet   = renderer.GetTextureSet();

            uint32_t uniformOffset = VulkanUtility::PadUniformBufferSize(sizeof(GPUSceneData)) * renderer.GetFrameIndex();

            cmd.CMD_BindDescriptorSets(PipelineBindPoint::Graphics, pipeline._layout, 0, 1, &descSet, 1, &uniformOffset);
            cmd.CMD_BindDescriptorSets(PipelineBindPoint::Graphics, pipeline._layout, 1, 1, &objSet, 0, nullptr);
            // cmd.CMD_BindDescriptorSets(PipelineBindPoint::Graphics, pipeline._layout, 2, 1, &txtSet, 0, nullptr);

            Graphics::MeshPushConstants constants;
            constants.renderMatrix = Matrix::Translate(Vector3(0, 0, 0));
            cmd.CMD_PushConstants(mat->GetShaderHandle().value->GetPipeline()._layout, GetShaderStage(ShaderStage::Vertex), 0, sizeof(Graphics::MeshPushConstants), &constants);

            for (auto entityID : batch.entityIDs)
            {
                uint64 offset = 0;
                cmd.CMD_BindVertexBuffers(0, 1, mesh->GetGPUVtxBuffer()._ptr, &offset);
                cmd.CMD_BindIndexBuffers(mesh->GetGPUIndexBuffer()._ptr, 0, IndexType::Uint32);
                cmd.CMD_DrawIndexed(static_cast<uint32>(mesh->GetIndexSize()), 1, 0, 0, 0);
            }

            // for (auto mr : m_renderables)
            // {
            //     Graphics::MeshPushConstants constants;
            //     constants.renderMatrix = Matrix::Translate(Vector3(0, 0, 0));
            //     GetCurrentFrame().commandBuffer.CMD_PushConstants(mat->GetShaderHandle().value->GetPipeline()._layout, GetShaderStage(ShaderSt
            //
            //     Model*     m = mr->m_modelHandle.IsValid() ? mr->m_modelHandle.value : RenderEngine::Get()->GetPlaceholderModel();
            //     ModelNode* n = mr->m_modelHandle.IsValid() ? RenderEngine::Get()->GetPlaceholderModelNode() : m->GetNodes()[mr->m_nodeIndex];
            //
            //     auto& meshes = n->GetMeshes();
            //     int   k      = 0;
            //
            //     for (auto mesh : meshes)
            //     {
            //         uint64 offset = 0;
            //         GetCurrentFrame().commandBuffer.CMD_BindVertexBuffers(0, 1, mesh->GetGPUVtxBuffer()._ptr, &offset);
            //         GetCurrentFrame().commandBuffer.CMD_BindIndexBuffers(mesh->GetGPUIndexBuffer()._ptr, 0, IndexType::Uint32);
            //         // buffer.CMD_Draw(static_cast<uint32>(rp.mesh->GetVertices().size()), 1, 0, i);
            //         GetCurrentFrame().commandBuffer.CMD_DrawIndexed(static_cast<uint32>(mesh->GetIndexSize()), 1, 0, 0, 0);
            //         k++;
            //     }
            // }
            // for (auto& rp : pair)
            // {
            //
            // }
        }
    }

    IndirectBatch* StaticMeshRenderer::FindInBatches(const MeshMaterialPair& pair)
    {
        for (auto& b : m_batches)
        {
            if (b.meshAndMaterial.mesh == pair.mesh && b.meshAndMaterial.material == pair.material)
                return &b;
        }

        return nullptr;
    }

} // namespace Lina::Graphics
