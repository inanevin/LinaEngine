/*
Class: Model


Timestamp: 5/6/2019 4:23:45 PM
*/

#pragma once

#ifndef MODEL_HPP
#define MODEL_HPP

#include "Rendering/ModelNode.hpp"
#include "Rendering/ModelAssetData.hpp"
#include "Rendering/RenderingCommon.hpp"
#include "Data/Vector.hpp"

namespace Lina::Graphics
{
    class VertexArray;
    class ModelLoader;

    class Model : public Resources::IResource
    {

    public:
        Model() = default;
        virtual ~Model();

        virtual void* LoadFromMemory(const String& path, unsigned char* data, size_t dataSize) override;
        virtual void* LoadFromFile(const String& path) override;

        inline ModelAssetData* GetAssetData()
        {
            return m_assetData;
        }
        inline Vector<ImportedModelMaterial>& GetImportedMaterials()
        {
            return m_importedMaterials;
        }

        inline ModelNode* GetRootNode()
        {
            return m_rootNode;
        }
        inline int GetNumMeshes()
        {
            return m_numMeshes;
        }
        inline int GetNumMaterials()
        {
            return m_numMaterials;
        }
        inline int GetNumAnims()
        {
            return m_numAnimations;
        }
        inline int GetNumVertices()
        {
            return m_numVertices;
        }
        inline int GetNumBones()
        {
            return m_numBones;
        }
        inline int GetNumNodes()
        {
            return m_numNodes;
        }
        inline Vector<ModelNode*>& GetAllNodes()
        {
            return m_allNodes;
        }


    private:
        friend class RenderEngine;
        friend class ModelLoader;
        friend class ModelNode;

        int                                m_numMeshes     = 0;
        int                                m_numMaterials  = 0;
        int                                m_numAnimations = 0;
        int                                m_numVertices   = 0;
        int                                m_numBones      = 0;
        int                                m_numNodes      = 0;
        ModelAssetData*                    m_assetData     = nullptr;
        ModelNode*                         m_rootNode;
        Vector<ModelNode*>            m_allNodes;
        Vector<ImportedModelMaterial> m_importedMaterials;
    };
} // namespace Lina::Graphics

#endif
