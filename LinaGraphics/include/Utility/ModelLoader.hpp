/*
Class: ModelLoader

Assimp wrapper, responsible for loading modals and creating the necessary Mesh data.

Timestamp: 4/26/2019 12:07:47 AM
*/

#pragma once

#ifndef ModelLoader_HPP
#define ModelLoader_HPP

#include <Data/String.hpp>
#include "Data/Vector.hpp"

struct aiNode;
struct aiScene;
struct aiMesh;

namespace Lina::Graphics
{
    class Mesh;
    class Model;

    class ModelLoader
    {
    public:
        // Load models using ASSIMP
        static void FillMeshData(const aiMesh* aimesh, Mesh* linaMesh);
        static bool LoadModel(const aiScene* scene, Model* model);
        static bool LoadModel(unsigned char* data, size_t dataSize, Model* model);
        static bool LoadModel(const String& fileName, Model* model);
        static bool LoadSpriteQuad(Mesh& model);
        static void SetVertexBoneData(Vector<int>& vertexBoneIDs, Vector<float>& vertexBoneWeights, int boneID, float weight);
    };
} // namespace Lina::Graphics

#endif
