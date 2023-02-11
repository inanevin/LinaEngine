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

#include "Graphics/Platform/Vulkan/Core/DrawPass.hpp"
#include "Graphics/Platform/Vulkan/Core/GPUStorage.hpp"
#include "Graphics/Platform/Vulkan/Objects/Buffer.hpp"
#include "Graphics/Platform/Vulkan/Objects/CommandBuffer.hpp"
#include "Graphics/Platform/Vulkan/Objects/DescriptorSet.hpp"
#include "Graphics/Components/RenderableComponent.hpp"
#include "Graphics/Core/View.hpp"
#include "World/Core/Entity.hpp"
#include "Graphics/Resource/Mesh.hpp"
#include "Graphics/Resource/Material.hpp"
#include "Graphics/Resource/ModelNode.hpp"
#include "Profiling/Profiler.hpp"
#include "JobSystem/JobSystem.hpp"

#include <vulkan/vulkan.h>
#include "Math/Math.hpp"

namespace Lina
{
	void DrawPass::PrepareRenderData(Vector<RenderableData>& drawList, const View& view)
	{
		ExtractPassRenderables(drawList, view);
		BatchPassRenderables();
	}

	void DrawPass::ExtractPassRenderables(const Vector<RenderableData>& drawList, const View& view)
	{
		PROFILER_FUNC(PROFILER_THREAD_RENDER);

		m_renderables.clear();

		Taskflow tf;
		tf.for_each_index(0, static_cast<int>(drawList.size()), 1, [&](int i) {
			const RenderableData& data = drawList[i];

			// Cull by distance
			if (view.GetPos().Distance(data.position) > m_drawDistance)
				return;

			// Cull by pass mask.
			if (!data.passMask.IsSet(m_passMask))
				return;

			// Cull by frustum
			if (view.IsVisible(data.aabb))
			{
				LOCK_GUARD(m_mtx);
				m_renderables.push_back(data);
			}
		});

		m_exec->RunAndWait(tf);
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
	}

	void DrawPass::UpdateViewData(Buffer& viewDataBuffer, const View& view)
	{
		GPUViewData data;
		data.view		= view.GetView();
		data.proj		= view.GetProj();
		data.viewProj	= data.proj * data.view;
		data.camPos		= Vector4(0.0f, 0.2f, 0.0f, 0.0f);
		data.camNearFar = Vector2(view.GetNear(), view.GetFar());
		viewDataBuffer.CopyInto(&data, sizeof(GPUViewData));
	}

	void DrawPass::RecordDrawCommands(const CommandBuffer& cmd, const HashMap<Mesh*, MergedBufferMeshEntry>& mergedMeshes, Buffer& indirectBuffer, uint32 imageIndex)
	{
		Vector<VkDrawIndexedIndirectCommand> commands;
		uint32								 i = 0;
		for (auto b : m_batches)
		{
			uint32 i = 0;
			for (auto ri : b.renderableIndices)
			{
				auto&						 merged = mergedMeshes.at(b.meshes[i]);
				VkDrawIndexedIndirectCommand c;
				c.instanceCount = 1;
				c.indexCount	= merged.indexSize;
				c.vertexOffset	= merged.vertexOffset;
				c.firstIndex	= merged.firstIndex;
				c.firstInstance = m_renderables[ri].objDataIndex;
				commands.push_back(c);
				i++;
			}
		}

		indirectBuffer.CopyInto(commands.data(), commands.size() * sizeof(VkDrawIndexedIndirectCommand));

		const uint32 batchesSize   = static_cast<uint32>(m_batches.size());
		uint32		 firstInstance = 0;

		Material* lastBoundMat = nullptr;

		for (uint32 i = 0; i < batchesSize; i++)
		{
			InstancedBatch& batch = m_batches[i];
			Material*		mat	  = batch.mat;

			if (mat != lastBoundMat)
			{
				cmd.CMD_BindPipeline(m_gpuStorage->GetPipeline(mat), nullptr, MaterialBindFlag::BindPipeline);
				lastBoundMat = mat;
			}

			cmd.CMD_BindPipeline(m_gpuStorage->GetPipeline(mat), &m_gpuStorage->GetDescriptor(mat, imageIndex), MaterialBindFlag::BindDescriptor);
			// cmd.CMD_Draw(3, 1, 0, 0);

			const uint64 indirectOffset = firstInstance * sizeof(VkDrawIndexedIndirectCommand);
			cmd.CMD_DrawIndexedIndirect(indirectBuffer._ptr, indirectOffset, batch.count, sizeof(VkDrawIndexedIndirectCommand));
			firstInstance += batch.count;
			// cmd.CMD_DrawIndexed(static_cast<uint32>(mesh->GetIndexSize()), batch.count, 0, 0, batch.firstInstance);
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

} // namespace Lina
