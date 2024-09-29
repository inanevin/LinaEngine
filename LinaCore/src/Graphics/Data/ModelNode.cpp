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

#include "Core/Graphics/Data/ModelNode.hpp"

namespace Lina
{
	ModelNode::~ModelNode()
	{
		for (auto* c : m_children)
			delete c;

		m_children.clear();

		if (m_mesh != nullptr)
			delete m_mesh;
	}

	void ModelNode::SaveToStream(OStream& stream) const
	{
		stream << m_name;
		stream << m_localMatrix;
		stream << m_meshIndex;

		stream << static_cast<uint32>(m_children.size());

		for (auto* c : m_children)
			c->SaveToStream(stream);

		const bool meshExists = m_mesh != nullptr;
		stream << meshExists;

		if (meshExists)
			m_mesh->SaveToStream(stream);
	}

	void ModelNode::LoadFromStream(IStream& stream)
	{
		stream >> m_name;
		stream >> m_localMatrix;
		stream >> m_meshIndex;

		uint32 childSz = 0;
		stream >> childSz;

		m_children.resize(childSz);
		for (uint32 i = 0; i < childSz; i++)
		{
			ModelNode* node = new ModelNode();
			node->LoadFromStream(stream);
			node->m_parent = this;
			m_children[i]  = node;
		}

		bool meshExists = false;
		stream >> meshExists;

		if (meshExists)
		{
			m_mesh = new MeshDefault();
			m_mesh->LoadFromStream(stream);
			m_mesh->m_node = this;
		}
	}

} // namespace Lina
