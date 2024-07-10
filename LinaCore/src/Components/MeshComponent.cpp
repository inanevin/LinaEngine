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
#include "Common/Serialization/VectorSerialization.hpp"
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
		m_model.SaveToStream(stream);
		m_material.SaveToStream(stream);
		stream << m_meshIndex;
	}

	void MeshComponent::LoadFromStream(IStream& stream)
	{
		m_model.LoadFromStream(stream);
		m_material.LoadFromStream(stream);
		stream >> m_meshIndex;
	}

	void MeshComponent::SetMesh(StringID sid, uint32 meshIndex)
	{
		m_entity->RemoveAABB(m_usedLocalAABB);
		m_model.sid		= sid;
		m_model.raw		= m_resourceManager->GetResource<Model>(m_model.sid);
		m_meshIndex		= meshIndex;
		m_mesh			= m_model.raw->GetMesh(m_meshIndex);
		m_usedLocalAABB = m_mesh->GetAABB();
		m_entity->AddAABB(m_usedLocalAABB);
	}

	void MeshComponent::SetMesh(Model* model, uint32 meshIndex)
	{
		m_entity->RemoveAABB(m_usedLocalAABB);
		m_model.sid		= model->GetSID();
		m_model.raw		= model;
		m_meshIndex		= meshIndex;
		m_mesh			= m_model.raw->GetMesh(m_meshIndex);
		m_usedLocalAABB = m_mesh->GetAABB();
		m_entity->AddAABB(m_usedLocalAABB);
	}

	void MeshComponent::SetMaterial(StringID sid)
	{
		m_material.sid = sid;
		m_material.raw = m_resourceManager->GetResource<Material>(m_material.sid);
	}

	void MeshComponent::SetMaterial(Material* mat)
	{
		m_material.sid = mat->GetSID();
		m_material.raw = mat;
	}

} // namespace Lina
