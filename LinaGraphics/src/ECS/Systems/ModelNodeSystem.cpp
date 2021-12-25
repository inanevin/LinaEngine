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

#include "ECS/Systems/ModelNodeSystem.hpp"
#include "ECS/Registry.hpp"
#include "Rendering/RenderConstants.hpp"
#include "ECS/Components/EntityDataComponent.hpp"
#include "ECS/Components/ModelRendererComponent.hpp"
#include "Rendering/Model.hpp"
#include "Core/RenderEngineBackend.hpp"
#include "Core/RenderDeviceBackend.hpp"
#include "Rendering/Material.hpp"
#include "Animation/Skeleton.hpp"
#include "EventSystem/EventSystem.hpp"
#include "ECS/Components/ModelNodeComponent.hpp"
#include "Utility/UtilityFunctions.hpp"

namespace Lina::ECS
{
	void ModelNodeSystem::Initialize(ApplicationMode appMode)
	{
		System::Initialize();
		m_appMode = appMode;
		m_renderEngine = Graphics::RenderEngineBackend::Get();
		m_renderDevice = m_renderEngine->GetRenderDevice();
		m_ecs->on_destroy<ModelRendererComponent>().connect<&ModelNodeSystem::OnModelRendererRemoved>(this);
	}

	void ModelNodeSystem::ConstructEntityHierarchy(Entity entity, Graphics::ModelNode* node)
	{
		const auto& meshes = node->GetMeshes();

		if (meshes.size() > 0)
		{
			ModelNodeComponent& nodeComponent = m_ecs->emplace<ModelNodeComponent>(entity);
			nodeComponent.m_modelNode = node;
			nodeComponent.m_materialIDs.resize(meshes.size());
		}

		for (uint32 i = 0; i < node->m_children.size(); i++)
		{
			Entity childEntity = m_ecs->CreateEntity(node->m_children[i]->m_name);
			m_ecs->AddChildToEntity(entity, childEntity);
			ConstructEntityHierarchy(childEntity, node->m_children[i]);
		}
	}

	void ModelNodeSystem::CreateModelHierarchy(Graphics::Model& model)
	{
		Graphics::ModelNode& root = model.GetRootNode();
		const std::string parentEntityName = Utility::GetFileWithoutExtension(Utility::GetFileNameOnly(model.GetPath()));
		Entity parentEntity = m_ecs->CreateEntity(parentEntityName);
		ConstructEntityHierarchy(parentEntity, &root);
	}

	static float t = 0.0f;

	void ModelNodeSystem::UpdateComponents(float delta)
	{
		auto view = m_ecs->view<EntityDataComponent, ModelNodeComponent>();

		t += 0.016f;

		if (t > 3.0f)
		{
			t = -1000;
			Graphics::Model& model = Graphics::Model::GetModel(StringID("Resources/Sandbox/Target/RicochetTarget.fbx").value());
			CreateModelHierarchy(model);
		}
		for (auto entity : view)
		{
			ModelNodeComponent& nodeComponent = view.get<ModelNodeComponent>(entity);
			if (!nodeComponent.m_isEnabled) return;

			auto& data = view.get<EntityDataComponent>(entity);
			auto* node = nodeComponent.m_modelNode;
			auto& meshes = node->GetMeshes();

			const Matrix finalMatrix = data.ToMatrix();

			for (uint32 i = 0; i < meshes.size(); i++)
			{
				auto* mesh = meshes[i];
				uint32 materialSlot = mesh->GetMaterialSlotIndex();

				// Check if material exists.
				const StringIDType materialSID = nodeComponent.m_materialIDs[i];
				if (!Graphics::Material::MaterialExists(materialSID)) continue;

				// Render the material & vertex array.
				Graphics::Material& mat = Graphics::Material::GetMaterial(materialSID);

				if(mat.GetSurfaceType() == Graphics::MaterialSurfaceType::Opaque)
					RenderOpaque(mesh->GetVertexArray(), Graphics::Skeleton(), mat, finalMatrix);
				else
				{
					float priority = (m_renderEngine->GetCameraSystem()->GetCameraLocation() - data.GetLocation()).MagnitudeSqrt();
					RenderTransparent(mesh->GetVertexArray(), Graphics::Skeleton(), mat, finalMatrix, priority);
				}
			}
		}

	}

	void ModelNodeSystem::RenderOpaque(Graphics::VertexArray& vertexArray, Lina::Graphics::Skeleton& skeleton, Graphics::Material& material, const Matrix& transformIn)
	{
		// Render commands basically add the necessary
		// draw data into the maps/lists etc.
		Graphics::BatchDrawData drawData;
		drawData.m_vertexArray = &vertexArray;
		drawData.m_material = &material;
		m_opaqueRenderBatch[drawData].m_models.push_back(transformIn);

		if (skeleton.IsLoaded())
		{

		}

	}

	void ModelNodeSystem::RenderTransparent(Graphics::VertexArray& vertexArray, Lina::Graphics::Skeleton& skeleton, Graphics::Material& material, const Matrix& transformIn, float priority)
	{
		// Render commands basically add the necessary
		// draw data into the maps/lists etc.
		Graphics::BatchDrawData drawData;
		drawData.m_vertexArray = &vertexArray;
		drawData.m_material = &material;
		drawData.m_distance = priority;

		Graphics::BatchModelData modelData;
		modelData.m_models.push_back(transformIn);

		if (skeleton.IsLoaded())
		{

		}
	}

	void ModelNodeSystem::FlushOpaque(Graphics::DrawParams& drawParams, Graphics::Material* overrideMaterial, bool completeFlush)
	{
		// When flushed, all the data is delegated to the render device to do the actual
		// drawing. Then the data is cleared if complete flush is requested.

		for (std::map<Graphics::BatchDrawData, Graphics::BatchModelData>::iterator it = m_opaqueRenderBatch.begin(); it != m_opaqueRenderBatch.end(); ++it)
		{
			// Get references.
			Graphics::BatchDrawData drawData = it->first;
			Graphics::BatchModelData& modelData = it->second;
			size_t numTransforms = modelData.m_models.size();
			if (numTransforms == 0) continue;

			Graphics::VertexArray* vertexArray = drawData.m_vertexArray;
			Matrix* models = &modelData.m_models[0];

			// Get the material for drawing, object's own material or overriden material.
			Graphics::Material* mat = overrideMaterial == nullptr ? drawData.m_material : overrideMaterial;

			// Update the buffer w/ each transform.
			vertexArray->UpdateBuffer(7, models, numTransforms * sizeof(Matrix));

			if (modelData.m_boneTransformations.size() == 0)
				mat->SetBool(UF_BOOL_SKINNED, false);
			else
				mat->SetBool(UF_BOOL_SKINNED, true);

			for (int i = 0; i < modelData.m_boneTransformations.size(); i++)
				mat->SetMatrix4(std::string(UF_BONE_MATRICES) + "[" + std::to_string(i) + "]", modelData.m_boneTransformations[i]);

			m_renderEngine->UpdateShaderData(mat);

			m_renderDevice->Draw(vertexArray->GetID(), drawParams, (uint32)numTransforms, vertexArray->GetIndexCount(), false);

			// Clear the buffer.
			if (completeFlush)
			{
				modelData.m_models.clear();
				modelData.m_boneTransformations.clear();
			}
		}
	}

	void ModelNodeSystem::FlushTransparent(Graphics::DrawParams& drawParams, Graphics::Material* overrideMaterial, bool completeFlush)
	{
		// When flushed, all the data is delegated to the render device to do the actual
		// drawing. Then the data is cleared if complete flush is requested.

		// Empty out the queue
		while (!m_transparentRenderBatch.empty())
		{
			BatchPair pair = m_transparentRenderBatch.top();

			Graphics::BatchDrawData& drawData = std::get<0>(pair);
			Graphics::BatchModelData& modelData = std::get<1>(pair);
			size_t numTransforms = modelData.m_models.size();
			if (numTransforms == 0) continue;

			Graphics::VertexArray* vertexArray = drawData.m_vertexArray;
			Matrix* models = &modelData.m_models[0];

			// Get the material for drawing, object's own material or overriden material.
			Graphics::Material* mat = overrideMaterial == nullptr ? drawData.m_material : overrideMaterial;

			// Draw call.
			// Update the buffer w/ each transform.
			vertexArray->UpdateBuffer(7, models, numTransforms * sizeof(Matrix));

			if (modelData.m_boneTransformations.size() == 0)
				mat->SetBool(UF_BOOL_SKINNED, false);
			else
				mat->SetBool(UF_BOOL_SKINNED, true);

			for (int i = 0; i < modelData.m_boneTransformations.size(); i++)
				mat->SetMatrix4(std::string(UF_BONE_MATRICES) + "[" + std::to_string(i) + "]", modelData.m_boneTransformations[i]);
			m_renderEngine->UpdateShaderData(mat);
			m_renderDevice->Draw(vertexArray->GetID(), drawParams, (uint32)numTransforms, vertexArray->GetIndexCount(), false);

			// Clear the buffer.
			if (completeFlush)
			{
				modelData.m_models.clear();
				modelData.m_boneTransformations.clear();
			}

			m_transparentRenderBatch.pop();
		}

	}


	void ModelNodeSystem::OnModelRendererRemoved(entt::registry& reg, entt::entity ent)
	{
		// m_ecs->DestroyAllChildren(ent);
	}

}