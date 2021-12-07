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

#include "ECS/Systems/MeshRendererSystem.hpp"
#include "ECS/Components/TransformComponent.hpp"
#include "ECS/Components/MeshRendererComponent.hpp"
#include "Rendering/Mesh.hpp"
#include "Rendering/RenderEngine.hpp"
#include "Rendering/Material.hpp"
#include "Animation/Skeleton.hpp"

namespace LinaEngine::ECS
{

	Matrix OZZToMatrix(ozz::math::Float4x4& model)
	{

		Matrix mat;
#ifdef OZZ_SIMD_SSEx
		mat[0][0] = model.cols[0].m128_f32[0];	mat[0][1] = model.cols[0].m128_f32[1];	mat[0][2] = model.cols[0].m128_f32[2];	mat[0][3] = model.cols[0].m128_f32[3];
		mat[1][0] = model.cols[1].m128_f32[0];	mat[1][1] = model.cols[1].m128_f32[1];	mat[1][2] = model.cols[1].m128_f32[2];	mat[1][3] = model.cols[1].m128_f32[3];
		mat[2][0] = model.cols[2].m128_f32[0];	mat[2][1] = model.cols[2].m128_f32[1];	mat[2][2] = model.cols[2].m128_f32[2];	mat[2][3] = model.cols[2].m128_f32[3];
		mat[3][0] = model.cols[3].m128_f32[0];	mat[3][1] = model.cols[3].m128_f32[1];	mat[3][2] = model.cols[3].m128_f32[2];	mat[3][3] = model.cols[3].m128_f32[3];
#else
		mat[0][0] = model.cols[0].x;	mat[0][1] = model.cols[0].y;	mat[0][2] = model.cols[0].z;	mat[0][3] = model.cols[0].w;
		mat[1][0] = model.cols[1].x;	mat[1][1] = model.cols[1].y;	mat[1][2] = model.cols[1].z;	mat[1][3] = model.cols[1].w;
		mat[2][0] = model.cols[2].x;	mat[2][1] = model.cols[2].y;	mat[2][2] = model.cols[2].z;	mat[2][3] = model.cols[2].w;
		mat[3][0] = model.cols[3].x;	mat[3][1] = model.cols[3].y;	mat[3][2] = model.cols[3].z;	mat[3][3] = model.cols[3].w;
#endif

		return mat;
	}

	void MeshRendererSystem::UpdateComponents(float delta)
	{
		auto view = m_ecs->view<TransformComponent, MeshRendererComponent>();

		for (auto entity : view)
		{
			MeshRendererComponent& renderer = view.get<MeshRendererComponent>(entity);
			if (!renderer.m_isEnabled || renderer.m_excludeFromDrawList || renderer.m_materialID < 0 || renderer.m_meshID < 0) continue;

			TransformComponent& transform = view.get<TransformComponent>(entity);

			// We get the materials, then according to their surface types we add the mesh
			// data into either opaque queue or the transparent queue.
			Graphics::Material& mat = LinaEngine::Graphics::Material::GetMaterial(renderer.m_materialID);
			Graphics::Mesh& mesh = LinaEngine::Graphics::Mesh::GetMesh(renderer.m_meshID);

			if (mat.GetSurfaceType() == Graphics::MaterialSurfaceType::Opaque)
			{
				for (int i = 0; i < mesh.GetVertexArrays().size(); i++)
					RenderOpaque(*mesh.GetVertexArray(i), mesh.GetSkeleton(), mat, transform.transform.ToMatrix());
			}
			else
			{
				// Transparent queue is a priority queue unlike the opaque one, so we set the priority as distance to the camera.
				float priority = (m_renderEngine->GetCameraSystem()->GetCameraLocation() - transform.transform.GetLocation()).MagnitudeSqrt();

				for (int i = 0; i < mesh.GetVertexArrays().size(); i++)
					RenderTransparent(*mesh.GetVertexArray(i), mesh.GetSkeleton(), mat, transform.transform.ToMatrix(), priority);
			}
		}

	}

	void MeshRendererSystem::RenderOpaque(Graphics::VertexArray& vertexArray, LinaEngine::Graphics::Skeleton& skeleton, Graphics::Material& material, const Matrix& transformIn)
	{
		// Render commands basically add the necessary
		// draw data into the maps/lists etc.
		Graphics::BatchDrawData drawData;
		drawData.m_vertexArray = &vertexArray;
		drawData.m_material = &material;
		m_opaqueRenderBatch[drawData].m_models.push_back(transformIn);
		m_opaqueRenderBatch[drawData].m_inverseTransposeModels.push_back(transformIn.Transpose().Inverse());

		if (skeleton.IsLoaded())
		{
			auto& models = skeleton.GetModels();

			for (auto& model : models)
			{
				Matrix mat = OZZToMatrix(model);
				m_opaqueRenderBatch[drawData].m_boneTransformations.push_back(mat);
			}
		}


	}

	void MeshRendererSystem::RenderTransparent(Graphics::VertexArray& vertexArray, LinaEngine::Graphics::Skeleton& skeleton, Graphics::Material& material, const Matrix& transformIn, float priority)
	{
		// Render commands basically add the necessary
		// draw data into the maps/lists etc.
		Graphics::BatchDrawData drawData;
		drawData.m_vertexArray = &vertexArray;
		drawData.m_material = &material;
		drawData.m_distance = priority;

		Graphics::BatchModelData modelData;
		modelData.m_models.push_back(transformIn);
		modelData.m_inverseTransposeModels.push_back(transformIn.Transpose().Inverse());

		if (skeleton.IsLoaded())
		{
			auto& models = skeleton.GetModels();

			for (auto& model : models)
			{
				Matrix mat = OZZToMatrix(model);
				modelData.m_boneTransformations.push_back(mat);
			}
		}

		m_transparentRenderBatch.emplace(std::make_pair(drawData, modelData));

	}

	void MeshRendererSystem::FlushOpaque(Graphics::DrawParams& drawParams, Graphics::Material* overrideMaterial, bool completeFlush)
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
			Matrix* inverseTransposeModels = &modelData.m_inverseTransposeModels[0];

			// Get the material for drawing, object's own material or overriden material.
			Graphics::Material* mat = overrideMaterial == nullptr ? drawData.m_material : overrideMaterial;

			// Draw call.
			// Update the buffer w/ each transform.
			vertexArray->UpdateBuffer(7, models, numTransforms * sizeof(Matrix));
			vertexArray->UpdateBuffer(8, inverseTransposeModels, numTransforms * sizeof(Matrix));

			if (modelData.m_boneTransformations.size() == 0)
				mat->SetBool(UF_BOOL_SKINNED, false);
			else
				mat->SetBool(UF_BOOL_SKINNED, true);

			for (int i = 0; i < modelData.m_boneTransformations.size(); i++)
				mat->SetMatrix4(std::string(UF_BONE_MATRICES) + "[" + std::to_string(i) + "]", modelData.m_boneTransformations[i]);

			m_renderEngine->UpdateShaderData(mat);
			s_renderDevice->Draw(vertexArray->GetID(), drawParams, numTransforms, vertexArray->GetIndexCount(), false);

			// Clear the buffer.
			if (completeFlush)
			{
				modelData.m_models.clear();
				modelData.m_inverseTransposeModels.clear();
				modelData.m_boneTransformations.clear();
			}
		}
	}

	void MeshRendererSystem::FlushSingleRenderer(ECS::MeshRendererComponent& mrc, ECS::TransformComponent& tr, Graphics::DrawParams drawParams)
	{
		if (!Graphics::Mesh::MeshExists(mrc.m_meshID) || !Graphics::Material::MaterialExists(mrc.m_materialID))
		{
			LINA_CORE_WARN("Mesh or material does not exists for this renderer, aborting single flush.");
			return;
		}


		Graphics::Mesh& mesh = Graphics::Mesh::GetMesh(mrc.m_meshID);
		Graphics::Material& mat = Graphics::Material::GetMaterial(mrc.m_materialID);

		for (Graphics::VertexArray* va : mesh.GetVertexArrays())
		{
			const Matrix model = tr.transform.ToMatrix();
			va->UpdateBuffer(7, &model[0][0], sizeof(Matrix));
			va->UpdateBuffer(8, &tr.transform.ToMatrix().Inverse().Transpose()[0][0], sizeof(Matrix));
			m_renderEngine->UpdateShaderData(&mat);
			s_renderDevice->Draw(va->GetID(), drawParams, 1, va->GetIndexCount(), false);
		}

	}

	void MeshRendererSystem::FlushTransparent(Graphics::DrawParams& drawParams, Graphics::Material* overrideMaterial, bool completeFlush)
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
			Matrix* inverseTransposeModels = &modelData.m_inverseTransposeModels[0];

			// Get the material for drawing, object's own material or overriden material.
			Graphics::Material* mat = overrideMaterial == nullptr ? drawData.m_material : overrideMaterial;

			// Draw call.
			// Update the buffer w/ each transform.
			vertexArray->UpdateBuffer(7, models, numTransforms * sizeof(Matrix));
			vertexArray->UpdateBuffer(8, inverseTransposeModels, numTransforms * sizeof(Matrix));

			if (modelData.m_boneTransformations.size() == 0)
				mat->SetBool(UF_BOOL_SKINNED, false);
			else
				mat->SetBool(UF_BOOL_SKINNED, true);

			for (int i = 0; i < modelData.m_boneTransformations.size(); i++)
				mat->SetMatrix4(std::string(UF_BONE_MATRICES) + "[" + std::to_string(i) + "]", modelData.m_boneTransformations[i]);
			m_renderEngine->UpdateShaderData(mat);
			s_renderDevice->Draw(vertexArray->GetID(), drawParams, numTransforms, vertexArray->GetIndexCount(), false);

			// Clear the buffer.
			if (completeFlush)
			{
				modelData.m_models.clear();
				modelData.m_inverseTransposeModels.clear();
				modelData.m_boneTransformations.clear();
			}

			m_transparentRenderBatch.pop();
		}

	}

}