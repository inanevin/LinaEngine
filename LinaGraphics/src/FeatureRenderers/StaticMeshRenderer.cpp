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
    void StaticMeshRenderer::Initialize(FeatureRendererManager& manager)
    {
        manager.onExtractPerView.push_back(std::bind(&StaticMeshRenderer::OnExtractPerView, this, std::placeholders::_1, std::placeholders::_2));
        manager.onPrepare.push_back(std::bind(&StaticMeshRenderer::OnPrepare, this));
        manager.onSubmit.push_back(std::bind(&StaticMeshRenderer::OnSubmit, this, std::placeholders::_1, std::placeholders::_2));
    }

    void StaticMeshRenderer::Shutdown()
    {
    }

    void StaticMeshRenderer::OnExtractPerView(World::EntityWorld* world, View* v)
    {
        if (v->GetType() != ViewType::Player)
            return;

        PROFILER_FUNC(PROFILER_THREAD_MAIN);
        ExtractedData extData;
        uint32        modelNodeCompsSize = 0;
        const auto&   modelNodes         = world->View<ModelNodeComponent>(&modelNodeCompsSize);

        const auto& visibles     = v->GetVisibleObjects();
        auto&       visibleNodes = extData.nodeComponents;

        visibleNodes.resize(modelNodeCompsSize, nullptr);

        Taskflow tf;
        tf.for_each_index(0, static_cast<int>(modelNodeCompsSize), 1, [&](int i) {
            ModelNodeComponent* comp = modelNodes[i];
            if (comp == nullptr)
                return;

            if (visibles[comp->GetRenderableID()] == nullptr)
                return;

            visibleNodes[i] = comp;
        });
        JobSystem::Get()->GetMainExecutor().Run(tf).wait();

        m_mtx.lock();
        m_extractQueue.push(extData);
        m_mtx.unlock();

        // // Don't allow shit.
        // if (m_extractQueue.size() > 2)
        //     m_extractQueue.pop();
    }

    void StaticMeshRenderer::OnPrepare()
    {
        PROFILER_FUNC(PROFILER_THREAD_RENDER);

        if (m_extractQueue.empty())
            return;

        m_mtx.lock();
        ExtractedData extData = m_extractQueue.front();
        m_extractQueue.pop();
        m_mtx.unlock();

        const auto&  nodeComponents = extData.nodeComponents;
        const uint32 size           = static_cast<uint32>(nodeComponents.size());

        Taskflow tf;
        Mutex    mtx;
        tf.for_each_index(0, static_cast<int>(size), 1, [&](int i) {
            auto* comp = nodeComponents[i];

            if (comp == nullptr)
                return;

            Model*     model = comp->m_modelHandle.IsValid() ? comp->m_modelHandle.value : RenderEngine::Get()->GetPlaceholderModel();
            ModelNode* node  = comp->m_modelHandle.IsValid() ? model->GetNodes()[comp->m_nodeIndex] : RenderEngine::Get()->GetPlaceholderModelNode();

            for (auto m : node->GetMeshes())
            {
                const uint32 meshSlot = static_cast<uint32>(m->GetMaterialSlot());
                auto&        handle   = comp->m_materials[meshSlot];
                Material*    mat      = handle.IsValid() ? handle.value : RenderEngine::Get()->GetPlaceholderMaterial();

                RenderPair pair = RenderPair{
                    .transform = comp->GetEntity()->GetTransform().ToMatrix(),
                    .mesh      = m,
                };

                mtx.lock();
                m_gpuData[mat].push_back(pair);
                mtx.unlock();
            }
        });

        JobSystem::Get()->GetMainExecutor().Run(tf).wait();
    }

    void StaticMeshRenderer::OnSubmit(CommandBuffer& buffer, View* v)
    {
        if (v->GetType() != ViewType::Player)
            return;

        PROFILER_FUNC(PROFILER_THREAD_RENDER);

        for (auto [mat, pair] : m_gpuData)
        {
            auto& pipeline = mat->GetShaderHandle().value->GetPipeline();
            pipeline.Bind(buffer, PipelineBindPoint::Graphics);

            auto&          renderer = RenderEngine::Get()->GetLevelRenderer();
            DescriptorSet& descSet  = renderer.GetGlobalSet();
            DescriptorSet& objSet = renderer.GetObjectSet();
            DescriptorSet& txtSet   = renderer.GetTextureSet();
            
            uint32_t uniformOffset = VulkanUtility::PadUniformBufferSize(sizeof(GPUSceneData)) * renderer.GetFrameIndex();

            buffer.CMD_BindDescriptorSets(PipelineBindPoint::Graphics, pipeline._layout, 0, 1, &descSet, 1, &uniformOffset);

            buffer.CMD_BindDescriptorSets(PipelineBindPoint::Graphics, pipeline._layout, 1, 1, &objSet, 0, nullptr);

            buffer.CMD_BindDescriptorSets(PipelineBindPoint::Graphics, pipeline._layout, 2, 1, &txtSet, 0, nullptr);


            int i = 0;
            for (auto& rp : pair)
            {
                Graphics::MeshPushConstants constants;
                constants.renderMatrix = rp.transform;
                buffer.CMD_PushConstants(mat->GetShaderHandle().value->GetPipeline()._layout, GetShaderStage(ShaderStage::Vertex), 0, sizeof(Graphics::MeshPushConstants), &constants);

                uint64 offset = 0;
                buffer.CMD_BindVertexBuffers(0, 1, rp.mesh->GetGPUVtxBuffer()._ptr, &offset);
                buffer.CMD_BindIndexBuffers(rp.mesh->GetGPUIndexBuffer()._ptr, 0, IndexType::Uint32);
                // buffer.CMD_Draw(static_cast<uint32>(rp.mesh->GetVertices().size()), 1, 0, i);
                buffer.CMD_DrawIndexed(static_cast<uint32>(rp.mesh->GetIndexSize()), 1, 0, 0, 0);
                i++;
            }
        }

        m_gpuData.clear();
    }

} // namespace Lina::Graphics
