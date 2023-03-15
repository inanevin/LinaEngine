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

#include "Graphics/Resource/Mesh.hpp"
#include "Math/Color.hpp"
#include "Log/Log.hpp"
#include "Serialization/StringSerialization.hpp"
#include "Serialization/VectorSerialization.hpp"

namespace Lina
{
	void Mesh::AddVertex(const Vector3& pos, const Vector3& normal, const Vector2& uv)
	{
		m_vertices.emplace_back(Vertex(pos, normal, Color(normal.r, normal.g, normal.b, 1), Vector2(uv.x,  uv.y)));
	}

	void Mesh::AddIndices(uint32 i1)
	{
		m_indices.push_back(i1);
	}

	void Mesh::Flush()
	{
		m_vertices.clear();
		m_indices.clear();
	}

	void Mesh::SaveToStream(OStream& stream)
	{
		stream << m_materialSlot;
		m_vertexCenter.SaveToStream(stream);
		m_aabb.SaveToStream(stream);
		StringSerialization::SaveToStream(stream, m_name);
		VectorSerialization::SaveToStream_OBJ(stream, m_vertices);
		VectorSerialization::SaveToStream_PT(stream, m_indices);
	}

	void Mesh::LoadFromStream(IStream& stream)
	{
		stream >> m_materialSlot;
		m_vertexCenter.LoadFromStream(stream);
		m_aabb.LoadFromStream(stream);
		StringSerialization::LoadFromStream(stream, m_name);
		VectorSerialization::LoadFromStream_OBJ(stream, m_vertices);
		VectorSerialization::LoadFromStream_PT(stream, m_indices);
	}
} // namespace Lina
