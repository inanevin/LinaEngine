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
#include "Graphics/Interfaces/IGfxResourceGPU.hpp"
#include "Graphics/Interfaces/IGfxResourceCPU.hpp"
#include "Graphics/Interfaces/IGfxContext.hpp"
#include "Graphics/Components/RenderableComponent.hpp"
#include "World/Core/Entity.hpp"
#include "Graphics/Resource/Mesh.hpp"
#include "Graphics/Resource/Material.hpp"
#include "Graphics/Resource/ModelNode.hpp"
#include "Graphics/Core/View.hpp"
#include "Graphics/Core/GfxManager.hpp"
#include "System/ISystem.hpp"
#include "Graphics/Platform/RendererIncl.hpp"
#include "Profiling/Profiler.hpp"

namespace Lina
{
	DrawPass::DrawPass(GfxManager* gfxMan, IUploadContext* context) : m_gfxManager(gfxMan), m_uploadContext(context)
	{
		m_renderer		  = m_gfxManager->GetRenderer();
		m_contextGraphics = m_renderer->GetContextGraphics();
		m_contextTransfer = m_renderer->GetContextTransfer();

		for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& frame					   = m_frameData[i];
			frame.objDataBuffer			   = m_renderer->CreateGPUResource(sizeof(GPUObjectData) * 500, GPUResourceType::GPUOnlyWithStaging, false, L"Drawpass Object Data Buffer");
			frame.indirectBuffer		   = m_renderer->CreateCPUResource(sizeof(DrawIndexedIndirectCommand), CPUResourceHint::IndirectBuffer, L"Drawpass Indirect Buffer");
			frame.cmdListAllocatorTransfer = m_contextTransfer->CreateCommandAllocator();
			frame.cmdListTransfer		   = m_contextTransfer->CreateCommandList(frame.cmdListAllocatorTransfer);
		}

		m_transferFence		 = m_renderer->CreateFence();
		m_transferFenceValue = 1;
	}

	DrawPass::~DrawPass()
	{
		m_renderer->ReleaseFence(m_transferFence);

		for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& frame = m_frameData[i];
			m_renderer->DeleteGPUResource(frame.objDataBuffer);
			m_renderer->DeleteCPUResource(frame.indirectBuffer);
			m_contextTransfer->ReleaseCommandList(frame.cmdListTransfer);
			m_contextTransfer->ReleaseCommandAllocator(frame.cmdListAllocatorTransfer);
		}
	}

	void DrawPass::Process(Vector<RenderableData>& drawList, const View& targetView, float drawDistance, DrawPassMask drawPassMask)
	{
		PROFILER_FUNCTION();

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
		PROFILER_FUNCTION();

		auto& frame = m_frameData[frameIndex];

		uint32 id = PROFILER_STARTBLOCK("Obj Data Buf");

		m_contextTransfer->ResetCommandListAndAllocator(frame.cmdListAllocatorTransfer, frame.cmdListTransfer);

		// Update object data buffer.
		{
			Vector<GPUObjectData> objData;
			const size_t		  sz = static_cast<size_t>(m_renderables.size());
			objData.resize(sz);

			for (size_t i = 0; i < sz; i++)
			{
				GPUObjectData od;
				od.modelMatrix = m_renderables[i].modelMatrix;
				objData[i]	   = od;
			}

			frame.objDataBuffer->BufferData(objData.data(), sizeof(GPUObjectData) * sz, 0);
			frame.objDataBuffer->CopyImmediately(frame.cmdListTransfer, m_contextTransfer);
			m_contextTransfer->ExecuteCommandLists({frame.cmdListTransfer});
			m_contextTransfer->Signal(m_transferFence, m_transferFenceValue);
			m_contextGraphics->Wait(m_transferFence, m_transferFenceValue);
			m_transferFenceValue++;
			m_contextGraphics->BindObjectBuffer(cmdListHandle, frame.objDataBuffer);
		}

		PROFILER_ENDBLOCK(id);

		const auto& mergedMeshes = m_gfxManager->GetMeshManager().GetMergedMeshes();

		// Update indirect commands.
		{
			id = PROFILER_STARTBLOCK("Bind mats");

			Vector<DrawIndexedIndirectCommand> commands;

			// Bind materials.
			{
				const uint32	  matsSize = static_cast<uint32>(m_batches.size());
				Vector<Material*> materials;
				materials.resize(m_batches.size());

				for (uint32 i = 0; i < matsSize; i++)
					materials[i] = m_batches[i].mat;

				m_contextGraphics->BindMaterials(materials.data(), matsSize);
			}

			PROFILER_ENDBLOCK(id);
			id = PROFILER_STARTBLOCK("batches and indirect");

			for (auto b : m_batches)
			{
				uint32 i = 0;
				for (auto ri : b.renderableIndices)
				{
					auto&					   merged = mergedMeshes.at(b.meshes[i]);
					DrawIndexedIndirectCommand c;
					c.instanceID			= ri;
					c.materialID			= static_cast<uint32>(b.mat->GetGPUBindlessIndex());
					c.instanceCount			= 1;
					c.indexCountPerInstance = merged.indexSize;
					c.baseVertexLocation	= merged.vertexOffset;
					c.startIndexLocation	= merged.firstIndex;
					c.startInstanceLocation = 0;
					commands.push_back(c);
					i++;
				}
			}

			m_frameData[frameIndex].indirectBuffer->BufferData(commands.data(), sizeof(DrawIndexedIndirectCommand) * commands.size());

			PROFILER_ENDBLOCK(id);
		}
	}

	void DrawPass::Draw(uint32 frameIndex, uint32 cmdListHandle)
	{
		PROFILER_FUNCTION();

		const uint32 batchesSize	 = static_cast<uint32>(m_batches.size());
		uint32		 firstInstance	 = 0;
		Shader*		 lastBoundShader = nullptr;
		auto&		 frame			 = m_frameData[frameIndex];

		for (uint32 i = 0; i < batchesSize; i++)
		{
			InstancedBatch& batch	  = m_batches[i];
			Material*		mat		  = batch.mat;
			Shader*			matShader = mat->GetShader();

			if (matShader != lastBoundShader)
			{
				lastBoundShader = matShader;
				m_contextGraphics->BindPipeline(cmdListHandle, matShader);
			}

			const uint64 indirectOffset = firstInstance * sizeof(DrawIndexedIndirectCommand);
			m_contextGraphics->DrawIndexedIndirect(cmdListHandle, frame.indirectBuffer, batch.count, indirectOffset);
			firstInstance += batch.count;
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
