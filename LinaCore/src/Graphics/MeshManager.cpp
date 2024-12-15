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
		CreateBuffer(m_bufferStatic, MAX_VERTICES * sizeof(VertexStatic), MAX_INDICES * sizeof(uint16));
		CreateBuffer(m_bufferSkinned, MAX_VERTICES * sizeof(VertexSkinned), MAX_INDICES * sizeof(uint16));
	}

	void MeshManager::Shutdown()
	{
		DestroyBuffer(m_bufferSkinned);
		DestroyBuffer(m_bufferStatic);
	}

	void MeshManager::AddMesh(Mesh* mesh)
	{
		if (mesh->primitivesSkinned.empty())
		{
			m_bufferStatic.meshes.push_back(mesh);
			mesh->_vertexOffset = static_cast<uint32>(m_bufferStatic.startVertex);
			mesh->_indexOffset	= static_cast<uint32>(m_bufferStatic.startIndex);

			for (PrimitiveStatic& prim : mesh->primitivesStatic)
			{
				prim._vertexOffset = static_cast<uint32>(m_bufferStatic.startVertex);
				prim._indexOffset  = static_cast<uint32>(m_bufferStatic.startIndex);

				m_bufferStatic.vertexBuffer.BufferData(m_bufferStatic.startVertex * sizeof(VertexStatic), (uint8*)prim.vertices.data(), sizeof(VertexStatic) * prim.vertices.size());
				m_bufferStatic.startVertex += static_cast<uint32>(prim.vertices.size());

				m_bufferStatic.indexBuffer.BufferData(m_bufferStatic.startIndex * sizeof(uint16), (uint8*)prim.indices.data(), sizeof(uint16) * prim.indices.size());
				m_bufferStatic.startIndex += static_cast<uint32>(prim.indices.size());
			}
		}
		else
		{
			m_bufferSkinned.meshes.push_back(mesh);
			mesh->_vertexOffset = static_cast<uint32>(m_bufferSkinned.startVertex);
			mesh->_indexOffset	= static_cast<uint32>(m_bufferSkinned.startIndex);

			for (PrimitiveSkinned& prim : mesh->primitivesSkinned)
			{
				prim._vertexOffset = static_cast<uint32>(m_bufferSkinned.startVertex);
				prim._indexOffset  = static_cast<uint32>(m_bufferSkinned.startIndex);

				m_bufferSkinned.vertexBuffer.BufferData(m_bufferSkinned.startVertex * sizeof(VertexSkinned), (uint8*)prim.vertices.data(), sizeof(VertexSkinned) * prim.vertices.size());
				m_bufferSkinned.startVertex += static_cast<uint32>(prim.vertices.size());

				m_bufferSkinned.indexBuffer.BufferData(m_bufferSkinned.startIndex * sizeof(uint16), (uint8*)prim.indices.data(), sizeof(uint16) * prim.indices.size());
				m_bufferSkinned.startIndex += static_cast<uint32>(prim.indices.size());
			}
		}
	}

	void MeshManager::AddToUploadQueue(ResourceUploadQueue& queue)
	{
		queue.AddBufferRequest(&m_bufferStatic.vertexBuffer);
		queue.AddBufferRequest(&m_bufferStatic.indexBuffer);
		queue.AddBufferRequest(&m_bufferSkinned.vertexBuffer);
		queue.AddBufferRequest(&m_bufferSkinned.indexBuffer);
	}

	void MeshManager::BindStatic(LinaGX::CommandStream* stream)
	{
		m_bufferStatic.indexBuffer.BindIndex(stream, LinaGX::IndexType::Uint16);
		m_bufferStatic.vertexBuffer.BindVertex(stream, sizeof(VertexStatic));
	}

	void MeshManager::BindSkinned(LinaGX::CommandStream* stream)
	{
		m_bufferSkinned.indexBuffer.BindIndex(stream, LinaGX::IndexType::Uint16);
		m_bufferSkinned.vertexBuffer.BindVertex(stream, sizeof(VertexSkinned));
	}

	void MeshManager::RemoveMesh(Mesh* mesh)
	{
		auto& buf = mesh->primitivesSkinned.empty() ? m_bufferStatic : m_bufferSkinned;
		buf.meshes.erase(linatl::find_if(buf.meshes.begin(), buf.meshes.end(), [mesh](Mesh* m) -> bool { return m == mesh; }));
	}

	void MeshManager::Refresh()
	{
		m_bufferStatic.startIndex = m_bufferStatic.startVertex = 0;
		m_bufferSkinned.startIndex = m_bufferSkinned.startVertex = 0;

		Vector<Mesh*> meshesStatic	= m_bufferStatic.meshes;
		Vector<Mesh*> meshesSkinned = m_bufferSkinned.meshes;
		m_bufferStatic.meshes.clear();
		m_bufferSkinned.meshes.clear();

		for (Mesh* m : meshesStatic)
			AddMesh(m);

		for (Mesh* m : meshesSkinned)
			AddMesh(m);
	}

	void MeshManager::CreateBuffer(MeshBuffer& buf, size_t vtxSz, size_t idxSz)
	{
		buf.vertexBuffer.Create(LinaGX::ResourceTypeHint::TH_VertexBuffer, static_cast<uint32>(vtxSz), "MeshManager Big VertexBuffer");
		buf.indexBuffer.Create(LinaGX::ResourceTypeHint::TH_IndexBuffer, static_cast<uint32>(idxSz), "MeshManager Big IndexBuffer");
	}

	void MeshManager::DestroyBuffer(MeshBuffer& buf)
	{
		buf.vertexBuffer.Destroy();
		buf.indexBuffer.Destroy();
	}

} // namespace Lina
