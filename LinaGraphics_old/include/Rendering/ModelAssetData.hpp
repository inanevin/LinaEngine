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
#include "Data/Map.hpp"
#include "Data/Vector.hpp"

namespace Lina::Graphics
{
    LINA_CLASS("Model Data")
    class ModelAssetData : public Resources::IResource
    {

    public:
        ModelAssetData()  = default;
        ~ModelAssetData() = default;

        virtual void* LoadFromMemory(const String& path, unsigned char* data, size_t dataSize) override;
        virtual void* LoadFromFile(const char* path) override;

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
        bool m_generatePivots = false;

        bool                              m_regenerateConvexMeshes = false;
        bool                              m_triangulate            = true;
        Map<int, Vector<uint8>> m_convexMeshData;

        template <class Archive>
        void serialize(Archive& archive)
        {
            archive(m_triangulate, m_smoothNormals, m_generatePivots, m_calculateTangentSpace, m_flipUVs, m_flipWinding, m_globalScale, m_convexMeshData);
        }
    };
} // namespace Lina::Graphics

#endif
