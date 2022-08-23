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

#ifndef ModelNodeComponent_HPP
#define ModelNodeComponent_HPP

// Headers here.
#include "Components/RenderableComponent.hpp"
#include "Core/CommonReflection.hpp"
#include "Core/ResourceHandle.hpp"
#include "Resource/Model.hpp"
#include "Resource/Material.hpp"
#include "Data/HashMap.hpp"

namespace Lina
{
    namespace Graphics
    {
        class StaticMeshRenderer;
    }
} // namespace Lina

namespace Lina::Graphics
{
    LINA_COMPONENT("Model Node", "Graphics")
    class ModelNodeComponent : public RenderableComponent
    {
    public:
        virtual TypeID GetTID()
        {
            return GetTypeID<ModelNodeComponent>();
        }

        virtual void SaveToArchive(cereal::PortableBinaryOutputArchive& oarchive) override
        {
            RenderableComponent::SaveToArchive(oarchive);
            oarchive(m_modelHandle, m_nodeIndex, m_materials);
        };

        virtual void LoadFromArchive(cereal::PortableBinaryInputArchive& iarchive) override
        {
            RenderableComponent::LoadFromArchive(iarchive);
            iarchive(m_modelHandle, m_nodeIndex, m_materials);
        };

        inline uint32 GetNodeIndex()
        {
            return m_nodeIndex;
        }

    private:
        friend class Graphics::StaticMeshRenderer;
        friend class Model;

        Resources::ResourceHandle<Model>                     m_modelHandle;
        HashMap<uint32, Resources::ResourceHandle<Material>> m_materials; // Index - material pair
        uint32                                               m_nodeIndex = 0;
    };
} // namespace Lina::Graphics

#endif
