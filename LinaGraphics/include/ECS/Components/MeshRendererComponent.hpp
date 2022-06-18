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
Class: MeshRendererComponent
Timestamp: 4/14/2019 1:37:59 AM
*/

#pragma once

#ifndef RenderableMeshComponent_HPP
#define RenderableMeshComponent_HPP

#include "ECS/Component.hpp"
#include "Math/Vector.hpp"
#include "Utility/StringId.hpp"

#include <cereal/types/string.hpp>
#include <Data/Serialization/VectorSerialization.hpp>

namespace Lina::ECS
{
    struct MeshRendererComponent : public Component
    {
        std::string      m_modelPath         = "";
        std::string      m_materialPath      = "";
        StringIDType     m_modelID           = -1;
        StringIDType     m_materialID        = -1;
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
