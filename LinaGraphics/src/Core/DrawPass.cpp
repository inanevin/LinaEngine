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

#include "Core/DrawPass.hpp"
#include "Core/View.hpp"
#include "Components/RenderableComponent.hpp"
#include "Core/RenderEngine.hpp"
#include "Core/Entity.hpp"
#include "Core/Renderer.hpp"
#include "Resource/Mesh.hpp"
#include "Resource/Material.hpp"
#include "Resource/ModelNode.hpp"
#include "Utility/Vulkan/VulkanUtility.hpp"

namespace Lina::Graphics
{

    float  lastReportTime = 0.0f;
    int    drawCalls      = 0;
    uint32 batches        = 0;

    void DrawPass::PrepareRenderData(Vector<RenderableData>& drawList)
    {
        ExtractPassRenderables(drawList);
        BatchPassRenderables();
    }

    void DrawPass::ExtractPassRenderables(Vector<RenderableData>& drawList)
    {
        PROFILER_FUNC(PROFILER_THREAD_RENDER);

        m_renderables.clear();

        Taskflow tf;
        tf.for_each_index(0, static_cast<int>(drawList.size()), 1, [&](int i) {
            RenderableData& data = drawList[i];

            // Cull by distance
            if (m_view->GetPos().Distance(data.position) > m_drawDistance)
                return;

            // Cull by pass mask.
            if (!data.passMask.IsSet(m_passMask))
                return;

            // Cull by frustum
            if (m_view->IsVisible(data.aabb))
            {
                LOCK_GUARD(m_mtx);
                m_renderables.push_back(data);
            }
        });

        JobSystem::Get()->GetMainExecutor().RunAndWait(tf);
    }

    void DrawPass::BatchPassRenderables()
    {
        PROFILER_FUNC(PROFILER_THREAD_RENDER);

        m_batches.clear();

        const uint32 size = static_cast<uint32>(m_renderables.size());

        uint32 batchID = 0;
        for (uint32 i = 0; i < size; i++)
        {
            auto r = m_renderables[i];

            for (auto& p : r.meshMaterialPairs)
            {
                int32 index = FindInBatches(p);

                if (index != -1)
                {
                    InstancedBatch& batch = m_batches.at(index);
                    batch.renderableIndices.push_back(i);
                    batch.count++;
                    batch.meshes.push_back(p.mesh);
                }
                else
                {
                    InstancedBatch newBatch;
                    newBatch.mat = p.material;
                    newBatch.meshes.push_back(p.mesh);
                    newBatch.firstInstance = i;
                    newBatch.renderableIndices.push_back(i);
                    newBatch.count = 1;
                    m_batches.push_back(newBatch);
                }
            }
        }

        batches = static_cast<uint32>(m_batches.size());
    }

    void DrawPass::UpdateViewData(Buffer& viewDataBuffer)
    {
        GPUViewData data;
        data.view     = m_view->GetView();
        data.proj     = m_view->GetProj();
        data.viewProj = data.proj * data.view;
        viewDataBuffer.CopyInto(&data, sizeof(GPUViewData));
    }

    void DrawPass::RecordDrawCommands(CommandBuffer& cmd, RenderPassType rpType)
    {
        drawCalls = 0;

        PROFILER_FUNC(PROFILER_THREAD_RENDER);

        auto& renderer     = RenderEngine::Get()->GetRenderer();
        auto& mergedMeshes = renderer.GetMergedMeshEntries();

        Vector<VkDrawIndexedIndirectCommand> commands;
        uint32                               i = 0;
        for (auto b : m_batches)
        {
            uint32 i = 0;
            for (auto ri : b.renderableIndices)
            {
                auto&                        merged = mergedMeshes[b.meshes[i]];
                VkDrawIndexedIndirectCommand c;
                c.instanceCount = 1;
                c.indexCount    = merged.indexSize;
                c.vertexOffset  = merged.vertexOffset;
                c.firstIndex    = merged.firstIndex;
                c.firstInstance = m_renderables[ri].objDataIndex;
                commands.push_back(c);
                i++;
            }
        }

        renderer.GetIndirectBuffer().CopyInto(commands.data(), commands.size() * sizeof(VkDrawIndexedIndirectCommand));

        const uint32 batchesSize   = static_cast<uint32>(m_batches.size());
        uint32       firstInstance = 0;

        Material* lastBoundMat = nullptr;

        for (uint32 i = 0; i < batchesSize; i++)
        {
            InstancedBatch& batch = m_batches[i];
            Material*       mat   = batch.mat;

            if (mat != lastBoundMat)
            {
                mat->Bind(cmd, rpType, MaterialBindFlag::BindPipeline);
                lastBoundMat = mat;
            }
            mat->Bind(cmd, rpType, MaterialBindFlag::BindDescriptor);

            const uint64 indirectOffset = firstInstance * sizeof(VkDrawIndexedIndirectCommand);
            cmd.CMD_DrawIndexedIndirect(renderer.GetIndirectBuffer()._ptr, indirectOffset, batch.count, sizeof(VkDrawIndexedIndirectCommand));
            firstInstance += batch.count;
            // cmd.CMD_DrawIndexed(static_cast<uint32>(mesh->GetIndexSize()), batch.count, 0, 0, batch.firstInstance);
            drawCalls++;
        }

        if (Time::GetCPUTime() > lastReportTime + 1.0f)
        {
            lastReportTime = static_cast<float>(Time::GetCPUTime());
            LINA_TRACE("Draw calls {0} - batches {1}", drawCalls, batches);
        }
    }

    int32 DrawPass::FindInBatches(const MeshMaterialPair& pair)
    {
        const int32 size = static_cast<int32>(m_batches.size());

        for (int32 i = 0; i < size; i++)
        {
            const auto& b = m_batches.at(i);
            if (b.mat == pair.material)
                return i;
        }

        return -1;
    }

} // namespace Lina::Graphics
