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

#include "Graphics/Core/GfxMeshManager.hpp"
#include "Graphics/Core/GfxManager.hpp"
#include "Graphics/Platform/RendererIncl.hpp"
#include "Graphics/Resource/Model.hpp"
#include "Graphics/Resource/ModelNode.hpp"
#include "Graphics/Resource/Mesh.hpp"
#include "System/ISystem.hpp"
#include "Resources/Core/ResourceManager.hpp"
#include "Graphics/Data/Vertex.hpp"
#include "Graphics/Core/IGfxBufferResource.hpp"
#include "Graphics/Core/IUploadContext.hpp"

namespace Lina
{
	GfxMeshManager::GfxMeshManager(GfxManager* gfxManager) : m_gfxManager(gfxManager)
	{
		m_resourceManager = gfxManager->GetSystem()->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager);
	}

	void GfxMeshManager::Initialize()
	{
		m_renderer = m_gfxManager->GetRenderer();
	}

	void GfxMeshManager::Shutdown()
	{
		m_renderer->DeleteBufferResource(m_cpuIndexBuffer);
		m_renderer->DeleteBufferResource(m_cpuVtxBuffer);
		m_renderer->DeleteBufferResource(m_gpuIndexBuffer);
		m_renderer->DeleteBufferResource(m_gpuVtxBuffer);
	}

	void GfxMeshManager::MergeMeshes()
	{
		auto models = m_resourceManager->GetAllResourcesRaw<Model>();

		m_mergedModelIDs.clear();
		m_meshEntries.clear();

		Vector<Vertex> mergedVertices;
		Vector<uint32> mergedIndices;

		uint32 vertexOffset = 0;
		uint32 firstIndex	= 0;

		for (auto model : models)
		{
			m_mergedModelIDs.push_back(model->GetSID());

			for (auto& node : model->GetNodes())
			{
				for (auto& m : node->GetMeshes())
				{
					const auto& vertices = m->GetVertices();
					const auto& indices	 = m->GetIndices();

					MergedBufferMeshEntry entry;
					entry.vertexOffset = vertexOffset;
					entry.firstIndex   = firstIndex;
					entry.indexSize	   = static_cast<uint32>(indices.size());

					const uint32 vtxSize   = static_cast<uint32>(vertices.size());
					const uint32 indexSize = static_cast<uint32>(indices.size());
					for (auto& v : vertices)
						mergedVertices.push_back(v);

					for (auto& i : indices)
						mergedIndices.push_back(i);

					vertexOffset += vtxSize;
					firstIndex += indexSize;

					m_meshEntries[m] = entry;
				}
			}
		}

		const uint32 vtxSize   = static_cast<uint32>(mergedVertices.size() * sizeof(Vertex));
		const uint32 indexSize = static_cast<uint32>(mergedIndices.size() * sizeof(uint32));

		if (m_cpuIndexBuffer == nullptr)
		{
			// m_cpuIndexBuffer = m_renderer->CreateBufferResource(BufferResourceType::IndexBufferSrc, nullptr, indexSize);
			// m_cpuVtxBuffer	 = m_renderer->CreateBufferResource(BufferResourceType::VertexBufferSrc, nullptr, vtxSize);
			m_gpuIndexBuffer = m_renderer->CreateBufferResource(BufferResourceType::IndexBufferDst, nullptr, indexSize);
			m_gpuVtxBuffer	 = m_renderer->CreateBufferResource(BufferResourceType::VertexBufferDst, nullptr, vtxSize);
		}
		else
		{
			// m_cpuIndexBuffer->Recreate(nullptr, indexSize);
			// m_cpuVtxBuffer->Recreate(nullptr, vtxSize);
			m_gpuIndexBuffer->Recreate(nullptr, indexSize);
			m_gpuVtxBuffer->Recreate(nullptr, vtxSize);
		}

		auto uploadContext = m_renderer->GetUploadContext();
		uploadContext->UploadResources(m_gpuVtxBuffer, mergedVertices.data(), vtxSize);
		uploadContext->UploadResources(m_gpuIndexBuffer, mergedIndices.data(), indexSize);
		uploadContext->Flush(0);

		// m_renderer->CopyCPU2GPU(m_cpuIndexBuffer, m_gpuIndexBuffer, mergedIndices.data(), indexSize, ResourceState::IndexBuffer);
		// m_renderer->CopyCPU2GPU(m_cpuVtxBuffer, m_gpuVtxBuffer, mergedVertices.data(), vtxSize, ResourceState::VertexBuffer);
		// m_renderer->WaitForCopyQueue();
	}
} // namespace Lina
