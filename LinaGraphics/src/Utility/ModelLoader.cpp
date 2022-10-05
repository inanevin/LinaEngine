/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-] [Inan Evin]

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
#include "Resource/Model.hpp"
#include "Resource/ModelNode.hpp"
#include "Resource/StaticMesh.hpp"
#include "Resource/SkinnedMesh.hpp"
#include "Utility/UtilityFunctions.hpp"
#include <assimp/Importer.hpp>
#include <assimp/metadata.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace Lina::Graphics
{

    Matrix AssimpToLinaMatrix(const aiMatrix4x4& aiMat)
    {
        Matrix mat;
        mat[0][0] = aiMat.a1;
        mat[0][1] = aiMat.b1;
        mat[0][2] = aiMat.c1;
        mat[0][3] = aiMat.d1;
        mat[1][0] = aiMat.a2;
        mat[1][1] = aiMat.b2;
        mat[1][2] = aiMat.c2;
        mat[1][3] = aiMat.d2;
        mat[2][0] = aiMat.a3;
        mat[2][1] = aiMat.b3;
        mat[2][2] = aiMat.c3;
        mat[2][3] = aiMat.d3;
        mat[3][0] = aiMat.a4;
        mat[3][1] = aiMat.b4;
        mat[3][2] = aiMat.c4;
        mat[3][3] = aiMat.d4;
        return mat;
    }

    Vector3 AssimpToLinaVector3(const aiVector3D& vec)
    {
        return Vector3(vec.x, vec.y, vec.z);
    }

    Vector2 AssimpToLinaVector2(const aiVector3D& vec)
    {
        return Vector2(vec.x, vec.y);
    }

    bool ModelLoader::LoadModel(unsigned char* data, size_t dataSize, Model* model)
    {
        // Get the importer & set assimp scene.
        Assimp::Importer importer;
        uint32           importFlags = GetImportFlags(model);

        importer.SetPropertyFloat("GLOBAL_SCALE_FACTOR", model->GetAssetData().globalScale * 100.0f);

        const String   ext   = "." + Utility::GetFileExtension(model->GetPath());
        const aiScene* scene = importer.ReadFileFromMemory((void*)data, dataSize, importFlags, ext.c_str());
        const char*    err   = importer.GetErrorString();
        LINA_ASSERT(scene != nullptr, "Assimp could not read scene from memory.");
        return LoadModelProcess(scene, model);
    }

    bool ModelLoader::LoadModel(const String& fileName, Model* model)
    {
        // Get the importer & set assimp scene.
        Assimp::Importer importer;
        uint32           importFlags = GetImportFlags(model);

        importer.SetPropertyFloat("GLOBAL_SCALE_FACTOR", model->GetAssetData().globalScale * 100.0f);

        const aiScene* scene = importer.ReadFile(fileName.c_str(), importFlags);
        const char*    err   = importer.GetErrorString();
        LINA_ASSERT(scene != nullptr, "Assimp could not read scene from memory.");
        return LoadModelProcess(scene, model);
    }

    void ModelLoader::FillMeshData(const aiMesh* aiMesh, Mesh* linaMesh)
    {
        // Build and indexed aiMesh for each aiMesh & fill in the data.
        linaMesh->m_name         = aiMesh->mName.C_Str();
        linaMesh->m_materialSlot = aiMesh->mMaterialIndex;

        const aiVector3D aiZeroVector(0.0f, 0.0f, 0.0f);

        // Vector<Vector<int>>   vertexBoneIDs;
        // Vector<Vector<float>> vertexBoneWeights;
        //
        // if (aiMesh->mNumBones > 0)
        // {
        //     vertexBoneIDs.resize(aiMesh->mNumVertices, Vector<int>(4, -1));
        //     vertexBoneWeights.resize(aiMesh->mNumVertices, Vector<float>(4, 0.0f));
        // }

        Vector3 maxVertexPos = Vector3(-1000.0f, -1000.0f, -1000.0f);
        Vector3 minVertexPos = Vector3(1000.0f, 1000.0f, 1000.0f);

        // Iterate through vertices.
        for (uint32 i = 0; i < aiMesh->mNumVertices; i++)
        {

            // Get array references from the current aiMesh on stack.
            const aiVector3D pos       = aiMesh->mVertices[i];
            const aiVector3D normal    = aiMesh->HasNormals() ? aiMesh->mNormals[i] : aiZeroVector;
            const aiVector3D texCoord  = aiMesh->HasTextureCoords(0) ? aiMesh->mTextureCoords[0][i] : aiZeroVector;
            const aiVector3D tangent   = aiMesh->HasTangentsAndBitangents() ? aiMesh->mTangents[i] : aiZeroVector;
            const aiVector3D biTangent = aiMesh->HasTangentsAndBitangents() ? aiMesh->mBitangents[i] : aiZeroVector;
            linaMesh->AddVertex(AssimpToLinaVector3(pos), AssimpToLinaVector3(normal), AssimpToLinaVector2(texCoord));

            // Set aiMesh vertex data.
            // linaMesh->AddElement(0, pos.x, pos.y, pos.z);
            // linaMesh->AddElement(1, texCoord.x, texCoord.y);
            // linaMesh->AddElement(2, normal.x, normal.y, normal.z);
            // linaMesh->AddElement(3, tangent.x, tangent.y, tangent.z);
            // linaMesh->AddElement(4, biTangent.x, biTangent.y, biTangent.z);

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

            // if (vertexBoneIDs.size() > 0)
            // {
            //     const Vector<int>& vboneIDs = vertexBoneIDs[i];
            //     const Vector<float>& vboneWeights = vertexBoneWeights[i];
            //     linaMesh->AddElement(5, vboneIDs[0], vboneIDs[1], vboneIDs[2], vboneIDs[3]);
            //     linaMesh->AddElement(6, vboneWeights[0], vboneWeights[1], vboneWeights[2], vboneWeights[3]);
            //     // linaMesh->AddElement(5, 1, 3, 7, 0);
            //     // linaMesh->AddElement(6, 0.05f, 0.4f, 2.0f, 1.0f);
            // }
            // else
            // {
            //     linaMesh->AddElement(5, -1, -1, -1, -1);
            //     linaMesh->AddElement(6, 0.0f, 0.0f, 0.0f, 0.0f);
            // }
        }

        // Iterate through faces & add indices for each face.
        for (uint32 i = 0; i < aiMesh->mNumFaces; i++)
        {
            const aiFace& face = aiMesh->mFaces[i];

            for (unsigned int j = 0; j < face.mNumIndices; j++)
            {
                linaMesh->AddIndices(face.mIndices[j]);
            }
        }

        // Size calc.
        linaMesh->m_aabb.boundsMin         = minVertexPos;
        linaMesh->m_aabb.boundsMax         = maxVertexPos;
        linaMesh->m_vertexCenter           = (minVertexPos + maxVertexPos) / 2.0f;
        linaMesh->m_aabb.boundsHalfExtents = (maxVertexPos - minVertexPos) / 2.0f;
    }

    void ModelLoader::FillNodeHierarchy(const aiNode* ainode, const aiScene* scene, Model* parentModel, ModelNode* n)
    {
        n->m_name           = String(ainode->mName.C_Str());
        n->m_localTransform = AssimpToLinaMatrix(ainode->mTransformation);

        const unsigned int numMeshes = ainode->mNumMeshes;

        Vector3 currentMinBounds = Vector3(1000, 1000, 1000);
        Vector3 currentMaxBounds = Vector3(-1000, -1000, -1000);

        Mesh* addedMesh = nullptr;

        for (uint32 i = 0; i < numMeshes; i++)
        {
            aiMesh* aimesh = scene->mMeshes[ainode->mMeshes[i]];

            if (aimesh->HasBones())
            {
                SkinnedMesh* skinned = new SkinnedMesh();
                n->m_meshes.push_back(skinned);
                addedMesh = skinned;
            }
            else
            {
                StaticMesh* staticMesh = new StaticMesh();
                n->m_meshes.push_back(staticMesh);
                addedMesh = staticMesh;
            }

            parentModel->m_numVertices += aimesh->mNumVertices;
            parentModel->m_numBones += aimesh->mNumBones;
            FillMeshData(aimesh, addedMesh);

            n->m_totalVertexCenter += addedMesh->m_vertexCenter;

            const Vector3 meshBoundsMin = addedMesh->m_aabb.boundsMin;
            const Vector3 meshBoundsMax = addedMesh->m_aabb.boundsMax;

            if (meshBoundsMin.x < currentMinBounds.x)
                currentMinBounds.x = meshBoundsMin.x;
            if (meshBoundsMax.x > currentMaxBounds.x)
                currentMaxBounds.x = meshBoundsMax.x;

            if (meshBoundsMin.y < currentMinBounds.y)
                currentMinBounds.y = meshBoundsMin.y;
            if (meshBoundsMax.y > currentMaxBounds.y)
                currentMaxBounds.y = meshBoundsMax.y;

            if (meshBoundsMin.z < currentMinBounds.z)
                currentMinBounds.z = meshBoundsMin.z;
            if (meshBoundsMax.z > currentMaxBounds.z)
                currentMaxBounds.z = meshBoundsMax.z;
        }

        // Take the average of total bounds if we have any meshes for this node.
        if (numMeshes > 0)
        {
            const float numMeshesFloat = static_cast<float>(numMeshes);
            n->m_totalVertexCenter /= numMeshesFloat;
            n->m_aabb.boundsMin         = currentMinBounds;
            n->m_aabb.boundsMax         = currentMaxBounds;
            n->m_aabb.boundsHalfExtents = (currentMaxBounds - currentMinBounds) / 2.0f;

            n->m_aabb.positions.resize(8);
            n->m_aabb.positions[0] = n->m_aabb.boundsMin;                                                          // bottom-left-back
            n->m_aabb.positions[1] = Vector3(n->m_aabb.boundsMin.x, n->m_aabb.boundsMin.y, n->m_aabb.boundsMax.z); // bottom-left-front
            n->m_aabb.positions[2] = Vector3(n->m_aabb.boundsMax.x, n->m_aabb.boundsMin.y, n->m_aabb.boundsMin.z); // bottom-right-back
            n->m_aabb.positions[3] = Vector3(n->m_aabb.boundsMax.x, n->m_aabb.boundsMin.y, n->m_aabb.boundsMax.z); // bottom-right-front
            n->m_aabb.positions[4] = Vector3(n->m_aabb.boundsMin.x, n->m_aabb.boundsMax.y, n->m_aabb.boundsMin.z); // top - left - back
            n->m_aabb.positions[5] = Vector3(n->m_aabb.boundsMin.x, n->m_aabb.boundsMax.y, n->m_aabb.boundsMax.z); // top - left - front
            n->m_aabb.positions[6] = Vector3(n->m_aabb.boundsMax.x, n->m_aabb.boundsMax.y, n->m_aabb.boundsMin.z); // top - right - back
            n->m_aabb.positions[7] = n->m_aabb.boundsMax;                                                          // top - right - front
        }

        // Recursively fill the other nodes.
        for (uint32 i = 0; i < ainode->mNumChildren; i++)
        {
            ModelNode* newNode = new ModelNode();
            newNode->m_index   = static_cast<uint32>(parentModel->m_nodes.size());
            parentModel->m_nodes.push_back(newNode);
            n->m_children.push_back(newNode);
            FillNodeHierarchy(ainode->mChildren[i], scene, parentModel, n);
        }
    }

    bool ModelLoader::LoadModelProcess(const aiScene* scene, Model* model)
    {
        model->m_numMeshes    = scene->mNumMeshes;
        model->m_numMaterials = scene->mNumMaterials;
        model->m_numAnims     = scene->mNumAnimations;

        ModelNode* n               = new ModelNode();
        model->m_rootNode          = n;
        model->m_rootNode->m_index = static_cast<uint32>(model->m_nodes.size());
        model->m_nodes.push_back(model->m_rootNode);
        FillNodeHierarchy(scene->mRootNode, scene, model, n);

        // TODO?: Materials & textures.

        return false;
    }

    uint32 ModelLoader::GetImportFlags(Model* model)
    {
        uint32            importFlags = 0;
        Model::AssetData& assetData   = model->GetAssetData();
        if (assetData.calculateTangent)
            importFlags |= aiProcess_CalcTangentSpace;

        if (assetData.triangulate)
            importFlags |= aiProcess_Triangulate;

        if (assetData.smoothNormals)
            importFlags |= aiProcess_GenSmoothNormals;

        importFlags |= aiProcess_FlipUVs;

        if (assetData.flipWinding)
            importFlags |= aiProcess_FlipWindingOrder;

        importFlags |= aiProcess_GlobalScale;

        return importFlags;
    }
} // namespace Lina::Graphics
