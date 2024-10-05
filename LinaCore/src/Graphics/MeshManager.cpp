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

#include "Core/Graphics/MeshManager.hpp"
#include "Core/Graphics/Resource/Model.hpp"
#include "Core/Graphics/Resource/Mesh.hpp"
#include "Core/Graphics/ResourceUploadQueue.hpp"

#include "Core/Resources/ResourceManager.hpp"
#include "Core/Graphics/Data/Vertex.hpp"
#include "Core/Graphics/Data/Mesh.hpp"

namespace Lina
{

#define MAX_VERTICES 100000
#define MAX_INDICES	 300000

	void MeshManager::Initialize()
	{
		for (size_t i = 0; i < MESH_BUF_SIZE; i++)
		{
			auto& buf = m_meshBuffers[i];
			buf.vertexBuffer.Create(LinaGX::ResourceTypeHint::TH_VertexBuffer, MAX_VERTICES * sizeof(VertexDefault), "MeshManager Big VertexBuffer");
			buf.indexBuffer.Create(LinaGX::ResourceTypeHint::TH_IndexBuffer, MAX_INDICES * sizeof(uint16), "MeshManager Big IndexBuffer");
		}
	}

	void MeshManager::Shutdown()
	{
		for (size_t i = 0; i < MESH_BUF_SIZE; i++)
		{
			auto& buf = m_meshBuffers[i];
			buf.vertexBuffer.Destroy();
			buf.indexBuffer.Destroy();
		}
	}

	void MeshManager::BindBuffers(LinaGX::CommandStream* stream, uint32 bufferIndex)
	{
		auto& buf = m_meshBuffers[bufferIndex];
		buf.indexBuffer.BindIndex(stream, LinaGX::IndexType::Uint16);
		buf.vertexBuffer.BindVertex(stream, sizeof(VertexDefault));
	}

	void MeshManager::AddMesh(MeshDefault* mesh)
	{
		auto& buf = m_meshBuffers[0];

		buf.meshes.push_back(mesh);

		mesh->m_vertexOffset = static_cast<uint32>(buf.startVertex);
		mesh->m_indexOffset	 = static_cast<uint32>(buf.startIndex);

		const size_t vtxSize = sizeof(VertexDefault) * mesh->m_vertices.size();
		buf.vertexBuffer.BufferData(buf.startVertex * sizeof(VertexDefault), (uint8*)(mesh->m_vertices.data()), vtxSize);
		buf.startVertex += static_cast<uint32>(mesh->m_vertices.size());

		const size_t indexSize = sizeof(uint16) * mesh->m_indices16.size();
		buf.indexBuffer.BufferData(buf.startIndex * sizeof(uint16), (uint8*)(mesh->m_indices16.data()), indexSize);
		buf.startIndex += static_cast<uint32>(mesh->m_indices16.size());
	}

	void MeshManager::AddToUploadQueue(ResourceUploadQueue& queue)
	{
		auto& buf = m_meshBuffers[0];
		queue.AddBufferRequest(&buf.vertexBuffer);
		queue.AddBufferRequest(&buf.indexBuffer);
	}

	void MeshManager::RemoveMesh(MeshDefault* mesh)
	{
		auto& buf = m_meshBuffers[0];
		buf.meshes.erase(linatl::find_if(buf.meshes.begin(), buf.meshes.end(), [mesh](MeshDefault* m) -> bool { return m == mesh; }));
		m_requiresRefresh = true;
	}

	void MeshManager::Refresh()
	{
		/*
		if (!m_requiresRefresh)
			return;

		auto& buf = m_meshBuffers[0];

		buf.startIndex = buf.startVertex = 0;

		for (auto* m : buf.meshes)
		{
			m->m_vertexOffset = static_cast<uint32>(buf.startVertex);
			m->m_indexOffset  = static_cast<uint32>(buf.startIndex);

			const size_t vtxSize = sizeof(VertexDefault) * m->m_vertices.size();
			buf.vertexBuffer.BufferData(buf.startVertex * sizeof(VertexDefault), (uint8*)(m->m_vertices.data()), vtxSize);
			buf.startVertex += static_cast<uint32>(m->m_vertices.size());

			const size_t indexSize = sizeof(uint16) * m->m_indices16.size();
			buf.indexBuffer.BufferData(buf.startIndex * sizeof(uint16), (uint8*)(m->m_indices16.data()), indexSize);
			buf.startIndex += static_cast<uint32>(m->m_indices16.size());
		}

		queue.AddBufferRequest(&buf.vertexBuffer);
		queue.AddBufferRequest(&buf.indexBuffer);
		m_requiresRefresh = false;
		 */
	}

} // namespace Lina
