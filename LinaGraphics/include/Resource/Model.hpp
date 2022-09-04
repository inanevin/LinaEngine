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

#pragma once

#ifndef Model_HPP
#define Model_HPP

#include "Core/Resource.hpp"
#include "Data/Vector.hpp"
#include "Data/Vertex.hpp"
#include "Data/String.hpp"
namespace Lina
{
    namespace World
    {
        class EntityWorld;
        class Entity;
    } // namespace World

} // namespace Lina
namespace Lina::Graphics
{
    class ModelNode;

    class Model : public Resources::Resource
    {
    public:
        struct AssetData
        {
            bool  calculateTangent = false;
            bool  flipUVs          = false;
            bool  triangulate      = true;
            bool  smoothNormals    = false;
            bool  flipWinding      = false;
            float globalScale      = 1.0f;
        };

        Model() = default;
        virtual ~Model();

        virtual Resource* LoadFromMemory(const IStream& stream) override;
        virtual Resource* LoadFromFile(const String& path) override;

        World::Entity* AddToWorld(World::EntityWorld* w);

        inline AssetData& GetAssetData()
        {
            return m_assetData;
        }

        inline ModelNode* GetRootNode()
        {
            return m_rootNode;
        }

        inline const Vector<ModelNode*>& GetNodes()
        {
            return m_nodes;
        }

    protected:
        virtual void SaveToArchive(Serialization::Archive<OStream>& archive) override;
        virtual void LoadFromArchive(Serialization::Archive<IStream>& archive) override;

    private:
        World::Entity* CreateEntityForNode(World::Entity* parent, World::EntityWorld* w, ModelNode* n);

    private:
        friend class ModelLoader;
        friend class StaticMeshRenderer;
        AssetData m_assetData;

        // Runtime
        int                m_numMaterials = 0;
        int                m_numMeshes    = 0;
        int                m_numAnims     = 0;
        int                m_numVertices  = 0;
        int                m_numBones     = 0;
        ModelNode*         m_rootNode     = nullptr;
        Vector<ModelNode*> m_nodes;
    };

} // namespace Lina::Graphics

#endif
