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
