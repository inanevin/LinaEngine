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

	void AddBoneData(VertexBone* data, uint32 boneID, float weight)
	{
		for (uint32 i = 0; i < NUM_BONES_PER_VERTEX; i++)
		{
			if (data->m_weights[i] == 0.0f)
			{
				data->m_ids[i] = boneID;
				data->m_weights[i] = weight;
				return;
			}
		}

		LINA_CORE_ERR("Reached to the end of max bones per vertex!");
	}

	Matrix AssimpToInternal(aiMatrix4x4 aiMat)
	{
		Matrix mat;
		mat[0][0] = aiMat.a1;	mat[0][1] = aiMat.b1;	mat[0][2] = aiMat.c1;	mat[0][3] = aiMat.d1;
		mat[1][0] = aiMat.a2;	mat[1][1] = aiMat.b2;	mat[1][2] = aiMat.c2;	mat[1][3] = aiMat.d2;
		mat[2][0] = aiMat.a3;	mat[2][1] = aiMat.b3;	mat[2][2] = aiMat.c3;	mat[2][3] = aiMat.d3;
		mat[3][0] = aiMat.a4;	mat[3][1] = aiMat.b4;	mat[3][2] = aiMat.c4;	mat[3][3] = aiMat.d4;
		return mat;
	}

	bool ModelLoader::LoadModel(const std::string& fileName, std::vector<IndexedModel>& models, std::vector<uint32>& modelMaterialIndices, std::vector<ModelMaterial>& materials, Skeleton& skeleton, MeshParameters meshParams, MeshSceneParameters* worldParams)
	{
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

		if (worldParams != nullptr && scene->mRootNode->mNumChildren > 0)
		{
			aiVector3D ps, pr, pos;
			scene->mRootNode->mChildren[0]->mTransformation.Decompose(ps, pr, pos);
			worldParams->m_worldPosition = Vector3(pos.x, pos.y, pos.z);
			worldParams->m_worldRotation = Quaternion::Euler(pr.x, pr.y, pr.z);
			worldParams->m_worldScale = Vector3(ps.x, ps.y, ps.z);
		}

		aiMatrix4x4 rootTransformation = scene->mRootNode->mTransformation;
		worldParams->m_rootInverse = AssimpToInternal(rootTransformation).Inverse();

		const std::string runningDirectory = Utility::GetRunningDirectory();
		const std::string fileExt = Utility::GetFileExtension(fileName);
		const bool isFBX = fileExt.compare("fbx") == 0;

		// Load animations if fbx
		if (isFBX && scene->HasAnimations())
		{
			// Create .ozz files out of FBX.
			const std::string meshPath = runningDirectory + "\\" + fileName;
			const std::string meshName = Utility::GetFileNameOnly(Utility::GetFileWithoutExtension(fileName));
			const std::string meshFolder = runningDirectory + "\\" + Utility::GetFilePath(fileName);
			const std::string cdToBin = "cd " + runningDirectory + "\\resources\\engine\\bin";
			const std::string command = cdToBin + "&& fbx2ozz.exe --file=" + meshPath;
			system(command.c_str());

			// Make directory in the original fbx file dir.
			const std::string makeDirCommand = "cd " + meshFolder + " && mkdir " + meshName;
			system(makeDirCommand.c_str());
			LINA_CORE_ERR("command {0}", makeDirCommand);

			// Copy the ozz files to the newly created directory.
			const std::string moveCommand = cdToBin + "&& move *.ozz " + meshFolder + "\\" + meshName;
			system(moveCommand.c_str());

			// Load skeleton.
			const std::string skelPath = Utility::GetFileWithoutExtension(fileName) + "/skeleton.ozz";
			skeleton.LoadSkeleton(skelPath);

			// Create animation structure for each ozz file
			std::string path(meshFolder + "/");
			std::string ext(".ozz");
			for (auto& p : std::filesystem::recursive_directory_iterator(path))
			{
				if (p.path().extension() == ext)
				{
					const std::string animName = p.path().stem().string();

					// Skip skeleton
					if (animName.compare("skeleton") == 0)
						continue;

					// Create runtime animation for each ozz
					const std::string path = Utility::GetFileWithoutExtension(fileName) + "/" + animName + ".ozz";
					Animation* anim = new Animation();
					const bool success = anim->LoadAnimation(path);

					if (success)
					{
						if (skeleton.GetSkeleton().num_joints() != anim->GetAnim().num_tracks())
						{
							LINA_CORE_ERR("Loaded animation does not match with the loaded skeleton. {0}", path);
							delete anim;
						}
						else
							skeleton.GetAnimations()[animName] = anim;

					}
					else
						delete anim;

				}
			}



		}


		// Iterate through the meshes on the scene.
		for (uint32 j = 0; j < scene->mNumMeshes; j++)
		{
			// Build model reference for each mesh.
			const aiMesh* model = scene->mMeshes[j];
			modelMaterialIndices.push_back(model->mMaterialIndex);

			// Build and indexed model for each mesh & fill in the data.
			IndexedModel currentModel;
			currentModel.AllocateElement(3, true); // Positions
			currentModel.AllocateElement(2, true); // TexCoords
			currentModel.AllocateElement(3, true); // Normals
			currentModel.AllocateElement(3, true); // Tangents
			currentModel.AllocateElement(3, true); // Bitangents
			currentModel.AllocateElement(4, false); // Bone ids
			currentModel.AllocateElement(4, true);	// bone weights
			currentModel.SetStartIndex(7); // Begin instanced data
			currentModel.AllocateElement(16, true); // Model Matrix
			currentModel.AllocateElement(16, true); // Inverse transpose matrix


			const aiVector3D aiZeroVector(0.0f, 0.0f, 0.0f);

			std::vector<std::vector<int>> vertexBoneIDs;
			std::vector<std::vector<float>> vertexBoneWeights;

			if (model->mNumBones > 0)
			{
				vertexBoneIDs.resize(model->mNumVertices, std::vector<int>(4, -1));
				vertexBoneWeights.resize(model->mNumVertices, std::vector<float>(4, 0.0f));
			}


			int boneCounter = 0;
			for (int boneIndex = 0; boneIndex < model->mNumBones; ++boneIndex)
			{
				int boneID = -1;
				std::string boneName = model->mBones[boneIndex]->mName.C_Str();
				if (worldParams->m_boneInfoMap.find(boneName) == worldParams->m_boneInfoMap.end())
				{
					BoneInfo newBoneInfo;
					newBoneInfo.m_id = boneCounter;
					newBoneInfo.m_offset = AssimpToInternal(model->mBones[boneIndex]->mOffsetMatrix);
					worldParams->m_boneInfoMap[boneName] = newBoneInfo;
					boneID = boneCounter;
					boneCounter++;
				}
				else
				{
					boneID = worldParams->m_boneInfoMap[boneName].m_id;
				}
				assert(boneID != -1);
				auto weights = model->mBones[boneID]->mWeights;
				int numWeights = model->mBones[boneID]->mNumWeights;

				for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
				{
					int vertexId = weights[weightIndex].mVertexId;
					float weight = weights[weightIndex].mWeight;
					LINA_CORE_ASSERT(vertexId <= vertices.size());
					SetVertexBoneData(vertexBoneIDs[vertexId], vertexBoneWeights[vertexId], boneID, weight);
					//LINA_CORE_TRACE("MESH {3}, Bone ID {0}, Vertex ID {1},  Bone Weight {2}", boneID, vertexId, weight, j);
					//LINA_CORE_TRACE("Setting data on vertex {0}, bone: {1}, weight{2}", vboneIDs[0], vboneIDs[1], vboneIDs[2], vboneIDs[3]);

				}
			}
			

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


				if (vertexBoneIDs.size() > 0)
				{
					const std::vector<int>& vboneIDs = vertexBoneIDs[i];
					const std::vector<float>& vboneWeights = vertexBoneWeights[i];
					//currentModel.AddElement(5, vboneIDs[0], vboneIDs[1], vboneIDs[2], vboneIDs[3]);
					//currentModel.AddElement(6, vboneWeights[0], vboneWeights[1], vboneWeights[2], vboneWeights[3]);
					currentModel.AddElement(5, 1, 3 ,7, 0);
					currentModel.AddElement(6, 0.05f, 0.4f, 2.0f, 1.0f);
					//LINA_CORE_ERR("BONE ID: {0}", vertexBoneIDs[i][0]);


					if (vboneIDs[0] > 100)
						LINA_CORE_ERR("BIGGER");
					if (vboneIDs[1] > 100)
						LINA_CORE_ERR("BIGGER");
					if (vboneIDs[2] > 100)
						LINA_CORE_ERR("BIGGER");
					if (vboneIDs[3] > 100)
						LINA_CORE_ERR("BIGGER");

					//LINA_CORE_TRACE("Bone IDs : {0}, {1}, {2}, {3}", vboneIDs[0], vboneIDs[1], vboneIDs[2], vboneIDs[3]);
					//LINA_CORE_TRACE("Bone Weights : {0}, {1}, {2}, {3}", vboneWeights[0], vboneWeights[1], vboneWeights[2], vboneWeights[3]);
				}
				else
				{
					currentModel.AddElement(5, -1, -1, -1, -1);
					currentModel.AddElement(6, 0.0f, 0.0f, 0.0f, 0.0f);
				}
			
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
			// Build material reference & material specifications.
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
		// Build and indexed model for each mesh & fill in the data.
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
		// Build and indexed model for each mesh & fill in the data.
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

