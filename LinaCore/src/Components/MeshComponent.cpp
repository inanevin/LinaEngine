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

#include "Core/Components/MeshComponent.hpp"

#include "Core/Resources/ResourceManager.hpp"
#include "Core/Graphics/Resource/Model.hpp"
#include "Core/Graphics/Data/ModelNode.hpp"

namespace Lina
{
	void MeshComponent::Create()
	{
	}

	void MeshComponent::Destroy()
	{
		m_entity->RemoveAABB(m_usedLocalAABB);
	}

	void MeshComponent::SaveToStream(OStream& stream) const
	{
		stream << m_model << m_material << m_meshIndex;
	}

	void MeshComponent::LoadFromStream(IStream& stream)
	{
		stream >> m_model >> m_material >> m_meshIndex;
	}

	void MeshComponent::SetMesh(ResourceID id, uint32 meshIndex)
	{
		m_entity->RemoveAABB(m_usedLocalAABB);
		m_model.id		= id;
		m_model.raw		= m_resourceManager->GetResource<Model>(m_model.id);
		m_meshIndex		= meshIndex;
		m_mesh			= m_model.raw->GetMesh(m_meshIndex);
		m_usedLocalAABB = m_mesh->GetAABB();
		m_entity->AddAABB(m_usedLocalAABB);
	}

	void MeshComponent::SetMesh(Model* model, uint32 meshIndex)
	{
		m_entity->RemoveAABB(m_usedLocalAABB);
		m_model.id		= model->GetID();
		m_model.raw		= model;
		m_meshIndex		= meshIndex;
		m_mesh			= m_model.raw->GetMesh(m_meshIndex);
		m_usedLocalAABB = m_mesh->GetAABB();
		m_entity->AddAABB(m_usedLocalAABB);
	}

	void MeshComponent::SetMaterial(ResourceID id)
	{
		m_material.id  = id;
		m_material.raw = m_resourceManager->GetResource<Material>(m_material.id);
	}

	void MeshComponent::SetMaterial(Material* mat)
	{
		m_material.id  = mat->GetID();
		m_material.raw = mat;
	}

} // namespace Lina
