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
Class: ModelNode



Timestamp: 12/24/2021 9:00:02 PM
*/

#pragma once

#ifndef ModelNode_HPP
#define ModelNode_HPP

// Headers here.
#include "Math/Matrix.hpp"
#include "Utility/StringId.hpp"

#include <cereal/access.hpp>
#include <cereal/types/memory.hpp>
#include <memory>
#include <vector>

struct aiNode;
struct aiScene;

namespace Lina
{
    namespace ECS
    {
        class ModelNodeSystem;
    }
} // namespace Lina

namespace Lina::Graphics
{
    class Mesh;
    class Model;
    class ModelLoader;

    class ModelNode
    {

    public:
        ModelNode();
        ~ModelNode();
        ModelNode(const ModelNode& old_obj);

        void                             FillNodeHierarchy(const aiNode* node, const aiScene* scene, Model& parentModel, bool fillMeshesOnly);
        inline const std::vector<Mesh*>& GetMeshes() const
        {
            return m_meshes;
        }
        inline const std::string& GetName() const
        {
            return m_name;
        }
        inline const std::vector<ModelNode*>& GetChildren() const
        {
            return m_children;
        }

    private:
        void Clear()
        {
            m_id             = 0;
            m_name           = "";
            m_localTransform = Matrix();
            m_defaultMaterials.clear();
            m_children.clear();
        }

    private:
        friend class ECS::ModelNodeSystem;
        friend class Graphics::ModelLoader;
        friend class cereal::access;

        StringIDType              m_id = 0;
        std::vector<StringIDType> m_defaultMaterials;
        std::vector<Mesh*>        m_meshes;
        std::string               m_name = "";
        Matrix                    m_localTransform;
        std::vector<ModelNode*>   m_children;
    };
} // namespace Lina::Graphics

#endif
