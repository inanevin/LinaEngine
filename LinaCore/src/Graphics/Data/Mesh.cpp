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

#include "Core/Graphics/Data/Mesh.hpp"
#include "Core/Graphics/GfxManager.hpp"
#include "Common/Data/Streams.hpp"
#include "Common/Serialization/StringSerialization.hpp"
#include "Common/Serialization/VectorSerialization.hpp"
#include "Common/Platform/LinaGXIncl.hpp"

namespace Lina
{
	void MeshDefault::SaveToStream(OStream& stream) const
	{
		StringSerialization::SaveToStream(stream, m_name);
		VectorSerialization::SaveToStream_PT(stream, m_indices16);
		VectorSerialization::SaveToStream_OBJ(stream, m_vertices);
		VectorSerialization::SaveToStream_OBJ(stream, m_primitives);
	}

	void MeshDefault::LoadFromStream(IStream& stream)
	{
		StringSerialization::LoadFromStream(stream, m_name);
		VectorSerialization::LoadFromStream_PT(stream, m_indices16);
		VectorSerialization::LoadFromStream_OBJ(stream, m_vertices);
		VectorSerialization::LoadFromStream_OBJ(stream, m_primitives);
	}

	MeshDefault::~MeshDefault()
	{
		m_vertexBuffer.Destroy();
		m_indexBuffer.Destroy();
	}

	void MeshDefault::Create(GfxManager* gfxMan)
	{
		m_gfxManager = gfxMan;
		auto* lgx	 = gfxMan->GetLGX();
		m_vertexBuffer.Create(lgx, LinaGX::ResourceTypeHint::TH_VertexBuffer, static_cast<uint32>(sizeof(VertexDefault) * m_vertices.size()), m_name);
		m_indexBuffer.Create(lgx, LinaGX::ResourceTypeHint::TH_IndexBuffer, static_cast<uint32>(sizeof(uint16) * m_indices16.size()), m_name);
		m_vertexBuffer.BufferData(0, reinterpret_cast<uint8*>(m_vertices.data()), sizeof(VertexDefault) * m_vertices.size());
		m_indexBuffer.BufferData(0, reinterpret_cast<uint8*>(m_indices16.data()), sizeof(uint16) * m_indices16.size());
		m_gfxManager->GetResourceUploadQueue().AddBufferRequest(&m_vertexBuffer);
		m_gfxManager->GetResourceUploadQueue().AddBufferRequest(&m_indexBuffer);
	}

	void MeshDefault::Draw(LinaGX::CommandStream* stream, uint32 instances)
	{
		m_vertexBuffer.BindVertex(stream, sizeof(VertexDefault));
		m_indexBuffer.BindIndex(stream, LinaGX::IndexType::Uint16);

		for (const auto& prim : m_primitives)
		{
			LinaGX::CMDDrawIndexedInstanced* draw = stream->AddCommand<LinaGX::CMDDrawIndexedInstanced>();
			draw->indexCountPerInstance			  = static_cast<uint32>(m_indices16.size());
			draw->instanceCount					  = instances;
			draw->startInstanceLocation			  = 0;
			draw->startIndexLocation			  = prim.GetStartIndex();
			draw->baseVertexLocation			  = prim.GetStartVertex();
		}
	}
} // namespace Lina
