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
	void ObjectRenderer::Initialize(LinaGX::Instance* lgx, EntityWorld* world, ResourceManagerV2* rm)
	{
		m_lgx	= lgx;
		m_world = world;
		m_world->AddListener(this);
		m_rm = rm;
		FetchRenderables();
	}

	void ObjectRenderer::Shutdown()
	{
		m_world->RemoveListener(this);
	}

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

	void ObjectRenderer::ProduceFrame(const View& view, Shader* shaderOverride, ShaderType type)
	{
		auto add = [&](Vector<PerShaderDraw>* perShaderDraws, Material* targetMaterial, const Matrix4& modelMatrix, Model* model, uint32 meshIndex, uint32 primitiveIndex) {
			Shader* shader = shaderOverride ? shaderOverride : m_rm->GetIfExists<Shader>(targetMaterial->GetShader());
			LINA_ASSERT(shader, "");

			if (type != shader->GetShaderType())
				return;

			const ResourceID   shaderID		= shader->GetID();
			const InstanceData instanceData = {
				.entity =
					{
						.model = modelMatrix,
					},
				.materialID = targetMaterial->GetID(),
				.boneIndex	= 0,
			};

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
				Vector<PrimitiveDraw>& primitives = foundPerShader->primitives;
				auto				   foundPrimitive =
					linatl::find_if(primitives.begin(), primitives.end(), [model, meshIndex, primitiveIndex](const PrimitiveDraw& draw) -> bool { return draw.modelID == model->GetID() && draw.meshIndex == meshIndex && draw.primitiveIndex == primitiveIndex; });

				if (foundPrimitive == primitives.end())
				{
					primitives.push_back({
						.modelID   = model->GetID(),
						.meshIndex = meshIndex,
						.instances = {instanceData},
					});
				}
				else
					foundPrimitive->instances.push_back(instanceData);
			}
		};

		for (CompModel* comp : m_compModels)
		{
			if (!comp->GetEntity()->GetVisible())
				continue;

			const Vector<CompModelNode>& nodes = comp->GetNodes();

			Entity* entity = comp->GetEntity();
			// TODO: view visibility check.

			Model* model = m_rm->GetIfExists<Model>(comp->GetModel());
			LINA_ASSERT(model, "");

			Vector<Material*>		  materials;
			const Vector<ResourceID>& materialIDs = comp->GetMaterials();
			const size_t			  materialsSz = materialIDs.size();

			materials.resize(materialsSz);
			for (size_t i = 0; i < materialsSz; i++)
				materials[i] = m_rm->GetIfExists<Material>(materialIDs[i]);

			const Vector<Mesh>& meshes	 = model->GetAllMeshes();
			const size_t		meshesSz = meshes.size();
			for (size_t i = 0; i < meshesSz; i++)
			{
				const Mesh&	 mesh	   = meshes[i];
				const uint32 meshIndex = static_cast<uint32>(i);

				const Vector<PrimitiveStatic>& primsStatic	 = mesh.primitivesStatic;
				const size_t				   primsStaticSz = primsStatic.size();
				for (size_t j = 0; j < primsStaticSz; j++)
				{
					const PrimitiveStatic& prim			  = primsStatic.at(j);
					const uint32		   primitiveIndex = static_cast<uint32>(j);

					Material* targetMaterial = prim.materialIndex >= materials.size() ? nullptr : materials[prim.materialIndex];
					LINA_ASSERT(targetMaterial, "");

					// PROFILER_ADD_VERTICESINDICES(prim.vertices.size(), prim.indices.size());
					// PROFILER_ADD_TRIS(prim.indices.size() / 3);
					// PROFILER_ADD_DRAWCALL(1);

					const Matrix4 modelMat = entity->GetTransform().GetMatrix() * nodes.at(mesh.nodeIndex).transform.GetLocalMatrix();
					add(&m_cpuDrawData.staticDraws, targetMaterial, modelMat, model, meshIndex, primitiveIndex);
				}

				const Vector<PrimitiveSkinned>& primsSkinned   = mesh.primitivesSkinned;
				const size_t					primsSkinnedSz = primsSkinned.size();
				for (size_t j = 0; j < primsSkinnedSz; j++)
				{
					const PrimitiveSkinned& prim		   = primsSkinned.at(j);
					const uint32			primitiveIndex = static_cast<uint32>(j);

					Material* targetMaterial = prim.materialIndex >= materials.size() ? nullptr : materials[prim.materialIndex];
					LINA_ASSERT(targetMaterial, "");

					const Matrix4 modelMat = entity->GetTransform().GetMatrix() * nodes.at(mesh.nodeIndex).transform.GetLocalMatrix();
					add(&m_cpuDrawData.skinnedDraws, targetMaterial, modelMat, model, meshIndex, primitiveIndex);
				}
			}
		}
	}

	void ObjectRenderer::RecordFrame(uint32 frameIndex, LinaGX::CommandStream* stream, RenderPass& pass)
	{
		Buffer& indirectBuffer	  = pass.GetBuffer(frameIndex, "IndirectBuffer"_hs);
		Buffer& indirectConstants = pass.GetBuffer(frameIndex, "IndirectConstants"_hs);
		Buffer& entityBuffer	  = pass.GetBuffer(frameIndex, "EntityBuffer"_hs);

		auto record = [&](uint32 indexCount, uint32 instanceCount, uint32 indexOffset, uint32 vertexOffset, const Vector<InstanceData>& instances) {
			const uint32 indirectCount = indirectBuffer.GetIndirectCount();
			const uint32 drawCount	   = indirectConstants.GetIndirectCount();

			m_lgx->BufferIndexedIndirectCommandData(indirectBuffer.GetMapped(), indirectCount * m_lgx->GetIndexedIndirectCommandSize(), drawCount, indexCount, instanceCount, indexOffset, vertexOffset, drawCount);

			indirectBuffer.SetIndirectCount(indirectCount + 1);

			for (const InstanceData& instance : instances)
			{
				Material* mat = m_rm->GetIfExists<Material>(instance.materialID);
				LINA_ASSERT(mat, "");

				const uint32 entityCount = entityBuffer.GetIndirectCount();
				entityBuffer.BufferData(sizeof(GPUEntity) * entityCount, (uint8*)&instance.entity, sizeof(GPUEntity));
				entityBuffer.SetIndirectCount(entityCount + 1);

				GPUIndirectConstants0 constants = {
					.entityIndex	   = entityCount,
					.materialByteIndex = mat->GetBindlessIndex() / static_cast<uint32>(sizeof(uint32)),
					.boneIndex		   = instance.boneIndex,
				};

				indirectConstants.BufferData(drawCount * sizeof(GPUIndirectConstants0), (uint8*)&constants, sizeof(GPUIndirectConstants0));
				indirectConstants.SetIndirectCount(drawCount + 1);
			}
		};

		const size_t staticDrawsSz = m_gpuDrawData.staticDraws.size();

		for (size_t i = 0; i < staticDrawsSz; i++)
		{
			const PerShaderDraw& draw = m_gpuDrawData.staticDraws.at(i);

			for (const PrimitiveDraw& primitiveDraw : draw.primitives)
			{
				Model* model = m_rm->GetIfExists<Model>(primitiveDraw.modelID);
				LINA_ASSERT(model, "");

				const Mesh&			   mesh				= model->GetAllMeshes().at(primitiveDraw.meshIndex);
				const PrimitiveStatic& primitiveSkinned = mesh.primitivesStatic.at(primitiveDraw.primitiveIndex);
				record(static_cast<uint32>(primitiveSkinned.indices.size()), static_cast<uint32>(primitiveDraw.instances.size()), primitiveSkinned._indexOffset, primitiveSkinned._vertexOffset, primitiveDraw.instances);
			}
		}

		const size_t skinnedDrawsSz = m_gpuDrawData.skinnedDraws.size();
		for (size_t i = 0; i < skinnedDrawsSz; i++)
		{
			const PerShaderDraw& draw = m_gpuDrawData.skinnedDraws.at(i);

			for (const PrimitiveDraw& primitiveDraw : draw.primitives)
			{
				Model* model = m_rm->GetIfExists<Model>(primitiveDraw.modelID);
				LINA_ASSERT(model, "");

				const Mesh&				mesh			 = model->GetAllMeshes().at(primitiveDraw.meshIndex);
				const PrimitiveSkinned& primitiveSkinned = mesh.primitivesSkinned.at(primitiveDraw.primitiveIndex);
				record(static_cast<uint32>(primitiveSkinned.indices.size()), static_cast<uint32>(primitiveDraw.instances.size()), primitiveSkinned._indexOffset, primitiveSkinned._vertexOffset, primitiveDraw.instances);
			}
		}
	}

	void ObjectRenderer::RenderStatic(uint32 frameIndex, LinaGX::CommandStream* stream, RenderPass& pass)
	{
		Buffer& indirectBuffer = pass.GetBuffer(frameIndex, "IndirectBuffer"_hs);

		const size_t sz				 = m_gpuDrawData.staticDraws.size();
		Shader*		 lastBoundShader = nullptr;

		for (size_t i = 0; i < sz; i++)
		{
			const PerShaderDraw& perShader = m_gpuDrawData.staticDraws.at(i);
			Shader*				 shader	   = m_rm->GetIfExists<Shader>(perShader.shader);
			LINA_ASSERT(shader, "");

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

	void ObjectRenderer::RenderSkinned(uint32 frameIndex, LinaGX::CommandStream* stream, RenderPass& pass)
	{
		Buffer& indirectBuffer = pass.GetBuffer(frameIndex, "IndirectBuffer"_hs);

		const size_t sz				 = m_gpuDrawData.skinnedDraws.size();
		Shader*		 lastBoundShader = nullptr;

		for (size_t i = 0; i < sz; i++)
		{
			const PerShaderDraw& perShader = m_gpuDrawData.skinnedDraws.at(i);
			Shader*				 shader	   = m_rm->GetIfExists<Shader>(perShader.shader);
			LINA_ASSERT(shader, "");

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
		m_gpuDrawData = m_cpuDrawData;
		m_cpuDrawData = {};
	}

} // namespace Lina
