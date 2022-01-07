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
Class: ModelNodeComponent



Timestamp: 12/24/2021 10:20:14 PM
*/

#pragma once

#ifndef ModelNodeComponent_HPP
#define ModelNodeComponent_HPP

// Headers here.
#include "ECS/Component.hpp"
#include "Rendering/Model.hpp"

#include <cereal/access.hpp>
#include <cereal/types/vector.hpp>

namespace Lina
{
    namespace Graphics
    {
        class ModelNode;
    }
} // namespace Lina

namespace Lina::ECS
{
    LINA_COMPONENT("Model Node", "ICON_FA_CUBES", "Rendering", "false", "false", "")
    struct ModelNodeComponent : public Component
    {
        LINA_PROPERTY("Materials", "MaterialArray", "", "")
        std::vector<Resources::ResourceHandle<Graphics::Material>> m_materials;

        Resources::ResourceHandle<Graphics::ModelNode> m_modelNode;

    private:
        friend class cereal::access;

        template <class Archive>
        void serialize(Archive& archive)
        {
            archive(m_materials, m_modelNode, m_isEnabled);
        }
    };

} // namespace Lina::ECS

#endif
