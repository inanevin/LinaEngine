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

	void ObjectRenderer::ProduceFrame(const Camera& mainCamera, ResourceManagerV2* rm, float delta)
	{
		for (CompModel* comp : m_compModels)
		{
			if (!comp->GetEntity()->GetVisible())
				continue;

			Entity* entity = comp->GetEntity();

			// TODO: frustum cull :)

			Model* model = rm->GetIfExists<Model>(comp->GetModel());
			if (model == nullptr)
				continue;

			Vector<Material*>		  materials;
			const Vector<ResourceID>& materialIDs = comp->GetMaterials();
			const size_t			  materialsSz = materialIDs.size();

			materials.resize(materialsSz);
			for (size_t i = 0; i < materialsSz; i++)
				materials[i] = rm->GetIfExists<Material>(materialIDs[i]);

			const Vector<MeshDefault*>& meshes	 = model->GetMeshes();
			const size_t				meshesSz = meshes.size();
			for (size_t i = 0; i < meshesSz; i++)
			{
				Material* targetMaterial = materials[i];
				if (targetMaterial == nullptr)
					continue;

				if (targetMaterial->GetShaderType() != ShaderType::OpaqueSurface && targetMaterial->GetShaderType() != ShaderType::TransparentSurface)
					continue;

				Shader* shader = rm->GetIfExists<Shader>(targetMaterial->GetShader());
				if (shader == nullptr)
					continue;

				const ResourceID shaderID = shader->GetID();

				MeshDefault* mesh	   = meshes[i];
				const uint32 meshIndex = static_cast<uint32>(i);

				const PerMeshInstanceData instanceData = {
					.materialID = targetMaterial->GetID(),
					.entity =
						{
							.model = entity->GetTransform().GetMatrix() * mesh->GetNode()->GetLocalMatrix(),
						},
				};

				Vector<PerShaderDraw>* perShaderDraws = targetMaterial->GetShaderType() == ShaderType::OpaqueSurface ? &m_cpuDeferredDraws : &m_cpuForwardDraws;

				auto foundPerShader = linatl::find_if(perShaderDraws->begin(), perShaderDraws->end(), [shaderID](const PerShaderDraw& draw) -> bool { return draw.shader == shaderID; });

				if (foundPerShader == perShaderDraws->end())
				{
					PerShaderDraw draw = {
						.shader = shaderID,
						.meshes = {{
							.modelID   = model->GetID(),
							.meshIndex = meshIndex,
							.instances = {instanceData},
						}},
					};

					perShaderDraws->push_back(draw);
				}
				else
				{
					Vector<PerMeshDraw>& meshes	   = foundPerShader->meshes;
					auto				 foundMesh = linatl::find_if(meshes.begin(), meshes.end(), [model, meshIndex](const PerMeshDraw& meshDraw) -> bool { return meshDraw.modelID == model->GetID() && meshDraw.meshIndex == meshIndex; });

					if (foundMesh == meshes.end())
					{
						meshes.push_back({
							.modelID   = model->GetID(),
							.meshIndex = meshIndex,
							.instances = {instanceData},
						});
					}
					else
						foundMesh->instances.push_back(instanceData);
				}
			}
		}
	}

	void ObjectRenderer::RenderRecordIndirect(uint32 frameIndex, RenderPass& pass, RenderPassType type)
	{
		if (type != RenderPassType::Deferred && type != RenderPassType::Forward)
			return;

		Vector<PerShaderDraw>* data = type == RenderPassType::Deferred ? &m_renderDeferredDraws : &m_renderForwardDraws;
		const size_t		   sz	= data->size();

		Buffer& indirectBuffer	  = pass.GetBuffer(frameIndex, "IndirectBuffer"_hs);
		Buffer& indirectConstants = pass.GetBuffer(frameIndex, "IndirectConstants"_hs);

		for (size_t i = 0; i < sz; i++)
		{
			PerShaderDraw& perShader = data->at(i);

			for (PerMeshDraw& meshDraw : perShader.meshes)
			{
				const uint32 indirectCount = indirectBuffer.GetIndirectCount();
				const uint32 drawCount	   = indirectConstants.GetIndirectCount();

				Model* model = m_rm->GetIfExists<Model>(meshDraw.modelID);
				if (model == nullptr)
					continue;

				MeshDefault* mesh = model->GetMesh(meshDraw.meshIndex);

				m_lgx->BufferIndexedIndirectCommandData(
					indirectBuffer.GetMapped(), indirectCount * m_lgx->GetIndexedIndirectCommandSize(), drawCount, mesh->GetIndexCount(), static_cast<uint32>(meshDraw.instances.size()), mesh->GetIndexOffset(), mesh->GetVertexOffset(), drawCount);

				indirectBuffer.SetIndirectCount(indirectCount + 1);

				for (const PerMeshInstanceData& instance : meshDraw.instances)
				{
					Material* mat = m_rm->GetIfExists<Material>(instance.materialID);
					if (mat == nullptr)
						continue;

					GPUIndirectConstants0 constants = {
						.entity			   = instance.entity,
						.materialByteIndex = mat->GetBindlessIndex() / sizeof(uint32),
					};

					LINA_TRACE("RECORDING OBJECT DRAW MATERIAL BYTE INDEX IS {0}", constants.materialByteIndex);

					indirectConstants.BufferData(drawCount * sizeof(GPUIndirectConstants0), (uint8*)&constants, sizeof(GPUIndirectConstants0));
					indirectConstants.SetIndirectCount(drawCount + 1);
				}
			}
		}
	}

	void ObjectRenderer::RenderDrawIndirect(LinaGX::CommandStream* stream, uint32 frameIndex, RenderPass& pass, RenderPassType type)
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

			if (shader != lastBoundShader)
			{
				LinaGX::CMDBindPipeline* pipelineBind = stream->AddCommand<LinaGX::CMDBindPipeline>();
				pipelineBind->shader				  = shader->GetGPUHandle();
				lastBoundShader						  = shader;
			}

			LinaGX::CMDDrawIndexedIndirect* draw = stream->AddCommand<LinaGX::CMDDrawIndexedIndirect>();
			draw->count							 = static_cast<uint32>(perShader.meshes.size());
			draw->indirectBuffer				 = indirectBuffer.GetGPUResource();
			draw->indirectBufferOffset			 = indirectBuffer.GetIndirectCount() * static_cast<uint32>(m_lgx->GetIndexedIndirectCommandSize());

#ifdef LINA_DEBUG
			// uint32 totalTris = 0;
			// for (const auto& md : perShader.meshes)
			// 	totalTris += md.mesh.indexCount / 3;
			// PROFILER_ADD_DRAWCALL(totalTris, "WorldRenderer", 0);
#endif
		}
	}

	void ObjectRenderer::SyncRender()
	{
		m_renderForwardDraws  = m_cpuForwardDraws;
		m_renderDeferredDraws = m_cpuDeferredDraws;

		m_cpuForwardDraws.resize(0);
		m_cpuDeferredDraws.resize(0);
	}

} // namespace Lina
