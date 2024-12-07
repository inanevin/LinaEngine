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

#include "Core/Graphics/Renderers/ObjectRenderer.hpp"
#include "Core/World/Components/CompModel.hpp"
#include "Core/World/EntityWorld.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Graphics/Resource/Shader.hpp"
#include "Core/Graphics/Data/ModelNode.hpp"
#include "Core/Graphics/Pipeline/RenderPass.hpp"
#include "Core/Graphics/Data/Mesh.hpp"
#include "Common/Profiling/Profiler.hpp"

namespace Lina
{

	void ObjectRenderer::OnComponentAdded(Component* comp)
	{
		if (comp->GetTID() == GetTypeID<CompModel>())
			m_compModels.push_back(static_cast<CompModel*>(comp));
	}

	void ObjectRenderer::OnComponentRemoved(Component* comp)
	{
		auto it = linatl::find_if(m_compModels.begin(), m_compModels.end(), [comp](CompModel* c) -> bool { return c == comp; });
		if (it != m_compModels.end())
			m_compModels.erase(it);
	}

	void ObjectRenderer::FetchRenderables()
	{
		m_compModels.resize(0);
		m_world->GetCache<CompModel>()->View([this](CompModel* model, uint32 idex) {
			m_compModels.push_back(model);
			return false;
		});
	}

	void ObjectRenderer::ProduceFrame(const Camera& mainCamera, float delta)
	{
		for (CompModel* comp : m_compModels)
		{
			if (!comp->GetEntity()->GetVisible())
				continue;

			Entity* entity = comp->GetEntity();

			// TODO: frustum cull :)

			Model* model = m_rm->GetIfExists<Model>(comp->GetModel());
			if (model == nullptr)
				continue;

			Vector<Material*>		  materials;
			const Vector<ResourceID>& materialIDs = comp->GetMaterials();
			const size_t			  materialsSz = materialIDs.size();

			materials.resize(materialsSz);
			for (size_t i = 0; i < materialsSz; i++)
				materials[i] = m_rm->GetIfExists<Material>(materialIDs[i]);

			const Vector<MeshDefault*>& meshes	 = model->GetMeshes();
			const size_t				meshesSz = meshes.size();
			for (size_t i = 0; i < meshesSz; i++)
			{
				MeshDefault* mesh	   = meshes[i];
				const uint32 meshIndex = static_cast<uint32>(i);

				const Vector<PrimitiveDefault>& prims	= mesh->GetPrimitives();
				const size_t					primsSz = prims.size();
				for (size_t j = 0; j < primsSz; j++)
				{

					const PrimitiveDefault& prim		   = prims[j];
					const uint32			primitiveIndex = static_cast<uint32>(j);

					Material* targetMaterial = prim.GetMaterialIndex() >= materials.size() ? nullptr : materials[prim.GetMaterialIndex()];
					if (targetMaterial == nullptr)
						continue;

					if (targetMaterial->GetShaderType() != ShaderType::OpaqueSurface && targetMaterial->GetShaderType() != ShaderType::TransparentSurface)
						continue;

					Shader* shader = m_rm->GetIfExists<Shader>(targetMaterial->GetShader());
					if (shader == nullptr)
						continue;
					const ResourceID shaderID = shader->GetID();

					const InstanceData instanceData = {
						.materialID = targetMaterial->GetID(),
						.entity =
							{
								.model = entity->GetTransform().GetMatrix() * mesh->GetNode()->GetLocalMatrix(),
							},
					};

					Vector<PerShaderDraw>* perShaderDraws = targetMaterial->GetShaderType() == ShaderType::OpaqueSurface ? &m_cpuDeferredDraws : &m_cpuForwardDraws;

					PROFILER_ADD_VERTICESINDICES(prim.GetVertexCount(), prim.GetIndexCount());
					PROFILER_ADD_TRIS(prim.GetIndexCount() / 3);
					PROFILER_ADD_DRAWCALL(1);

					auto foundPerShader = linatl::find_if(perShaderDraws->begin(), perShaderDraws->end(), [shaderID](const PerShaderDraw& draw) -> bool { return draw.shader == shaderID; });

					if (foundPerShader == perShaderDraws->end())
					{
						PerShaderDraw draw = {
							.shader = shaderID,
							.primitives =
								{
									{
										.modelID		= model->GetID(),
										.meshIndex		= meshIndex,
										.primitiveIndex = primitiveIndex,
										.instances		= {instanceData},
									},
								},
						};

						perShaderDraws->push_back(draw);
					}
					else
					{
						Vector<PrimitiveDraw>& primitives	  = foundPerShader->primitives;
						auto				   foundPrimitive = linatl::find_if(
							  primitives.begin(), primitives.end(), [model, meshIndex, primitiveIndex](const PrimitiveDraw& draw) -> bool { return draw.modelID == model->GetID() && draw.meshIndex == meshIndex && draw.primitiveIndex == primitiveIndex; });

						if (foundPrimitive == primitives.end())
						{
							primitives.push_back({
								.modelID   = model->GetID(),
								.meshIndex = static_cast<uint32>(i),
								.instances = {instanceData},
							});
						}
						else
							foundPrimitive->instances.push_back(instanceData);
					}
				}
			}
		}
	}

	void ObjectRenderer::RenderRecordPass(uint32 frameIndex, RenderPass& pass, RenderPassType type)
	{
		if (type != RenderPassType::Deferred && type != RenderPassType::Forward)
			return;

		Vector<PerShaderDraw>* data = type == RenderPassType::Deferred ? &m_renderDeferredDraws : &m_renderForwardDraws;
		const size_t		   sz	= data->size();

		Buffer& indirectBuffer	  = pass.GetBuffer(frameIndex, "IndirectBuffer"_hs);
		Buffer& indirectConstants = pass.GetBuffer(frameIndex, "IndirectConstants"_hs);
		Buffer& entityBuffer	  = pass.GetBuffer(frameIndex, "EntityBuffer"_hs);

		for (size_t i = 0; i < sz; i++)
		{
			PerShaderDraw& perShader = data->at(i);

			for (const PrimitiveDraw& primitiveDraw : perShader.primitives)
			{
				const uint32 indirectCount = indirectBuffer.GetIndirectCount();
				const uint32 drawCount	   = indirectConstants.GetIndirectCount();

				Model* model = m_rm->GetIfExists<Model>(primitiveDraw.modelID);
				if (model == nullptr)
					continue;

				MeshDefault*			mesh	  = model->GetMesh(primitiveDraw.meshIndex);
				const PrimitiveDefault& primitive = mesh->GetPrimitives().at(primitiveDraw.primitiveIndex);

				m_lgx->BufferIndexedIndirectCommandData(indirectBuffer.GetMapped(),
														indirectCount * m_lgx->GetIndexedIndirectCommandSize(),
														drawCount,
														primitive.GetIndexCount(),
														static_cast<uint32>(primitiveDraw.instances.size()),
														mesh->GetIndexOffset() + primitive.GetStartIndex(),
														mesh->GetVertexOffset() + primitive.GetStartVertex(),
														drawCount);

				indirectBuffer.SetIndirectCount(indirectCount + 1);

				for (const InstanceData& instance : primitiveDraw.instances)
				{
					Material* mat = m_rm->GetIfExists<Material>(instance.materialID);
					if (mat == nullptr)
						continue;

					const uint32 entityCount = entityBuffer.GetIndirectCount();
					entityBuffer.BufferData(sizeof(GPUEntity) * entityCount, (uint8*)&instance.entity, sizeof(GPUEntity));
					entityBuffer.SetIndirectCount(entityCount + 1);

					GPUIndirectConstants0 constants = {
						.entityIndex	   = entityCount,
						.materialByteIndex = mat->GetBindlessIndex() / static_cast<uint32>(sizeof(uint32)),
					};

					indirectConstants.BufferData(drawCount * sizeof(GPUIndirectConstants0), (uint8*)&constants, sizeof(GPUIndirectConstants0));
					indirectConstants.SetIndirectCount(drawCount + 1);
				}
			}
		}
	}

	void ObjectRenderer::RenderDrawPass(LinaGX::CommandStream* stream, uint32 frameIndex, RenderPass& pass, RenderPassType type)
	{
		if (type != RenderPassType::Deferred && type != RenderPassType::Forward)
			return;

		Buffer& indirectBuffer = pass.GetBuffer(frameIndex, "IndirectBuffer"_hs);

		Vector<PerShaderDraw>* data = type == RenderPassType::Deferred ? &m_renderDeferredDraws : &m_renderForwardDraws;

		const size_t sz				 = data->size();
		Shader*		 lastBoundShader = nullptr;

		for (size_t i = 0; i < sz; i++)
		{
			PerShaderDraw& perShader = data->at(i);
			Shader*		   shader	 = m_rm->GetIfExists<Shader>(perShader.shader);
			if (shader == nullptr)
				continue;

			if (type == RenderPassType::Deferred && shader->GetShaderType() != ShaderType::OpaqueSurface)
				continue;

			if (type == RenderPassType::Forward && shader->GetShaderType() != ShaderType::TransparentSurface)
				continue;

			if (shader != lastBoundShader)
			{
				LinaGX::CMDBindPipeline* pipelineBind = stream->AddCommand<LinaGX::CMDBindPipeline>();
				pipelineBind->shader				  = shader->GetGPUHandle();
				lastBoundShader						  = shader;
			}

			LinaGX::CMDDrawIndexedIndirect* draw = stream->AddCommand<LinaGX::CMDDrawIndexedIndirect>();
			draw->count							 = static_cast<uint32>(perShader.primitives.size());
			draw->indirectBuffer				 = indirectBuffer.GetGPUResource();
			draw->indirectBufferOffset			 = indirectBuffer.GetIndirectCount() * static_cast<uint32>(m_lgx->GetIndexedIndirectCommandSize());
		}
	}

	void ObjectRenderer::SyncRender()
	{
		m_renderForwardDraws  = m_cpuForwardDraws;
		m_renderDeferredDraws = m_cpuDeferredDraws;

		m_cpuForwardDraws.resize(0);
		m_cpuDeferredDraws.resize(0);
	}

	void ObjectRenderer::DropRenderFrame()
	{
		m_cpuForwardDraws	  = {};
		m_cpuDeferredDraws	  = {};
		m_renderForwardDraws  = {};
		m_renderDeferredDraws = {};
	}

} // namespace Lina
