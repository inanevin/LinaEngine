/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

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

#include "ECS/Components/ModelRendererComponent.hpp"
#include "ECS/Components/MeshRendererComponent.hpp"
#include "ECS/Components/EntityDataComponent.hpp"
#include "Rendering/Model.hpp"

namespace Lina::ECS
{
	void ModelRendererComponent::SetModel(ECS::Entity parent, Graphics::Model& model)
	{
		ECS::Registry* reg = Lina::ECS::Registry::Get();
		
		// Assign model data
		m_modelID = model.GetID();
		m_modelPath = model.GetPath();
		m_modelParamsPath = model.GetParamsPath();
		m_materialPaths.clear();
		m_materialPaths.resize(model.GetMaterialSpecs().size());

		// Make sure children hierarchy of the entity that this component is attached is empty.
		reg->DestroyAllChildren(parent);

		ModelRendererComponent* modelRendererInEntity = reg->try_get<ModelRendererComponent>(parent);
		if (this != modelRendererInEntity)
		{
			LINA_ERR("You have to pass the same entity that this model component is attached to in order to set the model data. {0}", model.GetPath());
			return;
		}

		auto& defaultMaterial = Graphics::Material::GetMaterial("Resources/Engine/Materials/DefaultLit.mat");

		// Generate entities for each children.
		for (int i = 0; i < model.GetMeshes().size(); i++)
		{
			auto& mesh = model.GetMeshes()[i];
			ECS::Entity newEntity = reg->CreateEntity(mesh.GetName());
			reg->AddChildToEntity(parent, newEntity);
			auto& mr = reg->emplace<ECS::MeshRendererComponent>(newEntity);
			mr.m_meshIndex = i;
			mr.m_modelID = model.GetID();
			mr.m_modelPath = m_modelPath;

			// Set default material to mesh renderer.
			mr.m_materialID = defaultMaterial.GetID();
			mr.m_materialPath = defaultMaterial.GetPath();

			mr.m_isEnabled = m_isEnabled;
		}

		// Set default material to all the paths.
		for (int i = 0; i < model.GetMaterialSpecs().size(); i++)
			m_materialPaths[i] = defaultMaterial.GetPath();

		m_materialCount = model.GetMaterialSpecs().size();
	}

	void ModelRendererComponent::RemoveModel(ECS::Entity parent)
	{
		ECS::Registry* reg = Lina::ECS::Registry::Get();
		ModelRendererComponent* modelRendererInEntity = reg->try_get<ModelRendererComponent>(parent);
		if (this != modelRendererInEntity)
		{
			LINA_ERR("You have to pass the same entity that this model component is attached to in order to remove the model data.");
			return;
		}
		// Make sure children hierarchy of the entity that this component is attached is empty.
		reg->DestroyAllChildren(parent);
		m_modelID = -1;
		m_modelParamsPath = "";
		m_modelPath = "";
		m_materialPaths.clear();
	}

	void ModelRendererComponent::SetMaterial(ECS::Entity parent, int materialIndex, const Graphics::Material& material)
	{
		ECS::Registry* reg = Lina::ECS::Registry::Get();

		ModelRendererComponent* modelRendererInEntity = reg->try_get<ModelRendererComponent>(parent);
		if (this != modelRendererInEntity)
		{
			LINA_ERR("You have to pass the same entity that this model component is attached to in order to set the material data. {0}", material.GetPath());
			return;
		}

		// Set the path
		m_materialPaths[materialIndex] = material.GetPath();

		// Find the mesh that points to this material index.
		auto& model = Graphics::Model::GetModel(m_modelID);
		ECS::EntityDataComponent& data = reg->get<ECS::EntityDataComponent>(parent);

		for (auto child : data.m_children)
		{
			MeshRendererComponent* meshRenderer = reg->try_get<MeshRendererComponent>(child);

			if (meshRenderer != nullptr)
			{
				auto& mesh = model.GetMeshes()[meshRenderer->m_meshIndex];
				if (materialIndex == mesh.GetMaterialSlotIndex())
				{
					meshRenderer->m_materialID = material.GetID();
					meshRenderer->m_materialPath = material.GetPath();
				}
			}
		}
	}

	void ModelRendererComponent::RemoveMaterial(ECS::Entity parent, int materialIndex)
	{
		ECS::Registry* reg = Lina::ECS::Registry::Get();

		ModelRendererComponent* modelRendererInEntity = reg->try_get<ModelRendererComponent>(parent);
		if (this != modelRendererInEntity)
		{
			LINA_ERR("You have to pass the same entity that this model component is attached to in order to remove the material data. {0}", materialIndex);
			return;
		}

		m_materialPaths[materialIndex] = "";

		// Find the mesh that points to this material index.
		auto& model = Graphics::Model::GetModel(m_modelID);
		ECS::EntityDataComponent& data = reg->get<ECS::EntityDataComponent>(parent);

		for (auto child : data.m_children)
		{
			MeshRendererComponent* meshRenderer = reg->try_get<MeshRendererComponent>(child);

			if (meshRenderer != nullptr)
			{
				auto& mesh = model.GetMeshes()[meshRenderer->m_meshIndex];
				if (materialIndex == mesh.GetMaterialSlotIndex())
				{
					meshRenderer->m_materialID = -1;
				}
			}
		}
	}
}