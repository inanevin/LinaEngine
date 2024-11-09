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
#include "Core/Graphics/Resource/Model.hpp"
#include "Core/Graphics/Resource/Material.hpp"
#include "Core/Graphics/Data/Mesh.hpp"
#include "Core/Graphics/BindlessContext.hpp"
#include "Core/World/EntityWorld.hpp"
#include "Core/Components/MeshComponent.hpp"
#include "Common/Profiling/Profiler.hpp"
#include "Common/Data/Streams.hpp"

namespace Lina
{

	void DrawCollector::Initialize(LinaGX::Instance* lgx, EntityWorld* world, BindlessContext* context)
	{
		m_lgx			  = lgx;
		m_world			  = world;
		m_bindlessContext = context;
	}

	void DrawCollector::Clear()
	{
		m_drawData.clear();
	}

	void DrawCollector::CollectStaticMeshes(const Vector<MeshComponent*>& meshes, ShaderType shaderType)
	{
		for (auto* mc : meshes)
		{
			if (mc->GetEntity()->GetFlags().IsSet(EF_INVISIBLE))
				continue;

			Model*		 model	  = m_world->GetResourceManagerV2().GetIfExists<Model>(mc->GetModel());
			MeshDefault* mesh	  = model->GetMesh(mc->GetMeshIndex());
			Material*	 material = m_world->GetResourceManagerV2().GetIfExists<Material>(mc->GetMaterial());

			LINA_ASSERT(model && material, "");

			if (material->GetShaderType() != shaderType)
				continue;

			const Vector3 pos = mc->GetEntity()->GetPosition();

			Shader* shader = m_world->GetResourceManagerV2().GetIfExists<Shader>(material->GetShader());

			LINA_ASSERT(shader, "");

			auto& vec = m_drawData[shader];

			auto it = linatl::find_if(vec.begin(), vec.end(), [mesh](const DrawDataMeshDefault& dd) -> bool { return dd.mesh == mesh; });

			// if (it != vec.end())
			// 	it->instances.push_back({material, m_bindlessContext->GetBindlessIndex(mc->GetEntity())});
			// else
			// {
			// 	DrawDataMeshDefault drawData = {
			// 		.mesh	   = mesh,
			// 		.instances = {{material, m_bindlessContext->GetBindlessIndex(mc->GetEntity())}},
			// 	};
			// 	vec.push_back(drawData);
			// }
		}
	}

	void DrawCollector::RecordIndirectCommands(Buffer& indirectBuffer, Buffer& indirectArgsBuffer)
	{
		// Indirect commands.
		uint32 constantsCount = 0;
		uint32 drawID		  = 0;
		uint32 indirectAmount = 0;

		for (const auto& [shader, meshVector] : m_drawData)
		{
			for (const auto& md : meshVector)
			{
				m_lgx->BufferIndexedIndirectCommandData(
					indirectBuffer.GetMapped(), indirectAmount * m_lgx->GetIndexedIndirectCommandSize(), drawID, md.mesh->GetIndexCount(), static_cast<uint32>(md.instances.size()), md.mesh->GetIndexOffset(), md.mesh->GetVertexOffset(), drawID);
				indirectBuffer.MarkDirty();

				indirectAmount++;
				for (const InstanceData& inst : md.instances)
				{
					// GPUIndirectConstants0 constants = {
					// 	.entityID		   = inst.entityIndex,
					// 	.materialByteIndex = m_bindlessContext->GetBindlessIndex(inst.material) / 4,
					// };
					// indirectArgsBuffer.BufferData(constantsCount * sizeof(GPUIndirectConstants0), (uint8*)&constants, sizeof(GPUIndirectConstants0));
					// constantsCount++;
					// drawID++;
				}
			}
		}
	}

	void DrawCollector::Draw(LinaGX::CommandStream* stream, Buffer& indirectBuffer)
	{
		Shader* lastBoundShader = nullptr;
		uint32	indirectOffset	= 0;

		for (auto& [shader, meshVec] : m_drawData)
		{
			if (shader != lastBoundShader)
			{
				shader->Bind(stream, shader->GetGPUHandle());
				lastBoundShader = shader;
			}

			LinaGX::CMDDrawIndexedIndirect* draw = stream->AddCommand<LinaGX::CMDDrawIndexedIndirect>();
			draw->count							 = static_cast<uint32>(meshVec.size());
			draw->indirectBuffer				 = indirectBuffer.GetGPUResource();
			draw->indirectBufferOffset			 = indirectOffset;
			indirectOffset += draw->count * static_cast<uint32>(m_lgx->GetIndexedIndirectCommandSize());

#ifdef LINA_DEBUG
			uint32 totalTris = 0;
			for (const auto& md : meshVec)
				totalTris += md.mesh->GetIndexCount() / 3;
			PROFILER_ADD_DRAWCALL(totalTris, "WorldRenderer", 0);
#endif
		}
	}
} // namespace Lina
