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

#include "Utility/ModelLoader.hpp"

#include "Log/Log.hpp"
#include "Math/Math.hpp"
#include "Rendering/Mesh.hpp"
#include "Rendering/Model.hpp"
#include "Rendering/RenderingCommon.hpp"
#include "Utility/AssimpUtility.hpp"
#include "Utility/UtilityFunctions.hpp"

#include <assimp/Importer.hpp>
#include <assimp/metadata.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace Lina::Graphics
{
    void ModelLoader::FillMeshData(const aiMesh* aiMesh, Mesh* linaMesh)
    {
        // Build and indexed aiMesh for each aiMesh & fill in the data.
        linaMesh->SetName(aiMesh->mName.C_Str());
        linaMesh->SetMaterialSlot(aiMesh->mMaterialIndex);
        linaMesh->AllocateElement(3, 0, true);        // Positions
        linaMesh->AllocateElement(2, 1, true);        // TexCoords
        linaMesh->AllocateElement(3, 2, true);        // Normals
        linaMesh->AllocateElement(3, 3, true);        // Tangents
        linaMesh->AllocateElement(3, 4, true);        // Bitangents
        linaMesh->AllocateElement(4, 5, false);       // Bone ids
        linaMesh->AllocateElement(4, 6, true);        // bone weights
        linaMesh->AllocateElement(16, 7, true, true); // Model Matrix

        const aiVector3D aiZeroVector(0.0f, 0.0f, 0.0f);

        std::vector<std::vector<int>>   vertexBoneIDs;
        std::vector<std::vector<float>> vertexBoneWeights;

        if (aiMesh->mNumBones > 0)
        {
            vertexBoneIDs.resize(aiMesh->mNumVertices, std::vector<int>(4, -1));
            vertexBoneWeights.resize(aiMesh->mNumVertices, std::vector<float>(4, 0.0f));
        }

        Vector3 totalVertexPos = Vector3::Zero;
        Vector3 maxVertexPos   = Vector3(-1000.0f, -1000.0f, -1000.0f);
        Vector3 minVertexPos   = Vector3(1000.0f, 1000.0f, 1000.0f);

        // Iterate through vertices.
        for (uint32 i = 0; i < aiMesh->mNumVertices; i++)
        {

            // Get array references from the current aiMesh on stack.
            const aiVector3D pos       = aiMesh->mVertices[i];
            const aiVector3D normal    = aiMesh->HasNormals() ? aiMesh->mNormals[i] : aiZeroVector;
            const aiVector3D texCoord  = aiMesh->HasTextureCoords(0) ? aiMesh->mTextureCoords[0][i] : aiZeroVector;
            const aiVector3D tangent   = aiMesh->HasTangentsAndBitangents() ? aiMesh->mTangents[i] : aiZeroVector;
            const aiVector3D biTangent = aiMesh->HasTangentsAndBitangents() ? aiMesh->mBitangents[i] : aiZeroVector;

            // Set aiMesh vertex data.
            linaMesh->AddElement(0, pos.x, pos.y, pos.z);
            linaMesh->AddElement(1, texCoord.x, texCoord.y);
            linaMesh->AddElement(2, normal.x, normal.y, normal.z);
            linaMesh->AddElement(3, tangent.x, tangent.y, tangent.z);
            linaMesh->AddElement(4, biTangent.x, biTangent.y, biTangent.z);

            // Add to the total vertex pos, used to calculate vertex offset.
            totalVertexPos += AssimpToLinaVector3(pos);

            // Set the min & vertex poses. Used to calculate the bounding box.
            if (pos.x > maxVertexPos.x)
                maxVertexPos.x = pos.x;
            else if (pos.x < minVertexPos.x)
                minVertexPos.x = pos.x;

            if (pos.y > maxVertexPos.y)
                maxVertexPos.y = pos.y;
            else if (pos.y < minVertexPos.y)
                minVertexPos.y = pos.y;

            if (pos.z > maxVertexPos.z)
                maxVertexPos.z = pos.z;
            else if (pos.z < minVertexPos.z)
                minVertexPos.z = pos.z;

            if (vertexBoneIDs.size() > 0)
            {
                const std::vector<int>&   vboneIDs     = vertexBoneIDs[i];
                const std::vector<float>& vboneWeights = vertexBoneWeights[i];
                linaMesh->AddElement(5, vboneIDs[0], vboneIDs[1], vboneIDs[2], vboneIDs[3]);
                linaMesh->AddElement(6, vboneWeights[0], vboneWeights[1], vboneWeights[2], vboneWeights[3]);
                // linaMesh->AddElement(5, 1, 3, 7, 0);
                // linaMesh->AddElement(6, 0.05f, 0.4f, 2.0f, 1.0f);
            }
            else
            {
                linaMesh->AddElement(5, -1, -1, -1, -1);
                linaMesh->AddElement(6, 0.0f, 0.0f, 0.0f, 0.0f);
            }
        }

        // Iterate through faces & add indices for each face.
        for (uint32 i = 0; i < aiMesh->mNumFaces; i++)
        {
            const aiFace& face = aiMesh->mFaces[i];
            LINA_ASSERT(face.mNumIndices == 3, "Number of indices need to be 3!");
            linaMesh->AddIndices(face.mIndices[0], face.mIndices[1], face.mIndices[2]);
        }

        // Add aiMesh to array.
        linaMesh->m_vertexCenter      = totalVertexPos / (float)aiMesh->mNumVertices;
        linaMesh->m_boundsMin         = minVertexPos;
        linaMesh->m_boundsMax         = maxVertexPos;
        linaMesh->m_boundsHalfExtents = (linaMesh->m_boundsMax - linaMesh->m_boundsMin) / 2.0f;
    }

    bool ModelLoader::LoadModel(const aiScene* scene, Model* model)
    {
        const std::string runningDirectory = Utility::GetRunningDirectory();

        // Load the ai hierarchy into the root node.
        ModelNode& root = model->m_rootNode;
        root.FillNodeHierarchy(scene->mRootNode, scene, model);

        // Iterate through the materials in the scene.
        for (uint32 i = 0; i < scene->mNumMaterials; i++)
        {
            // Build material reference & material specifications.
            const aiMaterial* material = scene->mMaterials[i];

            ImportedModelMaterial importedMaterial;
            importedMaterial.m_name = scene->mMaterials[i]->GetName().C_Str();

            // Currently only handles diffuse textures.
            aiString texturePath;

            std::vector<ImportTextureType> textureTypes{ImportTextureType::BaseColor, ImportTextureType::NormalCamera, ImportTextureType::EmissiveColor, ImportTextureType::Metalness, ImportTextureType::DiffuseRoughness, ImportTextureType::AmbientOcclusion};
            std::vector<uint32>            textureCounts;

            textureCounts.push_back(material->GetTextureCount(aiTextureType_BASE_COLOR));
            textureCounts.push_back(material->GetTextureCount(aiTextureType_NORMAL_CAMERA));
            textureCounts.push_back(material->GetTextureCount(aiTextureType_EMISSION_COLOR));
            textureCounts.push_back(material->GetTextureCount(aiTextureType_METALNESS));
            textureCounts.push_back(material->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS));
            textureCounts.push_back(material->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION));

            // TODO: Fill in with the texture path.
            for (uint32 i = 0; i < textureCounts.size(); i++)
            {
                for (uint32 j = 0; j < textureCounts[i]; j++)
                    importedMaterial.m_textures[textureTypes[i]].push_back("");
            }

            // Push the material to list.
            model->GetImportedMaterials().push_back(importedMaterial);
        }

        if (model->GetImportedMaterials().size() == 0)
        {
            ImportedModelMaterial defaultMaterial;
            defaultMaterial.m_name = "Material";
            model->GetImportedMaterials().push_back(defaultMaterial);
        }

        return true;
    }

    bool ModelLoader::LoadModel(unsigned char* data, size_t dataSize, Model* model)
    {
        // Get the importer & set assimp scene.
        Assimp::Importer importer;
        uint32           importFlags = 0;
        ModelAssetData*  assetData   = model->GetAssetData();
        if (assetData->m_calculateTangentSpace)
            importFlags |= aiProcess_CalcTangentSpace;

        if (assetData->m_triangulate)
            importFlags |= aiProcess_Triangulate;

        if (assetData->m_smoothNormals)
            importFlags |= aiProcess_GenSmoothNormals;

        if (assetData->m_flipUVs)
            importFlags |= aiProcess_FlipUVs;

        if (assetData->m_flipWinding)
            importFlags |= aiProcess_FlipWindingOrder;

        importFlags |= aiProcess_GlobalScale;
        importer.SetPropertyFloat("GLOBAL_SCALE_FACTOR", assetData->m_globalScale * 100.0f);

        const std::string ext   = "." + Utility::GetFileExtension(model->GetPath());
        const aiScene*    scene = importer.ReadFileFromMemory((void*)data, dataSize, importFlags, ext.c_str());
        const char*       err   = importer.GetErrorString();
        LINA_ASSERT(scene != nullptr, "Assimp could not read scene from memory.");
        return LoadModel(scene, model);
    }

    bool ModelLoader::LoadModel(const std::string& fileName, Model* model)
    {
        // Get the importer & set assimp scene.
        Assimp::Importer importer;
        uint32           importFlags = 0;
        ModelAssetData*  assetData   = model->GetAssetData();
        if (assetData->m_calculateTangentSpace)
            importFlags |= aiProcess_CalcTangentSpace;

        if (assetData->m_triangulate)
            importFlags |= aiProcess_Triangulate;

        if (assetData->m_smoothNormals)
            importFlags |= aiProcess_GenSmoothNormals;

        if (assetData->m_flipUVs)
            importFlags |= aiProcess_FlipUVs;

        if (assetData->m_flipWinding)
            importFlags |= aiProcess_FlipWindingOrder;

        importFlags |= aiProcess_GlobalScale;
        importer.SetPropertyFloat("GLOBAL_SCALE_FACTOR", assetData->m_globalScale * 100.0f);

        const aiScene* scene = importer.ReadFile(fileName.c_str(), importFlags);

        if (!scene)
        {
            LINA_ERR("Mesh loading failed! {0}", fileName.c_str());
            return false;
        }

        return LoadModel(scene, model);
    }

    bool ModelLoader::LoadSpriteQuad(Mesh& mesh)
    {
        // Build and indexed aiMesh for each aiMesh & fill in the data.
        mesh.AllocateElement(3, 0, true);        // Positions
        mesh.AllocateElement(2, 1, true);        // TexCoords
        mesh.AllocateElement(16, 7, true, true); // Model Matrix

        Vector3 vertices[] = {
            Vector3(-0.5f, 0.5f, 0.0f),  // left top, id 0
            Vector3(-0.5f, -0.5f, 0.0f), // left bottom, id 1
            Vector3(0.5f, -0.5f, 0.0f),  // right bottom, id 2
            Vector3(0.5f, 0.5f, 0.0f),   // right top, id 3
        };

        int indices[] = {// Left bottom triangle
                         0, 1, 2,
                         // Right top triangle
                         2, 3, 0};

        // Tex coords.
        Vector2 texCoords[] = {
            Vector2(1.0f, 1.0f), // left top, id 0
            Vector2(1.0f, 0.0f), // left bottom, id 1
            Vector2(0.0f, 0.0f), // right bottom, id 2
            Vector2(0.0f, 1.0f), // right top, id 3
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

    void ModelLoader::SetVertexBoneData(std::vector<int>& vertexBoneIDs, std::vector<float>& vertexBoneWeights, int boneID, float weight)
    {
        for (int i = 0; i < 4; ++i)
        {
            if (vertexBoneIDs[i] < 0)
            {
                vertexBoneIDs[i]     = boneID;
                vertexBoneWeights[i] = weight;
                break;
            }
        }
    }
} // namespace Lina::Graphics
