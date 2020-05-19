/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: ModelLoader
Timestamp: 4/26/2019 12:11:04 AM

*/

#include "Rendering/ModelLoader.hpp"  
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace LinaEngine::Graphics
{
	bool ModelLoader::LoadModels(const std::string& fileName, LinaArray<IndexedModel>& models, LinaArray<uint32>& modelMaterialIndices, LinaArray<ModelMaterial>& materials)
	{
		// Get the importer & set assimp scene.
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(fileName.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

		if (!scene)
		{
			LINA_CORE_ERR("Mesh loading failed! {0}", fileName.c_str());
			return false;
		}

		// Iterate through the meshes on the scene.
		for (uint32 j = 0; j < scene->mNumMeshes; j++)
		{
			// Create model reference for each mesh.
			const aiMesh* model = scene->mMeshes[j];
			modelMaterialIndices.push_back(model->mMaterialIndex);

			// Create and indexed model for each mesh & fill in the data.
			IndexedModel currentModel;
			currentModel.AllocateElement(3); // Positions
			currentModel.AllocateElement(2); // TexCoords
			currentModel.AllocateElement(3); // Normals
			currentModel.AllocateElement(3); // Tangents
			currentModel.SetStartIndex(4); // Begin instanced data
			currentModel.AllocateElement(16); // Model Matrix
			currentModel.AllocateElement(16); // Inverse transpose matrix


			const aiVector3D aiZeroVector(0.0f, 0.0f, 0.0f);

			// Iterate through vertices.
			for (uint32 i = 0; i < model->mNumVertices; i++)
			{
				// Get array references from the current model on stack.
				const aiVector3D pos = model->mVertices[i];
				const aiVector3D normal = model->HasNormals() ? model->mNormals[i] : aiZeroVector;
				const aiVector3D texCoord = model->HasTextureCoords(0) ? model->mTextureCoords[0][i] : aiZeroVector;
				const aiVector3D tangent = model->HasTangentsAndBitangents() ? model->mTangents[i] : aiZeroVector;

				// Set model vertex data.
				currentModel.AddElement(0, pos.x, pos.y, pos.z);
				currentModel.AddElement(1, texCoord.x, texCoord.y);
				currentModel.AddElement(2, normal.x, normal.y, normal.z);
				currentModel.AddElement(3, tangent.x, tangent.y, tangent.z);
			}

			// Iterate through faces & add indices for each face.
			for (uint32 i = 0; i < model->mNumFaces; i++)
			{
				const aiFace& face = model->mFaces[i];
				LINA_CORE_ASSERT(face.mNumIndices == 3);
				currentModel.AddIndices(face.mIndices[0], face.mIndices[1], face.mIndices[2]);
			}

			// Add model to array.
			models.push_back(currentModel);
		}

		// Iterate through the materials in the scene.
		for (uint32 i = 0; i < scene->mNumMaterials; i++)
		{
			// Create material reference & material specifications.
			const aiMaterial* material = scene->mMaterials[i];
			ModelMaterial spec;

			// Currently only handles diffuse textures.
			aiString texturePath;
			if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0 && material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) != AI_SUCCESS)
			{
				std::string str(texturePath.data);
				spec.textureNames["diffuse"] = str;
			}
			// Push the material to list.
			materials.push_back(spec);
		}

		return true;
	}


	bool ModelLoader::LoadPrimitive(LinaArray<IndexedModel>& models, int vertexSize, int indicesSize, float* vertices, int* indices, float* texCoords)
	{
		// Create and indexed model for each mesh & fill in the data.
		IndexedModel currentModel;
		currentModel.AllocateElement(3); // Positions
		currentModel.AllocateElement(2); // TexCoords
		currentModel.AllocateElement(3); // Normals
		currentModel.AllocateElement(3); // Tangents
		currentModel.SetStartIndex(4); // Begin instanced data
		currentModel.AllocateElement(16); // Model Matrix
		currentModel.AllocateElement(16); // Inverse transpose matrix


		const aiVector3D aiZeroVector(0.0f, 0.0f, 0.0f);

		// Iterate through vertices.
		for (uint32 i = 0; i < vertexSize; i++)
		{
			// Get array references from the current model on stack.
			const Vector3 pos = vertices[i];
			const Vector2 texCoord = texCoords[i];
			const Vector3 normal = Vector3::Zero;
			const Vector3 tangent = Vector3::Zero;

			// Set model vertex data.
			currentModel.AddElement(0, pos.x, pos.y, pos.z);
			currentModel.AddElement(1, texCoord.x, texCoord.y);
			currentModel.AddElement(2, normal.x, normal.y, normal.z);
			currentModel.AddElement(3, tangent.x, tangent.y, tangent.z);
		}

		// Iterate through faces & add indices for each face.
		for (uint32 i = 0; i < indicesSize; i++)
		{
			if (i % 3 == 0)
				currentModel.AddIndices(indices[i], indices[i + 1], indices[i+2]);
		}

		// Add model to array.
		models.push_back(currentModel);
		return true;
	}
}

