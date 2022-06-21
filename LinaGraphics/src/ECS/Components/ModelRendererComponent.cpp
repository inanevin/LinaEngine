#include "ECS/Components/ModelRendererComponent.hpp"

#include "Rendering/Model.hpp"

namespace Lina::ECS
{
    // We keep track of all the children added to the entity hierarchy upon setting model renderer's model.
    // So we can extract any children that is not added by the model meshes upon deletion and get to keep them.
    Vector<ECS::Entity> m_processedChildren;

    bool EntityIsProcessed(ECS::Entity entity)
    {
        // for (uint32 i = 0; i < m_processedChildren.size(); i++)
        //{
        //	// Added by the user
        //	if (entity == m_processedChildren[i])
        //		return true;
        //}

        return false;
    }

    // Traverse the child hierarchy of a root entity
    // Find those entities that were not added by the auto-hierarchy generated from the model.
    void FindEntitiesToKeep(ECS::Entity entity, Vector<ECS::Entity>& keepVector)
    {
        // auto* reg = ECS::Registry::Get();
        // auto& data = reg->get<ECS::EntityDataComponent>(entity);
        //
        // for (auto child : data.m_children)
        //{
        //	if (!EntityIsProcessed(child))
        //		keepVector.push_back(child);
        //
        //	FindEntitiesToKeep(child, keepVector);
        //}
    }

    void RemoveUnprocessedChildren(ECS::Entity entity)
    {
        // ECS::Registry* reg = ECS::Registry::Get();
        //
        // Vector<ECS::Entity> markedForNullParent;
        // FindEntitiesToKeep(entity, markedForNullParent);
        //
        // for (uint32 i = 0; i < markedForNullParent.size(); i++)
        //	reg->RemoveFromParent(markedForNullParent[i]);
    }

    void ModelRendererComponent::ProcessNode(ECS::Entity parent, const Matrix& parentTransform, Graphics::ModelNode& node, Graphics::Model& model, bool isRoot)
    {
        // ECS::Registry* reg = ECS::Registry::Get();
        // ECS::Entity nodeEntity = entt::null;
        //
        // if (isRoot)
        //	nodeEntity = parent;
        // else
        //{
        //	nodeEntity = reg->CreateEntity(node.m_name);
        //	reg->AddChildToEntity(parent, nodeEntity);
        //}
        //
        // m_processedChildren.push_back(nodeEntity);
        //
        // ECS::EntityDataComponent& data = reg->get<ECS::EntityDataComponent>(nodeEntity);
        // Matrix mat = parentTransform * node.m_localTransform;
        // data.SetTransformation(mat, false);
        //
        // if (node.m_meshIndexes.size() > 0)
        //{
        //	AddMeshRenderer(nodeEntity, node.m_meshIndexes, model);
        //	auto& mr = reg->get<ECS::MeshRendererComponent>(nodeEntity);
        //
        //	if (m_generateMeshPivots)
        //	{
        //		// Calculate vertex offset & offset addition.
        //		const Vector3 vertexOffset = mr.m_totalVertexCenter * data.GetScale();
        //		const Vector3 offsetAddition = data.GetRotation().GetForward() * vertexOffset.z + data.GetRotation().GetRight() * vertexOffset.x + data.GetRotation().GetUp() * vertexOffset.y;
        //
        //		// Create an entity, attach as a child, move to calculated position, make it a parent of the current entity.
        //		ECS::Entity pivotParent = reg->CreateEntity(node.m_name + "_pvt");
        //		reg->AddChildToEntity(parent, pivotParent);
        //		auto& pivotParentData = reg->get<ECS::EntityDataComponent>(pivotParent);
        //		pivotParentData.SetLocation(data.GetLocation() + offsetAddition);
        //		pivotParentData.SetRotation(data.GetRotation());
        //		reg->AddChildToEntity(pivotParent, nodeEntity);
        //		m_processedChildren.push_back(pivotParent);
        //	}
        //
        //}
        //
        //// Process node for each children.
        // for (uint32 i = 0; i < node.m_children.size(); i++)
        //	ProcessNode(nodeEntity, mat, node.m_children[i], model);
    }

    void ModelRendererComponent::AddMeshRenderer(ECS::Entity targetEntity, const Vector<int>& meshIndexes, Graphics::Model& model)
    {
        // ECS::Registry* reg = ECS::Registry::Get();
        // auto& defaultMaterial = Graphics::Material::GetMaterial("Resources/Engine/Materials/DefaultLit.linamat");
        // auto& mr = reg->emplace<ECS::MeshRendererComponent>(targetEntity);
        // auto& data = reg->get<ECS::EntityDataComponent>(targetEntity);
        // mr.m_subMeshes = meshIndexes;
        // mr.m_modelID = model.GetID();
        // mr.m_modelPath = m_modelPath;
        //
        //// Set default material to mesh renderer.
        // mr.m_materialID = defaultMaterial.GetID();
        // mr.m_materialPath = defaultMaterial.GetPath();
        // mr.m_isEnabled = m_isEnabled;
        //
        //// Calc vertex offset & bounding boxfrom all submeshes.
        // const uint32 numSubmeshes = mr.m_subMeshes.size();
        // mr.m_totalVertexCenter = Vector3::Zero;
        //
        // Vector3 minBound = Vector3(1000.0f, 1000.0f, 1000.0f);
        // Vector3 maxBound = Vector3(-1000.0f, -1000.0f, -1000.0f);
        //
        // for (uint32 i = 0; i < numSubmeshes; i++)
        //{
        //	auto& mesh = model.GetMeshes()[mr.m_subMeshes[i]];
        //	mr.m_totalVertexCenter += mesh.GetVertexCenter();
        //
        //	const Vector3 meshBoundsMin = mesh.GetBoundsMin();
        //	const Vector3 meshBoundsMax = mesh.GetBoundsMax();
        //
        //	if (meshBoundsMin.x < minBound.x)
        //		minBound.x = meshBoundsMin.x;
        //	if (meshBoundsMin.y < minBound.y)
        //		minBound.y = meshBoundsMin.y;
        //	if (meshBoundsMin.z < minBound.z)
        //		minBound.z = meshBoundsMin.z;
        //
        //	if (meshBoundsMax.x > maxBound.x)
        //		maxBound.x = meshBoundsMax.x;
        //	if (meshBoundsMax.y > maxBound.y)
        //		maxBound.y = meshBoundsMax.y;
        //	if (meshBoundsMax.z > maxBound.z)
        //		maxBound.z = meshBoundsMax.z;
        //}
        //
        // mr.m_totalVertexCenter /= numSubmeshes;
        // mr.m_totalBoundsMin = minBound;
        // mr.m_totalBoundsMax = maxBound;
        // mr.m_totalHalfBounds = (maxBound - minBound) / 2.0f;
    }

    void ModelRendererComponent::SetModel(ECS::Entity parent, Graphics::Model& model)
    {

        // ECS::Registry* reg = ECS::Registry::Get();
        // ModelRendererComponent* modelRendererInEntity = reg->try_get<ModelRendererComponent>(parent);
        // if (this != modelRendererInEntity)
        //{
        //	LINA_ERR("You have to pass the same entity that this model component is attached to in order to set the model data. {0}", model.GetPath());
        //	return;
        //}
        //
        //// Assign model data
        // m_modelID = model.GetID();
        // m_modelPath = model.GetPath();
        // m_modelParamsPath = model.GetAssetDataPath();
        // m_materialPaths.clear();
        // m_materialPaths.resize(model.GetImportedMaterials().size());
        // auto& defaultMaterial = Graphics::Material::GetMaterial("Resources/Engine/Materials/DefaultLit.linamat");
        //
        // RefreshHierarchy(parent);
        //
        //// Set default material to all the paths.
        // for (int i = 0; i < model.GetImportedMaterials().size(); i++)
        //	m_materialPaths[i] = defaultMaterial.GetPath();
        //
        // m_materialCount = model.GetImportedMaterials().size();
    }

    void ModelRendererComponent::RemoveModel(ECS::Entity parent)
    {
        // ECS::Registry* reg = ECS::Registry::Get();
        // ModelRendererComponent* modelRendererInEntity = reg->try_get<ModelRendererComponent>(parent);
        // if (this != modelRendererInEntity)
        //{
        //	LINA_ERR("You have to pass the same entity that this model component is attached to in order to remove the model data.");
        //	return;
        //}
        //
        //// Make sure the hierarchy is clear.
        // RemoveUnprocessedChildren(parent);
        // reg->DestroyAllChildren(parent);
        // m_processedChildren.clear();
        //
        // m_modelID = -1;
        // m_modelParamsPath = "";
        // m_modelPath = "";
        // m_materialPaths.clear();
    }

    void ModelRendererComponent::SetMaterial(ECS::Entity parent, int materialIndex, const Graphics::Material& material)
    {
        // ECS::Registry* reg = ECS::Registry::Get();
        //
        // ModelRendererComponent* modelRendererInEntity = reg->try_get<ModelRendererComponent>(parent);
        // if (this != modelRendererInEntity)
        //{
        //	LINA_ERR("You have to pass the same entity that this model component is attached to in order to set the material data. {0}", material.GetPath());
        //	return;
        //}
        //
        //// Set the path
        // m_materialPaths[materialIndex] = material.GetPath();
        //
        //// Find the mesh that points to this material index.
        // auto& model = Graphics::Model::GetModel(m_modelID);
        // ECS::EntityDataComponent& data = reg->get<ECS::EntityDataComponent>(parent);
        //
        // for (auto child : data.m_children)
        //{
        //	MeshRendererComponent* meshRenderer = reg->try_get<MeshRendererComponent>(child);
        //
        //	if (meshRenderer != nullptr)
        //	{
        //		for (uint32 i = 0; i < meshRenderer->m_subMeshes.size(); i++)
        //		{
        //			auto& mesh = model.GetMeshes()[meshRenderer->m_subMeshes[i]];
        //			if (materialIndex == mesh.GetMaterialSlotIndex())
        //			{
        //				meshRenderer->m_materialID = material.GetID();
        //				meshRenderer->m_materialPath = material.GetPath();
        //			}
        //		}
        //	}
        //}
    }

    void ModelRendererComponent::RemoveMaterial(ECS::Entity parent, int materialIndex)
    {
        // ECS::Registry* reg = ECS::Registry::Get();
        //
        // ModelRendererComponent* modelRendererInEntity = reg->try_get<ModelRendererComponent>(parent);
        // if (this != modelRendererInEntity)
        //{
        //	LINA_ERR("You have to pass the same entity that this model component is attached to in order to remove the material data. {0}", materialIndex);
        //	return;
        //}
        //
        // m_materialPaths[materialIndex] = "";
        //
        //// Find the mesh that points to this material index.
        // auto& model = Graphics::Model::GetModel(m_modelID);
        // ECS::EntityDataComponent& data = reg->get<ECS::EntityDataComponent>(parent);
        //
        // for (auto child : data.m_children)
        //{
        //	MeshRendererComponent* meshRenderer = reg->try_get<MeshRendererComponent>(child);
        //
        //	if (meshRenderer != nullptr)
        //	{
        //		for (uint32 i = 0; i < meshRenderer->m_subMeshes.size(); i++)
        //		{
        //			auto& mesh = model.GetMeshes()[meshRenderer->m_subMeshes[i]];
        //			if (materialIndex == mesh.GetMaterialSlotIndex())
        //				meshRenderer->m_materialID = -1;
        //		}
        //	}
        //}
    }

    void ModelRendererComponent::RefreshHierarchy(ECS::Entity parent)
    {
        // if (!Graphics::Model::ModelExists(m_modelID)) return;
        // ECS::Registry* reg = ECS::Registry::Get();
        // ECS::EntityDataComponent& data = reg->get<ECS::EntityDataComponent>(parent);
        //
        // RemoveUnprocessedChildren(parent);
        // reg->DestroyAllChildren(parent);
        // m_processedChildren.clear();
        //
        // auto& model = Graphics::Model::GetModel(m_modelID);
        // ProcessNode(parent, data.ToMatrix(), model.GetRoot(), model, true);
    }

} // namespace Lina::ECS
