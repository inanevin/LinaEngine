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

#include "Rendering/ModelLoader.hpp"  
#include "Rendering/RenderingCommon.hpp"
#include "Rendering/Model.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <ozz/animation/offline/tools/import2ozz.h>
#include <Utility/UtilityFunctions.hpp>
#include <fstream>
#include <iostream>
#include <filesystem>

namespace LinaEngine::Graphics
{


	Matrix AssimpToInternal(aiMatrix4x4 aiMat)
	{
		Matrix mat;
		mat[0][0] = aiMat.a1;	mat[0][1] = aiMat.b1;	mat[0][2] = aiMat.c1;	mat[0][3] = aiMat.d1;
		mat[1][0] = aiMat.a2;	mat[1][1] = aiMat.b2;	mat[1][2] = aiMat.c2;	mat[1][3] = aiMat.d2;
		mat[2][0] = aiMat.a3;	mat[2][1] = aiMat.b3;	mat[2][2] = aiMat.c3;	mat[2][3] = aiMat.d3;
		mat[3][0] = aiMat.a4;	mat[3][1] = aiMat.b4;	mat[3][2] = aiMat.c4;	mat[3][3] = aiMat.d4;
		return mat;
	}

	bool ModelLoader::LoadModel(const std::string& fileName, Model& model, ModelParameters meshParams)
	{
		ModelSceneParameters& worldParams = model.GetWorldParameters();


		// Get the importer & set assimp scene.
		Assimp::Importer importer;
		uint32 importFlags = 0;
		if (meshParams.m_calculateTangentSpace)
			importFlags |= aiProcess_CalcTangentSpace;

		if (meshParams.m_triangulate)
			importFlags |= aiProcess_Triangulate;

		if (meshParams.m_smoothNormals)
			importFlags |= aiProcess_GenSmoothNormals;

		if (meshParams.m_flipUVs)
			importFlags |= aiProcess_FlipUVs;

		if (meshParams.m_flipWinding)
			importFlags |= aiProcess_FlipWindingOrder;


		const aiScene* scene = importer.ReadFile(fileName.c_str(), importFlags);


		if (!scene)
		{
			LINA_CORE_ERR("Mesh loading failed! {0}", fileName.c_str());
			return false;
		}

		if (scene->mRootNode->mNumChildren > 0)
		{
			aiVector3D ps, pr, pos;
			scene->mRootNode->mChildren[0]->mTransformation.Decompose(ps, pr, pos);
			worldParams.m_worldPosition = Vector3(pos.x, pos.y, pos.z);
			worldParams.m_worldRotation = Quaternion::Euler(pr.x, pr.y, pr.z);
			worldParams.m_worldScale = Vector3(ps.x, ps.y, ps.z);
		}

		aiMatrix4x4 rootTransformation = scene->mRootNode->mTransformation;
		worldParams.m_rootInverse = AssimpToInternal(rootTransformation).Inverse();

		const std::string runningDirectory = Utility::GetRunningDirectory();
		const std::string fileExt = Utility::GetFileExtension(fileName);
		const bool isFBX = fileExt.compare("fbx") == 0;

		

		// Iterate through the meshes on the scene.
		for (uint32 j = 0; j < scene->mNumMeshes; j++)
		{
			// Build aiMesh reference for each aiMesh.
			const aiMesh* aiMesh = scene->mMeshes[j];
		
			// Build and indexed aiMesh for each aiMesh & fill in the data.
			Mesh currentMesh;
			currentMesh.SetMaterialSlot(scene->mMeshes[j]->mMaterialIndex);
			currentMesh.AllocateElement(3, 0, true); // Positions
			currentMesh.AllocateElement(2, 1, true); // TexCoords
			currentMesh.AllocateElement(3, 2, true); // Normals
			currentMesh.AllocateElement(3, 3, true); // Tangents
			currentMesh.AllocateElement(3, 4, true); // Bitangents
			currentMesh.AllocateElement(4, 5, false); // Bone ids
			currentMesh.AllocateElement(4, 6, true);	// bone weights
			currentMesh.AllocateElement(16, 7, true, true); // Model Matrix
			currentMesh.AllocateElement(16, 11, true, true); // Inverse transpose matrix


			const aiVector3D aiZeroVector(0.0f, 0.0f, 0.0f);

			std::vector<std::vector<int>> vertexBoneIDs;
			std::vector<std::vector<float>> vertexBoneWeights;

			if (aiMesh->mNumBones > 0)
			{
				vertexBoneIDs.resize(aiMesh->mNumVertices, std::vector<int>(4, -1));
				vertexBoneWeights.resize(aiMesh->mNumVertices, std::vector<float>(4, 0.0f));
			}



			// Iterate through vertices.
			for (uint32 i = 0; i < aiMesh->mNumVertices; i++)
			{

				// Get array references from the current aiMesh on stack.
				const aiVector3D pos = aiMesh->mVertices[i];
				const aiVector3D normal = aiMesh->HasNormals() ? aiMesh->mNormals[i] : aiZeroVector;
				const aiVector3D texCoord = aiMesh->HasTextureCoords(0) ? aiMesh->mTextureCoords[0][i] : aiZeroVector;
				const aiVector3D tangent = aiMesh->HasTangentsAndBitangents() ? aiMesh->mTangents[i] : aiZeroVector;
				const aiVector3D biTangent = aiMesh->HasTangentsAndBitangents() ? aiMesh->mBitangents[i] : aiZeroVector;

				// Set aiMesh vertex data.
				currentMesh.AddElement(0, pos.x, pos.y, pos.z);
				currentMesh.AddElement(1, texCoord.x, texCoord.y);
				currentMesh.AddElement(2, normal.x, normal.y, normal.z);
				currentMesh.AddElement(3, tangent.x, tangent.y, tangent.z);
				currentMesh.AddElement(4, biTangent.x, biTangent.y, biTangent.z);


				if (vertexBoneIDs.size() > 0)
				{
					const std::vector<int>& vboneIDs = vertexBoneIDs[i];
					const std::vector<float>& vboneWeights = vertexBoneWeights[i];
					currentMesh.AddElement(5, vboneIDs[0], vboneIDs[1], vboneIDs[2], vboneIDs[3]);
					currentMesh.AddElement(6, vboneWeights[0], vboneWeights[1], vboneWeights[2], vboneWeights[3]);
					//currentMesh.AddElement(5, 1, 3, 7, 0);
					//currentMesh.AddElement(6, 0.05f, 0.4f, 2.0f, 1.0f);
				}
				else
				{
					currentMesh.AddElement(5, -1, -1, -1, -1);
					currentMesh.AddElement(6, 0.0f, 0.0f, 0.0f, 0.0f);
				}
			}

			// Iterate through faces & add indices for each face.
			for (uint32 i = 0; i < aiMesh->mNumFaces; i++)
			{
				const aiFace& face = aiMesh->mFaces[i];
				LINA_CORE_ASSERT(face.mNumIndices == 3);
				currentMesh.AddIndices(face.mIndices[0], face.mIndices[1], face.mIndices[2]);
			}

			// Add aiMesh to array.
			model.GetMeshes().push_back(currentMesh);
		}

		// Iterate through the materials in the scene.
		for (uint32 i = 0; i < scene->mNumMaterials; i++)
		{
			// Build material reference & material specifications.
			const aiMaterial* material = scene->mMaterials[i];
			ModelMaterial spec;
			spec.m_name = scene->mMaterials[i]->GetName().C_Str();

			// Currently only handles diffuse textures.
			aiString texturePath;
			if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0 && material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) != AI_SUCCESS)
			{
				std::string str(texturePath.data);
				spec.m_textureNames["diffuse"] = str;
			}
			// Push the material to list.
			model.GetMaterialSpecs().push_back(spec);
		}
		if (model.GetMaterialSpecs().size() == 0)
		{
			ModelMaterial defaultSpec;
			defaultSpec.m_name = "Material";
			model.GetMaterialSpecs().push_back(defaultSpec);
		}

		return true;
	}



	bool ModelLoader::LoadQuad(Mesh& mesh)
	{
		// Build and indexed aiMesh for each aiMesh & fill in the data.
		mesh.AllocateElement(3, 0, true); // Positions
		mesh.AllocateElement(2, 1, true); // TexCoords
		mesh.AllocateElement(16, 7, true, true); // Model Matrix
		mesh.AllocateElement(16, 11, true, true); // Inverse transpose matrix

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


		// Set aiMesh vertex data.
		for (int i = 0; i < 4; i++)
		{
			mesh.AddElement(0, vertices[i].x, vertices[i].y, vertices[i].z);
			mesh.AddElement(1, texCoords[i].x, texCoords[i].y);
		}

		// Add indices.
		mesh.AddIndices(indices[0], indices[1], indices[2]);
		mesh.AddIndices(indices[3], indices[4], indices[5]);

		return true;
	}

	bool ModelLoader::LoadPrimitive(std::vector<Mesh>& meshes, int vertexSize, int indicesSize, float* vertices, int* indices, float* texCoords)
	{
		// Build and indexed aiMesh for each aiMesh & fill in the data.
		Mesh currentMesh;
		currentMesh.AllocateElement(3, 0, true); // Positions
		currentMesh.AllocateElement(2, 1, true); // TexCoords
		currentMesh.AllocateElement(3, 2, true); // Normals
		currentMesh.AllocateElement(3, 3, true); // Tangents

		currentMesh.AllocateElement(16, 7, true, true); // Model Matrix
		currentMesh.AllocateElement(16, 11, true, true); // Inverse transpose matrix


		const aiVector3D aiZeroVector(0.0f, 0.0f, 0.0f);

		// Iterate through vertices.
		for (uint32 i = 0; i < vertexSize; i++)
		{
			// Get array references from the current aiMesh on stack.
			const Vector3 pos = vertices[i];
			const Vector2 texCoord = texCoords[i];
			const Vector3 normal = Vector3::Zero;
			const Vector3 tangent = Vector3::Zero;
			const aiVector3D biTangent = aiZeroVector;
			// Set aiMesh vertex data.
			currentMesh.AddElement(0, pos.x, pos.y, pos.z);
			currentMesh.AddElement(1, texCoord.x, texCoord.y);
			currentMesh.AddElement(2, normal.x, normal.y, normal.z);
			currentMesh.AddElement(3, tangent.x, tangent.y, tangent.z);

		}

		// Iterate through faces & add indices for each face.
		for (uint32 i = 0; i < indicesSize; i++)
		{
			if (i % 3 == 0)
				currentMesh.AddIndices(indices[i], indices[i + 1], indices[i + 2]);
		}

		// Add aiMesh to array.
		meshes.push_back(currentMesh);
		return true;
	}
	void ModelLoader::SetVertexBoneData(std::vector<int>& vertexBoneIDs, std::vector<float>& vertexBoneWeights, int boneID, float weight)
	{
		for (int i = 0; i < 4; ++i)
		{
			if (vertexBoneIDs[i] < 0)
			{
				vertexBoneIDs[i] = boneID;
				vertexBoneWeights[i] = weight;
				break;
			}
		}
	}
}

