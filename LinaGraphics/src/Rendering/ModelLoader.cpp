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
	bool ModelLoader::LoadModel(const std::string& fileName, std::vector<IndexedModel>& models, std::vector<uint32>& modelMaterialIndices, std::vector<ModelMaterial>& materials, MeshParameters meshParams)
	{
		// Get the importer & set assimp scene.
		Assimp::Importer importer;
		uint32 importFlags = 0;
		if (meshParams.calculateTangentSpace)
			importFlags |= aiProcess_CalcTangentSpace;

		if (meshParams.triangulate)
			importFlags |= aiProcess_Triangulate;

		if (meshParams.smoothNormals)
			importFlags |= aiProcess_GenSmoothNormals;

		const aiScene* scene = importer.ReadFile(fileName.c_str(), importFlags);
		// | aiProcess_FlipUVs
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
			currentModel.AllocateElement(3, true); // Positions
			currentModel.AllocateElement(2, true); // TexCoords
			currentModel.AllocateElement(3, true); // Normals
			currentModel.AllocateElement(3, true); // Tangents
			currentModel.AllocateElement(3, true); // Bitangents
			//currentModel.AllocateElement(3, false); // Joint IDs
			//currentModel.AllocateElement(3, true); // Weights
			currentModel.SetStartIndex(5); // Begin instanced data
			currentModel.AllocateElement(16, true); // Model Matrix
			currentModel.AllocateElement(16, true); // Inverse transpose matrix

			const aiVector3D aiZeroVector(0.0f, 0.0f, 0.0f);

			// Iterate through vertices.
			for (uint32 i = 0; i < model->mNumVertices; i++)
			{
				// Get array references from the current model on stack.
				const aiVector3D pos = model->mVertices[i];
				const aiVector3D normal = model->HasNormals() ? model->mNormals[i] : aiZeroVector;
				const aiVector3D texCoord = model->HasTextureCoords(0) ? model->mTextureCoords[0][i] : aiZeroVector;
				const aiVector3D tangent = model->HasTangentsAndBitangents() ? model->mTangents[i] : aiZeroVector;
				const aiVector3D biTangent = model->HasTangentsAndBitangents() ? model->mBitangents[i] : aiZeroVector;


				// Set model vertex data.
				currentModel.AddElement(0, pos.x, pos.y, pos.z);
				currentModel.AddElement(1, texCoord.x, texCoord.y);
				currentModel.AddElement(2, normal.x, normal.y, normal.z);
				currentModel.AddElement(3, tangent.x, tangent.y, tangent.z);
				currentModel.AddElement(4, biTangent.x, biTangent.y, biTangent.z);
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
				spec.m_textureNames["diffuse"] = str;
			}
			// Push the material to list.
			materials.push_back(spec);
		}

		return true;
	}

	bool ModelLoader::LoadModelAnimated(const std::string& fileName, std::vector<IndexedModel>& models, std::vector<uint32>& modelMaterialIndices, std::vector<ModelMaterial>& materials)
	{
		// Get the importer & set assimp scene.
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(fileName.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);
		// | aiProcess_FlipUVs
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
			currentModel.AllocateElement(3, true); // Positions
			currentModel.AllocateElement(2, true); // TexCoords
			currentModel.AllocateElement(3, true); // Normals
			currentModel.AllocateElement(3, true); // Tangents
			currentModel.AllocateElement(3, true); // Bitangents
			currentModel.SetStartIndex(5); // Begin instanced data
			currentModel.AllocateElement(16, true); // Model Matrix
			currentModel.AllocateElement(16, true); // Inverse transpose matrix


			const aiVector3D aiZeroVector(0.0f, 0.0f, 0.0f);

			// Iterate through vertices.
			for (uint32 i = 0; i < model->mNumVertices; i++)
			{
				// Get array references from the current model on stack.
				const aiVector3D pos = model->mVertices[i];
				const aiVector3D normal = model->HasNormals() ? model->mNormals[i] : aiZeroVector;
				const aiVector3D texCoord = model->HasTextureCoords(0) ? model->mTextureCoords[0][i] : aiZeroVector;
				const aiVector3D tangent = model->HasTangentsAndBitangents() ? model->mTangents[i] : aiZeroVector;
				const aiVector3D biTangent = model->HasTangentsAndBitangents() ? model->mBitangents[i] : aiZeroVector;

				// Set model vertex data.
				currentModel.AddElement(0, pos.x, pos.y, pos.z);
				currentModel.AddElement(1, texCoord.x, texCoord.y);
				currentModel.AddElement(2, normal.x, normal.y, normal.z);
				currentModel.AddElement(3, tangent.x, tangent.y, tangent.z);
				currentModel.AddElement(4, biTangent.x, biTangent.y, biTangent.z);
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
				spec.m_textureNames["diffuse"] = str;
			}
			// Push the material to list.
			materials.push_back(spec);
		}

		return true;
	}

	bool ModelLoader::LoadQuad(IndexedModel& currentModel)
	{
		// Create and indexed model for each mesh & fill in the data.
		currentModel.AllocateElement(3, true); // Positions
		currentModel.AllocateElement(2, true); // TexCoords
		currentModel.SetStartIndex(2); // Begin instanced data
		currentModel.AllocateElement(16, true); // Model Matrix
		currentModel.AllocateElement(16, true); // Inverse transpose matrix

		Vector3 vertices[] = {
			Vector3(-0.5f, 0.5f, 0.0f),  // left top, id 0
			Vector3(-0.5f, -0.5f, 0.0f), // left bottom, id 1
			Vector3(0.5f, -0.5f, 0.0f),  // right bottom, id 2
			Vector3(0.5f, 0.5f, 0.0f),   // right top, id 3
		};

		int indices[] = {
			// Left bottom triangle
			0, 1, 2,
			// Right top triangle
			2, 3, 0
		};

		// Tex coords.
		Vector2 texCoords[] = {
			Vector2(1.0f, 1.0f),  // left top, id 0
			Vector2(1.0f, 0.0f), // left bottom, id 1
			Vector2(0.0f, 0.0f),  // right bottom, id 2
			Vector2(0.0f, 1.0f),   // right top, id 3
		};


		// Set model vertex data.
		for (int i = 0; i < 4; i++)
		{
			currentModel.AddElement(0, vertices[i].x, vertices[i].y, vertices[i].z);
			currentModel.AddElement(1, texCoords[i].x, texCoords[i].y);
		}
		
		// Add indices.
		currentModel.AddIndices(indices[0], indices[1], indices[2]);
		currentModel.AddIndices(indices[3], indices[4], indices[5]);

		return true;
	}

	bool ModelLoader::LoadPrimitive(std::vector<IndexedModel>& models, int vertexSize, int indicesSize, float* vertices, int* indices, float* texCoords)
	{
		// Create and indexed model for each mesh & fill in the data.
		IndexedModel currentModel;
		currentModel.AllocateElement(3, true); // Positions
		currentModel.AllocateElement(2, true); // TexCoords
		currentModel.AllocateElement(3, true); // Normals
		currentModel.AllocateElement(3, true); // Tangents
		currentModel.SetStartIndex(4); // Begin instanced data
		currentModel.AllocateElement(16, true); // Model Matrix
		currentModel.AllocateElement(16, true); // Inverse transpose matrix


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
				currentModel.AddIndices(indices[i], indices[i + 1], indices[i + 2]);
		}

		// Add model to array.
		models.push_back(currentModel);
		return true;
	}
}

