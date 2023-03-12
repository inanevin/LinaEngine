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

#include "Graphics/Core/DrawPass.hpp"
#include "Graphics/Components/RenderableComponent.hpp"
#include "World/Core/Entity.hpp"
#include "Graphics/Resource/Mesh.hpp"
#include "Graphics/Resource/Material.hpp"
#include "Graphics/Resource/ModelNode.hpp"
#include "Graphics/Core/View.hpp"
#include "Graphics/Core/GfxManager.hpp"
#include "System/ISystem.hpp"
#include "Graphics/Platform/RendererIncl.hpp"
#include "Graphics/Core/IGfxBufferResource.hpp"

namespace Lina
{
	DrawPass::DrawPass(GfxManager* gfxMan) : m_gfxManager(gfxMan)
	{
		m_renderer = m_gfxManager->GetRenderer();

		for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			m_objDataBufferStaging[i] = m_renderer->CreateBufferResource(BufferResourceType::ObjectDataBufferStaging, nullptr, sizeof(GPUObjectData));
			m_objDataBufferGPU[i]	  = m_renderer->CreateBufferResource(BufferResourceType::ObjectDataBufferGPU, nullptr, sizeof(GPUObjectData));
			m_indirectBuffer[i]		  = m_renderer->CreateBufferResource(BufferResourceType::IndirectBuffer, nullptr, sizeof(DrawIndexedIndirectCommand));
		}
	}

	DrawPass::~DrawPass()
	{
		for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			m_renderer->DeleteBufferResource(m_objDataBufferStaging[i]);
			m_renderer->DeleteBufferResource(m_objDataBufferGPU[i]);
			m_renderer->DeleteBufferResource(m_indirectBuffer[i]);
		}
	}

	void DrawPass::Process(Vector<RenderableData>& drawList, const View& targetView, float drawDistance, DrawPassMask drawPassMask)
	{
		// Cull the draw list & finalize renderables.
		{
			m_renderables.clear();

			Taskflow tf;
			tf.for_each_index(0, static_cast<int>(drawList.size()), 1, [&](int i) {
				const RenderableData& data = drawList[i];

				// Cull by distance
				if (targetView.GetPos().Distance(data.position) > drawDistance)
					return;

				// Cull by pass mask.
				if (!data.passMask.IsSet(drawPassMask))
					return;

				// Cull by frustum
				if (targetView.IsVisible(data.aabb))
				{
					LOCK_GUARD(m_mtx);
					m_renderables.push_back(data);
				}
			});

			m_gfxManager->GetSystem()->GetMainExecutor()->RunAndWait(tf);
		}

		// Batch the renderables.
		{
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
						r.batchID = static_cast<uint32>(index);
					}
					else
					{
						InstancedBatch newBatch;
						newBatch.mat = p.material;
						newBatch.meshes.push_back(p.mesh);
						newBatch.firstInstance = i;
						newBatch.renderableIndices.push_back(i);
						newBatch.count = 1;
						r.batchID	   = static_cast<uint32>(m_batches.size());
						m_batches.push_back(newBatch);
					}
				}
			}
		}
	}

	void DrawPass::UpdateBuffers(uint32 frameIndex, uint32 cmdListHandle)
	{
		// Update object data buffer.
		{
			Vector<GPUObjectData> objData;
			const size_t		  sz = static_cast<size_t>(m_renderables.size());
			objData.resize(sz);

			Taskflow tf;
			tf.for_each_index(0, static_cast<int>(sz), 1, [&](int i) {
				GPUObjectData od;
				od.modelMatrix = m_renderables[i].modelMatrix;
				objData[i]	   = od;
			});

			auto* staging = m_objDataBufferStaging[frameIndex];
			auto* gpuBuf  = m_objDataBufferGPU[frameIndex];

			m_gfxManager->GetSystem()->GetMainExecutor()->RunAndWait(tf);
			staging->Update(objData.data(), sizeof(GPUObjectData) * sz);

			if (gpuBuf->GetSize() < staging->GetSize())
			{
				delete gpuBuf;
				m_objDataBufferGPU[frameIndex] = m_renderer->CreateBufferResource(BufferResourceType::ObjectDataBufferGPU, nullptr, staging->GetSize());
			}

			m_renderer->CopyFromStaging(cmdListHandle, m_objDataBufferStaging[frameIndex], m_objDataBufferGPU[frameIndex], ResourceState::NonPixelShaderResource);
			m_renderer->BindObjectBuffer(cmdListHandle, m_objDataBufferGPU[frameIndex]);
		}

		const auto& mergedMeshes = m_gfxManager->GetMeshManager().GetMergedMeshes();

		// Update indirect commands.
		{
			Vector<DrawIndexedIndirectCommand> commands;
			uint32							   i = 0;
			for (auto b : m_batches)
			{
				uint32 i = 0;
				for (auto ri : b.renderableIndices)
				{
					auto&					   merged = mergedMeshes.at(b.meshes[i]);
					DrawIndexedIndirectCommand c;
					c.instanceID			= ri;
					c.instanceCount			= 1;
					c.indexCountPerInstance = merged.indexSize;
					c.baseVertexLocation	= merged.vertexOffset;
					c.startIndexLocation	= merged.firstIndex;
					c.startInstanceLocation = 0;
					commands.push_back(c);
					i++;
				}
			}

			m_indirectBuffer[frameIndex]->Update(commands.data(), sizeof(DrawIndexedIndirectCommand) * commands.size());
		}
	}

	void DrawPass::Draw(uint32 frameIndex, uint32 cmdListHandle)
	{
		const uint32 batchesSize   = static_cast<uint32>(m_batches.size());
		uint32		 firstInstance = 0;

		Shader* lastBoundShader = nullptr;

		for (uint32 i = 0; i < batchesSize; i++)
		{
			InstancedBatch& batch	  = m_batches[i];
			Material*		mat		  = batch.mat;
			Shader*			matShader = mat->GetShader();

			if (matShader != lastBoundShader)
			{
				m_renderer->BindMaterial(cmdListHandle, mat, MBF_BindShader);
				lastBoundShader = matShader;
			}

			m_renderer->BindMaterial(cmdListHandle, mat, MBF_BindMaterialProperties);

			const uint64 indirectOffset = firstInstance * sizeof(DrawIndexedIndirectCommand);
			m_renderer->DrawIndexedIndirect(cmdListHandle, m_indirectBuffer[frameIndex], batch.count, indirectOffset);

			// m_renderer->DrawIndexedInstanced(cmdListHandle, 36, 3, 0, 0, firstInstance);
			//  cmd.CMD_DrawIndexedIndirect(indirectBuffer._ptr, indirectOffset, batch.count, sizeof(VkDrawIndexedIndirectCommand));
			firstInstance += batch.count;
		}
	}

	// void DrawPass::UpdateViewData(Buffer& viewDataBuffer, const View& view)
	//{
	//	GPUViewData data;
	//	data.view	  = view.GetView();
	//	data.proj	  = view.GetProj();
	//	data.viewProj = data.proj * data.view;
	//	viewDataBuffer.CopyInto(&data, sizeof(GPUViewData));
	// }

	// void DrawPass::RecordDrawCommands(const CommandBuffer& cmd, const HashMap<Mesh*, MergedBufferMeshEntry>& mergedMeshes, Buffer& indirectBuffer)
	// {
	// 	drawCalls = 0;
	//
	// 	Vector<VkDrawIndexedIndirectCommand> commands;
	// 	uint32								 i = 0;
	// 	for (auto b : m_batches)
	// 	{
	// 		uint32 i = 0;
	// 		for (auto ri : b.renderableIndices)
	// 		{
	// 			auto&						 merged = mergedMeshes.at(b.meshes[i]);
	// 			VkDrawIndexedIndirectCommand c;
	// 			c.instanceCount = 1;
	// 			c.indexCount	= merged.indexSize;
	// 			c.vertexOffset	= merged.vertexOffset;
	// 			c.firstIndex	= merged.firstIndex;
	// 			c.firstInstance = m_renderables[ri].objDataIndex;
	// 			commands.push_back(c);
	// 			i++;
	// 		}
	// 	}
	//
	// 	indirectBuffer.CopyInto(commands.data(), commands.size() * sizeof(VkDrawIndexedIndirectCommand));
	//
	// 	const uint32 batchesSize   = static_cast<uint32>(m_batches.size());
	// 	uint32		 firstInstance = 0;
	//
	// 	Material* lastBoundMat = nullptr;
	//
	// 	for (uint32 i = 0; i < batchesSize; i++)
	// 	{
	// 		InstancedBatch& batch = m_batches[i];
	// 		Material*		mat	  = batch.mat;
	//
	// 		if (mat != lastBoundMat)
	// 		{
	// 			mat->Bind(cmd, MaterialBindFlag::BindPipeline);
	// 			lastBoundMat = mat;
	// 		}
	// 		mat->Bind(cmd, MaterialBindFlag::BindDescriptor);
	//
	// 		const uint64 indirectOffset = firstInstance * sizeof(VkDrawIndexedIndirectCommand);
	// 		cmd.CMD_DrawIndexedIndirect(indirectBuffer._ptr, indirectOffset, batch.count, sizeof(VkDrawIndexedIndirectCommand));
	// 		firstInstance += batch.count;
	// 		// cmd.CMD_DrawIndexed(static_cast<uint32>(mesh->GetIndexSize()), batch.count, 0, 0, batch.firstInstance);
	// 		drawCalls++;
	// 	}
	//
	// 	if (Time::GetCPUTime() > lastReportTime + 1.0f)
	// 	{
	// 		lastReportTime = static_cast<float>(Time::GetCPUTime());
	// 		// LINA_TRACE("Draw calls {0} - batches {1}", drawCalls, batches);
	// 	}
	// }

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
