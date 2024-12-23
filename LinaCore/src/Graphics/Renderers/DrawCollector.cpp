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

#include "Core/Graphics/Renderers/DrawCollector.hpp"
#include "Core/World/EntityWorld.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Graphics/Data/Mesh.hpp"
#include "Core/Graphics/GfxContext.hpp"
#include "Core/World/Components/CompModel.hpp"

namespace Lina
{
	void DrawCollector::Initialize(LinaGX::Instance* lgx, EntityWorld* world, ResourceManagerV2* rm, GfxContext* context, JobExecutor* executor)
	{
		m_jobExecutor = executor;
		m_lgx		  = lgx;
		m_world		  = world;
		m_rm		  = rm;
		m_gfxContext  = context;
	}

	void DrawCollector::Shutdown()
	{
	}

	void DrawCollector::OnComponentAdded(Component* comp)
	{
		if (comp->GetTID() == GetTypeID<CompModel>())
			m_compModels.push_back(static_cast<CompModel*>(comp));
	}

	void DrawCollector::OnComponentRemoved(Component* comp)
	{
		auto it = linatl::find_if(m_compModels.begin(), m_compModels.end(), [comp](CompModel* c) -> bool { return c == comp; });
		if (it != m_compModels.end())
			m_compModels.erase(it);
	}

	void DrawCollector::CollectCompModels(DrawGroup& group, const View& view, ShaderType shaderType)
	{
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
				const Mesh& mesh = meshes[i];

				const Vector<PrimitiveStatic>& primsStatic	 = mesh.primitivesStatic;
				const size_t				   primsStaticSz = primsStatic.size();
				for (size_t j = 0; j < primsStaticSz; j++)
				{
					const PrimitiveStatic& prim = primsStatic.at(j);

					Material* targetMaterial = prim.materialIndex >= materials.size() ? nullptr : materials[prim.materialIndex];
					LINA_ASSERT(targetMaterial, "");
					if (targetMaterial->GetShaderType() != shaderType)
						continue;
					Shader*			 targetShader = m_rm->GetResource<Shader>(targetMaterial->GetShader());
					const ResourceID materialID	  = targetMaterial->GetID();

					const ModelDrawInstance inst = {
						.compModel	= comp,
						.materialID = materialID,
					};

					group.modelDraws.push_back({
						.modelID		= model->GetID(),
						.shaderID		= targetShader->GetID(),
						.shaderVariant	= "Static"_hs,
						.meshIndex		= static_cast<uint32>(i),
						.primitiveIndex = static_cast<uint32>(j),
						.isSkinned		= false,
						.instance		= inst,
					});
				}

				const Vector<PrimitiveSkinned>& primsSkinned   = mesh.primitivesSkinned;
				const size_t					primsSkinnedSz = primsSkinned.size();
				for (size_t j = 0; j < primsSkinnedSz; j++)
				{
					const PrimitiveSkinned& prim = primsSkinned.at(j);

					Material* targetMaterial = prim.materialIndex >= materials.size() ? nullptr : materials[prim.materialIndex];
					LINA_ASSERT(targetMaterial, "");
					if (targetMaterial->GetShaderType() != shaderType)
						continue;
					Shader*			 targetShader = m_rm->GetResource<Shader>(targetMaterial->GetShader());
					const ResourceID materialID	  = targetMaterial->GetID();

					const ModelDrawInstance inst = {
						.compModel	= comp,
						.materialID = materialID,
					};

					group.modelDraws.push_back({
						.modelID		= model->GetID(),
						.shaderID		= targetShader->GetID(),
						.shaderVariant	= "Skinned"_hs,
						.meshIndex		= static_cast<uint32>(i),
						.primitiveIndex = static_cast<uint32>(j),
						.isSkinned		= true,
						.instance		= inst,
					});
				}
			}
		}
	}

	void DrawCollector::AddModelDraw(DrawGroup& group, ResourceID model, uint32 meshIndex, uint32 primitiveIndex, ResourceID shaderID, StringID shaderVariant, const ModelDrawInstance& inst)
	{
		group.modelDraws.push_back({
			.modelID		= model,
			.shaderID		= shaderID,
			.shaderVariant	= shaderVariant,
			.meshIndex		= meshIndex,
			.primitiveIndex = primitiveIndex,
			.instance		= inst,
		});
	}

	void DrawCollector::AddCustomDraw(DrawGroup& group, const CustomDrawInstance& inst, ResourceID shaderID, StringID shaderVariant, Buffer* vertexBuffer, Buffer* indexBuffer, size_t vertexSize, uint32 baseVertex, uint32 indexCount, uint32 startIndex)
	{
		group.customDraws.push_back({
			.vertexBuffer		   = vertexBuffer,
			.indexBuffer		   = indexBuffer,
			.vertexSize			   = vertexSize,
			.shaderID			   = shaderID,
			.shaderVariant		   = shaderVariant,
			.baseVertexLocation	   = baseVertex,
			.indexCountPerInstance = indexCount,
			.startIndexLocation	   = startIndex,
			.instance			   = inst,
		});
	}

	void DrawCollector::AddCustomDrawRaw(DrawGroup& group, const CustomDrawInstance& inst, ResourceID shaderID, StringID shaderVariant, uint32 baseVertex, uint32 vtxCount)
	{
		group.customRawDraws.push_back({
			.shaderID				= shaderID,
			.shaderVariant			= shaderVariant,
			.baseVertexLocation		= baseVertex,
			.vertexCountPerInstance = vtxCount,
			.instance				= inst,
		});
	}

	void DrawCollector::PrepareGPUData()
	{
		m_renderingData = {};

		Vector<CompModel*> skinnedModels;

		// Create a list of all comp models supporting skinning.
		for (const DrawGroup& group : m_cpuDraw.drawGroups)
		{
			for (const ModelDraw& modelDraw : group.modelDraws)
			{
				if (modelDraw.instance.compModel != nullptr)
				{
					const Vector<PrimitiveSkinned>& prims = modelDraw.instance.compModel->GetModelPtr()->GetAllMeshes().at(modelDraw.meshIndex).primitivesSkinned;
					if (!prims.empty())
						skinnedModels.push_back(modelDraw.instance.compModel);
				}
			}
		}

		// Now we'll have bones populated for the rendering data.
		if (!skinnedModels.empty())
			CalculateSkinning(skinnedModels);

		for (const DrawGroup& group : m_cpuDraw.drawGroups)
		{
			m_renderingData.groups.push_back({
				.id	  = group.id,
				.name = group.name,
			});
			RenderingGroup& renderingGroup = m_renderingData.groups.back();
			PrepareModelDraws(skinnedModels, group, renderingGroup);
			PrepareCustomDraws(group, renderingGroup);
			PrepareCustomDrawsRaw(group, renderingGroup);
		}
	}

	void DrawCollector::CreateGroup(const String& name)
	{
		m_cpuDraw.drawGroups.push_back({
			.id	  = TO_SID(name),
			.name = name,
		});
	}

	DrawCollector::DrawGroup& DrawCollector::GetGroup(StringID groupId)
	{
		auto it = linatl::find_if(m_cpuDraw.drawGroups.begin(), m_cpuDraw.drawGroups.end(), [groupId](const DrawGroup& g) -> bool { return groupId == g.id; });
		LINA_ASSERT(it != m_cpuDraw.drawGroups.end(), "");
		return *it;
	}

	DrawCollector::RenderingGroup& DrawCollector::GetRenderGroup(StringID groupId)
	{
		auto it = linatl::find_if(m_renderingData.groups.begin(), m_renderingData.groups.end(), [groupId](const RenderingGroup& g) -> bool { return groupId == g.id; });
		LINA_ASSERT(it != m_renderingData.groups.end(), "");
		return *it;
	}

	bool DrawCollector::GroupExists(StringID groupId) const
	{
		auto it = linatl::find_if(m_cpuDraw.drawGroups.begin(), m_cpuDraw.drawGroups.end(), [groupId](const DrawGroup& g) -> bool { return groupId == g.id; });
		return it != m_cpuDraw.drawGroups.end();
	}

	bool DrawCollector::RenderGroupExists(StringID groupId) const
	{
		auto it = linatl::find_if(m_renderingData.groups.begin(), m_renderingData.groups.end(), [groupId](const RenderingGroup& g) -> bool { return groupId == g.id; });
		return it != m_renderingData.groups.end();
	}

	void DrawCollector::CalculateSkinning(const Vector<CompModel*>& comps)
	{
		Vector<uint32> boneIDs;
		uint32		   boneIdxCounter = 0;
		for (CompModel* comp : comps)
		{
			const Vector<ModelSkin>& skins = comp->GetModelPtr()->GetAllSkins();
			boneIDs.push_back(boneIdxCounter);
			for (const ModelSkin& skin : skins)
				boneIdxCounter += static_cast<uint32>(skin.jointIndices.size());
		}

		m_renderingData.bones.resize(boneIdxCounter);

		Taskflow tf;
		tf.for_each_index(0, static_cast<int>(comps.size()), 1, [&](int i) {
			CompModel*				 comp  = comps.at(i);
			const Vector<ModelSkin>& skins = comp->GetModelPtr()->GetAllSkins();
			Vector<CompModelNode>&	 nodes = comp->GetNodes();

			uint32 boneID = boneIDs.at(i);

			for (const ModelSkin& skin : skins)
			{
				const Matrix4 rootGlobal		= skin.rootJointIndex != -1 ? comp->CalculateGlobalMatrix(skin.rootJointIndex) : comp->GetEntity()->GetTransform().ToMatrix();
				const Matrix4 rootGlobalInverse = rootGlobal.Inverse();

				uint32 idx = 0;
				for (uint32 jointIndex : skin.jointIndices)
				{
					const Matrix4 global				= comp->CalculateGlobalMatrix(jointIndex);
					m_renderingData.bones[boneID + idx] = rootGlobalInverse * global * nodes.at(jointIndex).inverseBindTransform;
					idx++;
				}
			}
		});
		m_jobExecutor->RunAndWait(tf);
	}

	void DrawCollector::SyncRender()
	{
		PrepareGPUData();
		m_cpuDraw = {};
	}

	void DrawCollector::RenderGroup(StringID groupId, LinaGX::CommandStream* stream)
	{
		auto it = linatl::find_if(m_renderingData.groups.begin(), m_renderingData.groups.end(), [groupId](const RenderingGroup& g) -> bool { return groupId == g.id; });
		LINA_ASSERT(it != m_renderingData.groups.end(), "");

		uint16	bound		   = 0;
		bool	isBound		   = false;
		Buffer* boundVtxBuffer = nullptr;

		auto checkPipeline = [&](uint32 handle) {
			if (!isBound || bound != handle)
			{
				LinaGX::CMDBindPipeline* pipeline = stream->AddCommand<LinaGX::CMDBindPipeline>();
				pipeline->shader				  = handle;
				isBound							  = true;
				bound							  = handle;
			}
		};

		auto checkBuffers = [&](Buffer* vtxBuffer, Buffer* idxBuffer, size_t vtxSize) {
			if (boundVtxBuffer == nullptr || boundVtxBuffer != vtxBuffer)
			{
				vtxBuffer->BindVertex(stream, static_cast<uint32>(vtxSize));
				idxBuffer->BindIndex(stream, LinaGX::IndexType::Uint16);
				boundVtxBuffer = vtxBuffer;
			}
		};

		for (const DrawCall& dc : it->drawCalls)
		{
			checkPipeline(dc.shaderHandle);

			bool indexedInstanced = false;

			if (dc.indexBuffer && dc.vertexBuffer)
			{
				checkBuffers(dc.vertexBuffer, dc.indexBuffer, dc.vertexSize);
				indexedInstanced = true;
			}

			LinaGX::CMDBindConstants* pushConstants = stream->AddCommand<LinaGX::CMDBindConstants>();
			pushConstants->data						= stream->EmplaceAuxMemory<uint32>(dc.pushConstant);
			pushConstants->size						= sizeof(uint32);
			pushConstants->stagesSize				= 1;
			pushConstants->stages					= stream->EmplaceAuxMemory<LinaGX::ShaderStage>(LinaGX::ShaderStage::Vertex);

			if (indexedInstanced)
			{
				LinaGX::CMDDrawIndexedInstanced* draw = stream->AddCommand<LinaGX::CMDDrawIndexedInstanced>();
				draw->baseVertexLocation			  = dc.baseVertexLocation;
				draw->indexCountPerInstance			  = dc.indexCountPerInstance;
				draw->instanceCount					  = dc.instanceCount;
				draw->startIndexLocation			  = dc.startIndexLocation;
				draw->startInstanceLocation			  = dc.startInstanceLocation;
			}
			else
			{

				LinaGX::CMDDrawInstanced* draw = stream->AddCommand<LinaGX::CMDDrawInstanced>();
				draw->instanceCount			   = dc.instanceCount;
				draw->startInstanceLocation	   = dc.startInstanceLocation;
				draw->startVertexLocation	   = dc.baseVertexLocation;
				draw->vertexCountPerInstance   = dc.vertexCountPerInstance;
			}
		}
	}

	bool DrawCollector::DrawEntityExists(uint32& outIndex, uint64 uid, uint32 uid2, uint32 uid3, uint32 uid4)
	{
		const uint32 entitiesSize = static_cast<uint32>(m_renderingData.entities.size());
		for (uint32 i = 0; i < entitiesSize; i++)
		{
			const DrawEntity& de = m_renderingData.entities.at(i);

			if (de.entityGUID == 0 && de.entityGUID == de.uniqueID2 == de.uniqueID3 == de.uniqueID4)
				continue;

			if (de.entityGUID == uid && de.uniqueID2 == uid2 && de.uniqueID3 == uid3 && de.uniqueID4 == uid4)
			{
				outIndex = i;
				return true;
			}
		}
		return false;
	}

	void DrawCollector::PrepareModelDraws(Vector<CompModel*>& skinnedModels, const DrawGroup& group, RenderingGroup& renderingGroup)
	{
		Vector<InstancedModelDraw> modelDraws;
		modelDraws.reserve(group.modelDraws.size());

		for (const ModelDraw& draw : group.modelDraws)
		{
			auto it = linatl::find_if(modelDraws.begin(), modelDraws.end(), [&draw](const InstancedModelDraw& instanced) -> bool { return instanced == draw; });

			if (it != modelDraws.end())
				it->instances.push_back(draw.instance);
			else
			{
				modelDraws.push_back({
					.modelID		= draw.modelID,
					.shaderID		= draw.shaderID,
					.shaderVariant	= draw.shaderVariant,
					.meshIndex		= draw.meshIndex,
					.primitiveIndex = draw.primitiveIndex,
					.instances		= {draw.instance},
				});
			}
		}

		for (const InstancedModelDraw& modelDraw : modelDraws)
		{
			Model*		model = m_rm->GetResource<Model>(modelDraw.modelID);
			const Mesh& mesh  = model->GetAllMeshes().at(modelDraw.meshIndex);

			const uint32 instanceDataStart = static_cast<uint32>(m_renderingData.instanceData.size());

			for (const ModelDrawInstance& inst : modelDraw.instances)
			{
				uint32 entityIndex	 = 0;
				uint32 materialIndex = inst.materialID != 0 ? (m_rm->GetResource<Material>(inst.materialID)->GetBindlessIndex() / static_cast<uint32>(sizeof(uint32))) : 0;
				uint32 boneIndex	 = 0;

				if (inst.compModel)
				{
					if (!DrawEntityExists(entityIndex, inst.compModel->GetEntity()->GetGUID(), modelDraw.meshIndex, modelDraw.primitiveIndex, static_cast<uint32>(mesh.nodeIndex)))
					{
						entityIndex = static_cast<uint32>(m_renderingData.entities.size());

						m_renderingData.entities.push_back({
							.entity =
								{
									.model = inst.compModel->GetEntity()->GetTransform().GetMatrix() * inst.compModel->GetNodes().at(mesh.nodeIndex).transform.ToLocalMatrix(),
								},
							.entityGUID = inst.compModel->GetEntity()->GetGUID(),
							.uniqueID2	= modelDraw.meshIndex,
							.uniqueID3	= modelDraw.primitiveIndex,
							.uniqueID4	= static_cast<uint32>(mesh.nodeIndex),
						});
					}

					bool found = false;

					for (CompModel* comp : skinnedModels)
					{
						if (comp == inst.compModel)
						{
							found = true;
							break;
						}

						const Vector<ModelSkin>& skins = comp->GetModelPtr()->GetAllSkins();

						for (const ModelSkin& skin : skins)
							boneIndex += static_cast<uint32>(skin.jointIndices.size());
					}

					if (!found)
						boneIndex = 0;
				}
				else
				{
					if (!DrawEntityExists(entityIndex, inst.customEntityGUID, 0, 0, 0))
					{
						entityIndex = static_cast<uint32>(m_renderingData.entities.size());
						m_renderingData.entities.push_back({
							.entity		= inst.customEntityData,
							.entityGUID = inst.customEntityGUID,
						});
					}
				}

				const GPUDrawArguments arguments = {
					.constant0 = entityIndex,
					.constant1 = materialIndex,
					.constant2 = boneIndex,
				};

				m_renderingData.instanceData.push_back(arguments);
			}

			Shader*		 shader		  = m_rm->GetResource<Shader>(modelDraw.shaderID);
			const uint32 shaderHandle = modelDraw.shaderVariant == 0 ? shader->GetGPUHandle() : shader->GetGPUHandle(modelDraw.shaderVariant);
			if (!mesh.primitivesStatic.empty())
			{
				const PrimitiveStatic& prim = mesh.primitivesStatic.at(modelDraw.primitiveIndex);

				const DrawCall drawCall = {
					.shaderHandle		   = shaderHandle,
					.vertexBuffer		   = &m_gfxContext->GetMeshManagerDefault().GetVtxBufferStatic(),
					.indexBuffer		   = &m_gfxContext->GetMeshManagerDefault().GetIdxBufferStatic(),
					.vertexSize			   = sizeof(VertexStatic),
					.pushConstant		   = instanceDataStart,
					.baseVertexLocation	   = prim._vertexOffset,
					.indexCountPerInstance = static_cast<uint32>(prim.indices.size()),
					.instanceCount		   = static_cast<uint32>(modelDraw.instances.size()),
					.startIndexLocation	   = prim._indexOffset,
					.startInstanceLocation = 0,
				};

				renderingGroup.drawCalls.push_back(drawCall);
			}
			else
			{
				const PrimitiveSkinned& prim = mesh.primitivesSkinned.at(modelDraw.primitiveIndex);

				const DrawCall drawCall = {
					.shaderHandle		   = shaderHandle,
					.vertexBuffer		   = &m_gfxContext->GetMeshManagerDefault().GetVtxBufferSkinned(),
					.indexBuffer		   = &m_gfxContext->GetMeshManagerDefault().GetIdxBufferSkinned(),
					.vertexSize			   = sizeof(VertexSkinned),
					.pushConstant		   = instanceDataStart,
					.baseVertexLocation	   = prim._vertexOffset,
					.indexCountPerInstance = static_cast<uint32>(prim.indices.size()),
					.instanceCount		   = static_cast<uint32>(modelDraw.instances.size()),
					.startIndexLocation	   = prim._indexOffset,
					.startInstanceLocation = 0,
				};

				renderingGroup.drawCalls.push_back(drawCall);
			}
		}
	}

	void DrawCollector::PrepareCustomDraws(const DrawGroup& group, RenderingGroup& renderingGroup)
	{
		Vector<InstancedCustomDraw> customDraws;
		customDraws.reserve(group.customDraws.size());

		for (const CustomDraw& customDraw : group.customDraws)
		{
			auto it = linatl::find_if(customDraws.begin(), customDraws.end(), [&customDraw](const InstancedCustomDraw& instanced) -> bool { return instanced == customDraw; });

			if (it != customDraws.end())
				it->instances.push_back(customDraw.instance);
			else
			{
				customDraws.push_back({
					.vertexBuffer		   = customDraw.vertexBuffer,
					.indexBuffer		   = customDraw.indexBuffer,
					.vertexSize			   = customDraw.vertexSize,
					.shaderID			   = customDraw.shaderID,
					.shaderVariant		   = customDraw.shaderVariant,
					.baseVertexLocation	   = customDraw.baseVertexLocation,
					.indexCountPerInstance = customDraw.indexCountPerInstance,
					.startIndexLocation	   = customDraw.startIndexLocation,
					.instances			   = {customDraw.instance},
				});
			}
		}

		for (const InstancedCustomDraw& customDraw : customDraws)
		{
			const uint32 baseInstance = static_cast<uint32>(m_renderingData.instanceData.size());

			for (const CustomDrawInstance& inst : customDraw.instances)
			{
				GPUDrawArguments args = inst.args;

				if (inst.useEntityAsFirstArgument)
				{
					uint32 entityIndex = 0;
					if (!DrawEntityExists(entityIndex, inst.entityGUID, 0, 0, 0))
					{
						entityIndex = static_cast<uint32>(m_renderingData.entities.size());
						m_renderingData.entities.push_back({
							.entity		= inst.entity,
							.entityGUID = inst.entityGUID,
						});
					}

					args.constant0 = entityIndex;
				}

				if (inst.useMaterialIDAsSecondArgument)
				{
					args.constant1 = m_rm->GetResource<Material>(inst.materialID)->GetBindlessIndex() / static_cast<uint32>(sizeof(uint32));
				}

				m_renderingData.instanceData.push_back(args);
			}

			Shader*		 shader		  = m_rm->GetResource<Shader>(customDraw.shaderID);
			const uint32 shaderHandle = customDraw.shaderVariant == 0 ? shader->GetGPUHandle() : shader->GetGPUHandle(customDraw.shaderVariant);

			const DrawCall drawCall = {
				.shaderHandle		   = shaderHandle,
				.vertexBuffer		   = customDraw.vertexBuffer,
				.indexBuffer		   = customDraw.indexBuffer,
				.vertexSize			   = customDraw.vertexSize,
				.pushConstant		   = baseInstance,
				.baseVertexLocation	   = customDraw.baseVertexLocation,
				.indexCountPerInstance = customDraw.indexCountPerInstance,
				.instanceCount		   = static_cast<uint32>(customDraw.instances.size()),
				.startIndexLocation	   = customDraw.startIndexLocation,
				.startInstanceLocation = 0,
			};

			renderingGroup.drawCalls.push_back(drawCall);
		}
	}

	void DrawCollector::PrepareCustomDrawsRaw(const DrawGroup& group, RenderingGroup& renderingGroup)
	{
		Vector<InstancedCustomDrawRaw> customRawDraws;

		for (const CustomDrawRaw& draw : group.customRawDraws)
		{
			auto it = linatl::find_if(customRawDraws.begin(), customRawDraws.end(), [&draw](const InstancedCustomDrawRaw& instanced) -> bool { return instanced == draw; });

			if (it != customRawDraws.end())
				it->instances.push_back(draw.instance);
			else
			{
				customRawDraws.push_back({
					.shaderID				= draw.shaderID,
					.shaderVariant			= draw.shaderVariant,
					.baseVertexLocation		= draw.baseVertexLocation,
					.vertexCountPerInstance = draw.vertexCountPerInstance,
					.instances				= {draw.instance},
				});
			}
		}

		for (const InstancedCustomDrawRaw& customDrawRaw : customRawDraws)
		{
			const uint32 baseInstance = static_cast<uint32>(m_renderingData.instanceData.size());

			for (const CustomDrawInstance& inst : customDrawRaw.instances)
			{
				GPUDrawArguments args = inst.args;

				if (inst.useEntityAsFirstArgument)
				{
					uint32 entityIndex = 0;
					if (!DrawEntityExists(entityIndex, inst.entityGUID, 0, 0, 0))
					{
						entityIndex = static_cast<uint32>(m_renderingData.entities.size());
						m_renderingData.entities.push_back({
							.entity		= inst.entity,
							.entityGUID = inst.entityGUID,
						});
					}

					args.constant0 = entityIndex;
				}

				if (inst.useMaterialIDAsSecondArgument)
				{
					args.constant1 = m_rm->GetResource<Material>(inst.materialID)->GetBindlessIndex() / static_cast<uint32>(sizeof(uint32));
				}

				m_renderingData.instanceData.push_back(args);
			}

			Shader*		 shader		  = m_rm->GetResource<Shader>(customDrawRaw.shaderID);
			const uint32 shaderHandle = customDrawRaw.shaderVariant == 0 ? shader->GetGPUHandle() : shader->GetGPUHandle(customDrawRaw.shaderVariant);

			const DrawCall drawCall = {
				.shaderHandle			= shaderHandle,
				.pushConstant			= baseInstance,
				.baseVertexLocation		= customDrawRaw.baseVertexLocation,
				.instanceCount			= static_cast<uint32>(customDrawRaw.instances.size()),
				.vertexCountPerInstance = customDrawRaw.vertexCountPerInstance,
				.startInstanceLocation	= 0,
			};

			renderingGroup.drawCalls.push_back(drawCall);
		}
	}

} // namespace Lina
