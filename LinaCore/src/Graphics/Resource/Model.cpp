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

#include "Graphics/Resource/Model.hpp"
#include "Graphics/Resource/ModelNode.hpp"
#include "Graphics/Resource/Mesh.hpp"
#include "Graphics/Utility/ModelLoader.hpp"
#include "World/Core/EntityWorld.hpp"
#include "Graphics/Components/ModelNodeComponent.hpp"
#include "Resources/Core/ResourceManager.hpp"

namespace Lina
{
	Model::~Model()
	{
		if (m_rootNode != nullptr)
			delete m_rootNode;
	}

	void Model::LoadFromFile(const char* path)
	{
		// Populate metadata
		m_metadata.GetBool("CalculateTangent"_hs, false);
		m_metadata.GetBool("FlipUVs"_hs, false);
		m_metadata.GetBool("Triangulate"_hs, true);
		m_metadata.GetBool("SmoothNormals"_hs, false);
		m_metadata.GetBool("FlipWinding"_hs, false);
		m_metadata.GetFloat("GlobalScale"_hs, 1.0f);

		if (m_rootNode == nullptr)
			ModelLoader::LoadModel(path, this);
	}

	void Model::SaveToStream(OStream& stream)
	{
		m_metadata.SaveToStream(stream);

		const uint32 nodeSize = static_cast<uint32>(m_nodes.size());
		stream << nodeSize;

		for (uint32 i = 0; i < nodeSize; i++)
			m_nodes[i]->SaveToStream(stream);
	}

	void Model::LoadFromStream(IStream& stream)
	{
		m_metadata.LoadFromStream(stream);

		uint32 nodeSize = 0;
		stream >> nodeSize;

		for (uint32 i = 0; i < nodeSize; i++)
		{
			ModelNode* node = new ModelNode();
			node->LoadFromStream(stream);
			m_nodes.push_back(node);
		}

		if (nodeSize == 0)
			return;

		m_rootNode = m_nodes[0];

		// Assign childrens
		for (auto* node : m_nodes)
			node->FetchChildren(m_nodes);
	}

	Entity* Model::AddToWorld(EntityWorld* w)
	{
		LINA_ASSERT(w != nullptr, "World doesn't exist!");
		return CreateEntityForNode(nullptr, w, m_rootNode);
	}

	Entity* Model::CreateEntityForNode(Entity* parent, EntityWorld* world, ModelNode* node)
	{
		Entity* e = world->CreateEntity(node->GetName());
		if (parent != nullptr)
		{
			parent->AddChild(e);
			e->SetLocalTransformation(node->GetLocalTransform());
		}

		if (!node->GetMeshes().empty())
		{
			auto comp = world->AddComponent<ModelNodeComponent>(e);

			comp.Get().model	 = GetSID();
			comp.Get().nodeIndex = node->GetNodeIndex();

			for (auto mesh : node->GetMeshes())
			{
				const uint32 slot = static_cast<uint32>(mesh->GetMaterialSlot());

				// Assign default material for now.
				comp.Get().materials[slot] = "Resources/Core/Materials/LitStandard.linamat"_hs;
			}
		}

		for (auto c : node->GetChildren())
			CreateEntityForNode(e, world, c);

		return e;
	}

} // namespace Lina
