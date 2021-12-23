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
#include "ECS/Components/PhysicsComponent.hpp"
#include "Rendering/Model.hpp"

namespace Lina::ECS
{
	// We keep track of all the children added to the entity hierarchy upon setting model renderer's model.
	// So we can extract any children that is not added by the model meshes upon deletion and get to keep them.
	std::vector<ECS::Entity> m_processedChildren;

	bool EntityIsProcessed(ECS::Entity entity)
	{
		for (uint32 i = 0; i < m_processedChildren.size(); i++)
		{
			// Added by the user
			if (entity == m_processedChildren[i])
				return true;
		}

		return false;
	}

	bool IsMarkedByInheritance(ECS::Entity entity, std::vector<Entity>& vec)
	{
		for (uint32 i = 0; i < vec.size(); i++)
		{
			// Added by the user
			if (entity == vec[i])
				return true;
		}

		return false;
	}

	void MarkChildren(ECS::Entity entity, std::vector<ECS::Entity>& markedByInheritance)
	{
		auto* reg = ECS::Registry::Get();
		auto& data = reg->get<ECS::EntityDataComponent>(entity);
		for (auto subChild : data.m_children)
		{
			if (!EntityIsProcessed(entity))
			{
				markedByInheritance.push_back(subChild);
				MarkChildren(subChild, markedByInheritance);
			}
		}
	}

	// Traverse the child hierarchy of a root entity
	// Find those entities that were not added by the auto-hierarchy generated from the model.
	void FindEntitiesToKeep(ECS::Entity entity, std::vector<ECS::Entity>& keepVector)
	{
		auto* reg = ECS::Registry::Get();
		auto& data = reg->get<ECS::EntityDataComponent>(entity);

		for (auto child : data.m_children)
		{
			if (!EntityIsProcessed(child))
				keepVector.push_back(child);

			FindEntitiesToKeep(child, keepVector);
		}
	}

	void ModelRendererComponent::ProcessNode(ECS::Entity parent, const Lina::Matrix& parentTransform, Graphics::ModelNode& node, Graphics::Model& model, bool isRoot)
	{
		ECS::Registry* reg = Lina::ECS::Registry::Get();
		ECS::Entity nodeEntity = entt::null;
	
		if (isRoot)
			nodeEntity = parent;
		else
		{
			nodeEntity = reg->CreateEntity(node.m_name);
			reg->AddChildToEntity(parent, nodeEntity);
		}

		m_processedChildren.push_back(nodeEntity);

		ECS::EntityDataComponent& data = reg->get<ECS::EntityDataComponent>(nodeEntity);
		Matrix mat = parentTransform * node.m_localTransform;
		data.SetTransformation(mat, false);

		if (node.m_meshIndexes.size() > 0)
		{
			AddMeshRenderer(nodeEntity, node.m_meshIndexes, model);
			auto& mr = reg->get<ECS::MeshRendererComponent>(nodeEntity);

			if (m_generateMeshPivots)
			{
				const Vector3 vertexOffset = mr.m_localOffset * data.GetScale();
				const Vector3 offsetAddition = data.GetRotation().GetForward() * vertexOffset.z +
					data.GetRotation().GetRight() * vertexOffset.x +
					data.GetRotation().GetUp() * vertexOffset.y;

				ECS::Entity pivotParent = reg->CreateEntity(node.m_name + "_pvt");
				reg->AddChildToEntity(parent, pivotParent);
				auto& pivotParentData = reg->get<ECS::EntityDataComponent>(pivotParent);
				pivotParentData.SetLocation(data.GetLocation() + offsetAddition);
				pivotParentData.SetRotation(data.GetRotation());
				reg->AddChildToEntity(pivotParent, nodeEntity);
				m_processedChildren.push_back(pivotParent);
			}
			
		}

		// Process node for each children.
		for (uint32 i = 0; i < node.m_children.size(); i++)
			ProcessNode(nodeEntity, mat, node.m_children[i], model);
	}

	void ModelRendererComponent::AddMeshRenderer(ECS::Entity targetEntity, const std::vector<int>& meshIndexes, Graphics::Model& model)
	{
		auto& defaultMaterial = Graphics::Material::GetMaterial("Resources/Engine/Materials/DefaultLit.mat");
		ECS::Registry* reg = Lina::ECS::Registry::Get();
		auto& mr = reg->emplace<ECS::MeshRendererComponent>(targetEntity);
		mr.m_subMeshes = meshIndexes;
		mr.m_modelID = model.GetID();
		mr.m_modelPath = m_modelPath;

		// Set default material to mesh renderer.
		mr.m_materialID = defaultMaterial.GetID();
		mr.m_materialPath = defaultMaterial.GetPath();
		mr.m_isEnabled = m_isEnabled;

		// Calc local offset from all submeshes.
		mr.m_localOffset = Vector3::Zero;
		for (uint32 i = 0; i < mr.m_subMeshes.size(); i++)
			mr.m_localOffset += model.GetMeshes()[mr.m_subMeshes[i]].GetLocalOffset();
		mr.m_localOffset /= mr.m_subMeshes.size();
		
		// Set the child's bounding volume from mesh data.
		auto& data = reg->get<ECS::EntityDataComponent>(targetEntity);
		//data.m_halfBounds = halfBounds;
	}

	void ModelRendererComponent::SetModel(ECS::Entity parent, Graphics::Model& model)
	{

		ECS::Registry* reg = Lina::ECS::Registry::Get();
		ModelRendererComponent* modelRendererInEntity = reg->try_get<ModelRendererComponent>(parent);
		if (this != modelRendererInEntity)
		{
			LINA_ERR("You have to pass the same entity that this model component is attached to in order to set the model data. {0}", model.GetPath());
			return;
		}

		// Assign model data
		StringIDType previousModelID = m_modelID;
		m_modelID = model.GetID();
		m_modelPath = model.GetPath();
		m_modelParamsPath = model.GetParamsPath();
		m_materialPaths.clear();
		m_materialPaths.resize(model.GetMaterialSpecs().size());
		auto& defaultMaterial = Graphics::Material::GetMaterial("Resources/Engine/Materials/DefaultLit.mat");

		RefreshHierarchy(parent, previousModelID);

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
				for (uint32 i = 0; i < meshRenderer->m_subMeshes.size(); i++)
				{
					auto& mesh = model.GetMeshes()[meshRenderer->m_subMeshes[i]];
					if (materialIndex == mesh.GetMaterialSlotIndex())
					{
						meshRenderer->m_materialID = material.GetID();
						meshRenderer->m_materialPath = material.GetPath();
					}
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
				for (uint32 i = 0; i < meshRenderer->m_subMeshes.size(); i++)
				{
					auto& mesh = model.GetMeshes()[meshRenderer->m_subMeshes[i]];
					if (materialIndex == mesh.GetMaterialSlotIndex())
						meshRenderer->m_materialID = -1;
				}
			}
		}
	}

	void ModelRendererComponent::RefreshHierarchy(ECS::Entity parent, StringIDType previousModelID)
	{
		if (!Graphics::Model::ModelExists(m_modelID)) return;
		ECS::Registry* reg = Lina::ECS::Registry::Get();
		ECS::EntityDataComponent& data = reg->get<ECS::EntityDataComponent>(parent);
		
		std::vector<ECS::Entity> markedForNullParent;
		FindEntitiesToKeep(parent, markedForNullParent);
	
		for (uint32 i = 0; i < markedForNullParent.size(); i++)
			reg->RemoveFromParent(markedForNullParent[i]);

		reg->DestroyAllChildren(parent);
		auto& model = Graphics::Model::GetModel(m_modelID);
		ProcessNode(parent, data.ToMatrix(), model.GetRoot(), model, true);
	}
	

}