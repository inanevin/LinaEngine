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
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Graphics/Data/Mesh.hpp"
#include "Core/Graphics/GfxContext.hpp"
#include "Core/Graphics/Renderers/WorldRenderer.hpp"
#include "Core/Graphics/Pipeline/RenderPass.hpp"
#include "Core/World/Components/CompModel.hpp"

namespace Lina
{
	void DrawCollector::CollectCompModels(const Vector<CompModel*>& components, RenderPass& pass, ResourceManagerV2* rm, WorldRenderer* renderer, GfxContext* gfxContext, const CollectionFilter& filter)
	{
		struct Instance
		{
			CompModel* comp			 = nullptr;
			int32	   nodeIndex	 = 0;
			uint32	   materialIndex = 0;
			uint32	   boneIndex	 = 0;
		};
		struct Call
		{
			bool			 skinned	  = false;
			uint32			 shaderHandle = 0;
			uint32			 baseVertex	  = 0;
			uint32			 baseIndex	  = 0;
			uint32			 indexCount	  = 0;
			Vector<Instance> instances;
			Vector3			 sortPosition = Vector3::Zero;
			bool			 sort		  = false;

			bool operator==(const Call& other) const
			{
				return skinned == other.skinned && shaderHandle == other.shaderHandle && baseVertex == other.baseVertex && baseIndex == other.baseIndex && indexCount == other.indexCount;
			}
		};

		Vector<Call> calls;

		for (CompModel* comp : components)
		{
			const Vector<CompModelNode>& nodes	= comp->GetNodes();
			Entity*						 entity = comp->GetEntity();

			if (!entity->GetVisible())
				continue;

			if (!pass.GetView().CalculateVisibility({}))
				continue;

			Model* model = rm->GetIfExists<Model>(comp->GetModel());

			if (!model)
				continue;

			Vector<Material*>		  materials;
			const Vector<ResourceID>& materialIDs = comp->GetMaterials();
			const size_t			  materialsSz = materialIDs.size();

			materials.resize(materialsSz);
			for (size_t i = 0; i < materialsSz; i++)
				materials[i] = rm->GetIfExists<Material>(materialIDs[i]);

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

					Material* targetMaterial = filter.useMaterialOverride ? rm->GetResource<Material>(filter.materialOverride) : prim.materialIndex >= materials.size() ? nullptr : materials[prim.materialIndex];

					if (!targetMaterial)
						continue;

					if (!filter.allowedShaderTypes.empty())
					{
						const ShaderType type = targetMaterial->GetShaderType();
						auto			 it	  = linatl::find_if(filter.allowedShaderTypes.begin(), filter.allowedShaderTypes.end(), [type](ShaderType other) -> bool { return type == other; });
						if (it == filter.allowedShaderTypes.end())
							continue;
					}

					Shader* shader = nullptr;

					if (filter.useShaderOverride)
						shader = rm->GetResource<Shader>(filter.shaderOverride);
					else
						shader = rm->GetResource<Shader>(targetMaterial->GetShader());

					if (targetMaterial->GetShaderType() != ShaderType::ForwardSurface && targetMaterial->GetShaderType() != ShaderType::DeferredSurface)
						continue;

					const uint32 shaderHandle = filter.useVariantOverride ? shader->GetGPUHandle(filter.staticVariantOverride) : shader->GetGPUHandle("Static"_hs);

					const Instance instance = {
						.comp		   = comp,
						.nodeIndex	   = mesh.nodeIndex,
						.materialIndex = targetMaterial->GetBindlessIndex(),
						.boneIndex	   = 0,
					};

					const Call call = {
						.skinned	  = false,
						.shaderHandle = shaderHandle,
						.baseVertex	  = prim._vertexOffset,
						.baseIndex	  = prim._indexOffset,
						.indexCount	  = static_cast<uint32>(prim.indices.size()),
						.instances	  = {instance},
						.sortPosition = entity->GetPosition(),
						.sort		  = filter.distanceSort,
					};

					auto it = linatl::find_if(calls.begin(), calls.end(), [call](const Call& other) -> bool { return other == call; });

					if (it != calls.end() && !filter.distanceSort)
						it->instances.push_back(instance);
					else
						calls.push_back(call);
				}

				const Vector<PrimitiveSkinned>& primsSkinned   = mesh.primitivesSkinned;
				const size_t					primsSkinnedSz = primsSkinned.size();
				for (size_t j = 0; j < primsSkinnedSz; j++)
				{
					const PrimitiveSkinned& prim = primsSkinned.at(j);

					Material* targetMaterial = filter.useMaterialOverride ? rm->GetResource<Material>(filter.materialOverride) : prim.materialIndex >= materials.size() ? nullptr : materials[prim.materialIndex];

					if (!targetMaterial)
						continue;

					if (!filter.allowedShaderTypes.empty())
					{
						const ShaderType type = targetMaterial->GetShaderType();
						auto			 it	  = linatl::find_if(filter.allowedShaderTypes.begin(), filter.allowedShaderTypes.end(), [type](ShaderType other) -> bool { return type == other; });
						if (it == filter.allowedShaderTypes.end())
							continue;
					}

					Shader* shader = nullptr;

					if (filter.useShaderOverride)
						shader = rm->GetResource<Shader>(filter.shaderOverride);
					else
						shader = rm->GetResource<Shader>(targetMaterial->GetShader());

					if (targetMaterial->GetShaderType() != ShaderType::ForwardSurface && targetMaterial->GetShaderType() != ShaderType::DeferredSurface)
						continue;

					const uint32 shaderHandle = filter.useVariantOverride ? shader->GetGPUHandle(filter.skinnedVariantOverride) : shader->GetGPUHandle("Skinned"_hs);

					const Instance instance = {
						.comp		   = comp,
						.nodeIndex	   = mesh.nodeIndex,
						.materialIndex = targetMaterial->GetBindlessIndex(),
						.boneIndex	   = renderer->GetBoneIndex(comp),
					};

					const Call call = {
						.skinned	  = true,
						.shaderHandle = shaderHandle,
						.baseVertex	  = prim._vertexOffset,
						.baseIndex	  = prim._indexOffset,
						.indexCount	  = static_cast<uint32>(prim.indices.size()),
						.instances	  = {instance},
						.sortPosition = entity->GetPosition(),
						.sort		  = filter.distanceSort,
					};

					auto it = linatl::find_if(calls.begin(), calls.end(), [call](const Call& other) -> bool { return other == call; });

					if (it != calls.end() && !filter.distanceSort)
						it->instances.push_back(instance);
					else
						calls.push_back(call);
				}
			}
		}

		for (const Call& call : calls)
		{
			const uint32 argsStart = renderer->GetArgumentCount();

			for (const Instance& inst : call.instances)
			{
				const CompModelNode& node	= inst.comp->GetNodes().at(inst.nodeIndex);
				const Matrix4		 model	= inst.comp->GetEntity()->GetTransform().ToMatrix() * node.transform.ToLocalMatrix();
				const Matrix4		 normal = model.GetNormalMatrix();
				Entity*				 e		= inst.comp->GetEntity();

				const GPUEntity entity = {
					.model	  = model,
					.normal	  = normal,
					.position = e->GetPosition(),
					.forward  = e->GetRotation().GetForward(),
				};

				const EntityIdent ident = {
					.entityGUID = inst.comp->GetEntity()->GetGUID(),
					.uniqueID2	= static_cast<uint32>(inst.nodeIndex),
				};

				const uint32 idx = renderer->PushEntity(entity, ident);

				const GPUDrawArguments args = {
					.constant0 = idx,
					.constant1 = inst.materialIndex,
					.constant2 = inst.boneIndex,
					.constant3 = 0,
				};

				renderer->PushArgument(args);
			}

			const RenderPass::InstancedDraw draw = {
				.vertexBuffers =
					{
						call.skinned ? &gfxContext->GetMeshManagerDefault().GetVtxBufferSkinned() : &gfxContext->GetMeshManagerDefault().GetVtxBufferStatic(),
						call.skinned ? &gfxContext->GetMeshManagerDefault().GetVtxBufferSkinned() : &gfxContext->GetMeshManagerDefault().GetVtxBufferStatic(),
					},
				.indexBuffers =
					{
						call.skinned ? &gfxContext->GetMeshManagerDefault().GetIdxBufferSkinned() : &gfxContext->GetMeshManagerDefault().GetIdxBufferStatic(),
						call.skinned ? &gfxContext->GetMeshManagerDefault().GetIdxBufferSkinned() : &gfxContext->GetMeshManagerDefault().GetIdxBufferStatic(),
					},
				.vertexSize	   = call.skinned ? sizeof(VertexSkinned) : sizeof(VertexStatic),
				.shaderHandle  = call.shaderHandle,
				.baseVertex	   = call.baseVertex,
				.baseIndex	   = call.baseIndex,
				.indexCount	   = call.indexCount,
				.instanceCount = static_cast<uint32>(call.instances.size()),
				.pushConstant  = argsStart,
				.sortOrder	   = call.sort,
				.sortPosition  = call.sortPosition,
			};

			pass.AddDrawCall(draw);
		}
	}

} // namespace Lina
