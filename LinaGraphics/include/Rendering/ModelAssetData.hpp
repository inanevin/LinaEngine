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

/*
Class: ModelAssetData



Timestamp: 12/31/2021 8:18:10 PM
*/

#pragma once

#ifndef ModelAssetData_HPP
#define ModelAssetData_HPP

// Headers here.
#include "Resources/IResource.hpp"
#include "Core/CommonReflection.hpp"
#include <map>
#include <vector>

namespace Lina::Graphics
{
    LINA_CLASS("Model Data")
    class ModelAssetData : public Resources::IResource
    {

    public:
        ModelAssetData()  = default;
        ~ModelAssetData() = default;

        virtual void* LoadFromMemory(const std::string& path, unsigned char* data, size_t dataSize) override;
        virtual void* LoadFromFile(const std::string& path) override;

        LINA_PROPERTY("Global Scale", "Float")
        float m_globalScale = 1.0f; // 1 meter file = 1 unit Lina

        LINA_PROPERTY("Smooth Normals", "Bool")
        bool m_smoothNormals = true;

        LINA_PROPERTY("Calc Tangents", "Bool")
        bool m_calculateTangentSpace = true;

        LINA_PROPERTY("Flip Winding", "Bool")
        bool m_flipWinding = false;

        LINA_PROPERTY("Flip UVs", "Bool")
        bool m_flipUVs = false;

        LINA_PROPERTY("Generate Entity Pivots", "Bool", "If true, any entity generated via adding this model to the scene will have offset pivots as parents.")
        bool m_enitiesHasPivots = false;

        bool                              m_regenerateConvexMeshes = false;
        bool                              m_triangulate            = true;
        std::map<int, std::vector<uint8>> m_convexMeshData;

        template <class Archive>
        void serialize(Archive& archive)
        {
            archive(m_triangulate, m_smoothNormals, m_calculateTangentSpace, m_flipUVs, m_flipWinding, m_globalScale, m_convexMeshData);
        }
    };
} // namespace Lina::Graphics

#endif
