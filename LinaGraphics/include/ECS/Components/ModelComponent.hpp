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

#ifndef ModelComponent_HPP
#define ModelComponent_HPP

// Headers here.
#include "ECS/Component.hpp"
#include "Core/ResourceHandle.hpp"
#include "Resource/Model.hpp"
#include "Resource/Material.hpp"
#include "Core/CommonECS.hpp"
#include "Data/Vector.hpp"
#include <cereal/access.hpp>

namespace Lina
{
    namespace Graphics
    {
        class StaticMeshRenderer;
    } // namespace Graphics
} // namespace Lina

namespace Lina::ECS
{
    struct ModelComponent : public Component
    {
        bool visible = true;

        void SetModel(Graphics::Model* model, Entity owner, Registry& reg);

    private:
        void AddNodeRenderer(Graphics::Model* parentModel, Entity parent, Registry& reg, Graphics::ModelNode* node);

    private:
        friend class Graphics::StaticMeshRenderer;

        Resources::ResourceHandle<Graphics::Model>            m_modelHandle;
        Vector<Resources::ResourceHandle<Graphics::Material>> m_materials;
        Vector<ECS::Entity>                                   m_nodeEntities;

        friend class cereal::access;

        template <class Archive>
        void serialize(Archive& archive)
        {
            archive(m_modelHandle, visible, m_materials);
        }
    };
} // namespace Lina::ECS

#endif
