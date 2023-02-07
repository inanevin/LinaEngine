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

#include "Graphics/Resource/ModelNode.hpp"
#include "Serialization/StringSerialization.hpp"
#include "Serialization/VectorSerialization.hpp"
#include "Graphics/Resource/StaticMesh.hpp"
#include "Graphics/Resource/SkinnedMesh.hpp"

namespace Lina
{
	ModelNode::~ModelNode()
	{
		for (auto m : m_meshes)
			delete m;

		for (auto c : m_children)
			delete c;

		m_children.clear();
	}

	void ModelNode::SaveToStream(OStream& stream)
	{
		m_aabb.SaveToStream(stream);
		m_localTransform.SaveToStream(stream);
		m_totalVertexCenter.SaveToStream(stream);
		StringSerialization::SaveToStream(stream, m_name);
		stream << m_index;

		Vector<uint32> childrenIndices;

		for (auto& c : m_children)
			childrenIndices.push_back(c->m_index);

		VectorSerialization::SaveToStream_PT(stream, childrenIndices);

		const uint32 meshesSize = static_cast<uint32>(m_meshes.size());
		stream << meshesSize;

		for (auto* m : m_meshes)
		{
			const uint8 mt = static_cast<uint8>(m->GetMeshType());
			stream << mt;
			m->SaveToStream(stream);
		}
	}

	void ModelNode::LoadFromStream(IStream& stream)
	{

		m_aabb.LoadFromStream(stream);
		m_localTransform.LoadFromStream(stream);
		m_totalVertexCenter.LoadFromStream(stream);
		StringSerialization::LoadFromStream(stream, m_name);
		stream >> m_index;

		VectorSerialization::LoadFromStream_PT(stream, m_childrenIndices);

		uint32 meshesSize = 0;
		stream >> meshesSize;

		for (uint32 i = 0; i < meshesSize; i++)
		{
			uint8 mtint = 0;
			stream >> mtint;
			MeshType mt = static_cast<MeshType>(mtint);
			Mesh*	 m	= nullptr;

			if (mt == MeshType::StaticMesh)
				m = new StaticMesh();
			else
				m = new SkinnedMesh();

			m->LoadFromStream(stream);
			m_meshes.push_back(m);
		}
	}

	void ModelNode::FetchChildren(const Vector<ModelNode*>& allNodes)
	{
		for (auto& ci : m_childrenIndices)
			m_children.push_back(allNodes.at(ci));
	}
} // namespace Lina
