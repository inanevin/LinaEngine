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

#ifndef Entity_HPP
#define Entity_HPP

#include "Core/SizeDefinitions.hpp"
#include "Data/String.hpp"
#include "Data/HashSet.hpp"
#include "Data/Serialization/SetSerialization.hpp"
#include "Math/Transformation.hpp"

#include <cereal/access.hpp>

namespace Lina::World
{
    // Actual game state
    class Entity
    {
    public:
        Entity()  = default;
        ~Entity() = default;

        inline uint32 GetID()
        {
            return m_id;
        }

    private:
        friend class EntityWorld;
        friend struct EntitySerializer;

        uint32          m_id     = 0;
        String          m_name   = "";
        uint32          m_parent = 0;
        HashSet<uint32> m_children;
        Transformation  m_transform;

    private:
        friend class cereal::access;

        template <class Archive>
        void serialize(Archive& archive)
        {
            archive(m_id, m_name, m_parent, m_children, m_transform);
        }
    };

} // namespace Lina::World

#endif
