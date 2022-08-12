/*
Class: MeshRendererComponent
Timestamp: 4/14/2019 1:37:59 AM
*/

#pragma once

#ifndef RenderableMeshComponent_HPP
#define RenderableMeshComponent_HPP

#include "ECS/Component.hpp"
#include "Math/Vector.hpp"
#include "Utility/StringId.hpp"

#include "Data/Serialization/StringSerialization.hpp"
#include <Data/Serialization/VectorSerialization.hpp>

namespace Lina::ECS
{
    struct MeshRendererComponent : public Component
    {
        String      m_modelPath         = "";
        String      m_materialPath      = "";
        StringID     m_modelID           = -1;
        StringID     m_materialID        = -1;
        Vector3          m_totalVertexCenter = Vector3::Zero;
        Vector3          m_totalBoundsMin    = Vector3::Zero;
        Vector3          m_totalBoundsMax    = Vector3::Zero;
        Vector3          m_totalHalfBounds   = Vector3::Zero;
        Vector<int> m_subMeshes; // Index array for each mesh given under this renderer, each entry corresponds to a mesh within the model's mesh array
        int              m_nodeID              = -1;
        bool             m_excludeFromDrawList = false;

        template <class Archive> void serialize(Archive& archive)
        {
            archive(m_excludeFromDrawList, m_isEnabled, m_modelPath, m_materialPath, m_totalVertexCenter, m_totalBoundsMin, m_totalBoundsMax, m_totalHalfBounds);
        }
    };
} // namespace Lina::ECS

#endif
