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

#include "Core/IResource.hpp"
#include "Data/Vector.hpp"
#include "Math/Vertex.hpp"
#include "Data/String.hpp"

namespace Lina::Graphics
{
    class ModelNode;

    class Model : public Resources::IResource
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

        Model()          = default;
        virtual ~Model();

        virtual void* LoadFromMemory(const String& path, unsigned char* data, size_t dataSize) override;
        virtual void* LoadFromFile(const String& path) override;
        virtual void  LoadAssetData() override;
        virtual void  SaveAssetData() override;

        inline AssetData& GetAssetData()
        {
            return m_assetData;
        }

    private:
        friend class ModelLoader;

        AssetData m_assetData;

        // Runtime
        int             m_numMaterials = 0;
        int             m_numMeshes    = 0;
        int             m_numAnims     = 0;
        int             m_numVertices  = 0;
        int             m_numBones     = 0;
        ModelNode*      m_rootNode     = nullptr;
    };

} // namespace Lina::Graphics

#endif
