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

#ifndef PhysicsMaterial_HPP
#define PhysicsMaterial_HPP

#include "Utility/StringId.hpp"
#include "Core/Resource.hpp"
#include "Serialization/StringSerialization.hpp"

namespace Lina
{
    namespace World
    {
        class Level;
    }
} // namespace Lina
namespace Lina::Physics
{
    class PhysicsMaterial : public Resources::Resource
    {

    public:
        PhysicsMaterial()  = default;
        ~PhysicsMaterial() = default;

        template <class Archive>
        void Serialize(Archive& archive)
        {
            archive(m_staticFriction, m_dynamicFriction, m_restitution);
        }

        static PhysicsMaterial* CreatePhysicsMaterial(const String& savePath, float staticFriction, float dynamicFriction, float restitution);
        virtual Resource*       LoadFromMemory(const IStream& stream) override;
        virtual Resource*       LoadFromFile(const String& path) override;
        virtual void            WriteToPackage(Serialization::Archive<OStream>& archive) override;

        float GetStaticFriction()
        {
            return m_staticFriction;
        }
        float GetDynamicFriction()
        {
            return m_dynamicFriction;
        }
        float GetRestitution()
        {
            return m_restitution;
        }

    private:
        friend class World::Level;
        friend class PhysicsEngine;

        float m_staticFriction  = 0.5f;
        float m_dynamicFriction = 0.5f;
        float m_restitution     = 0.5f;
    };
} // namespace Lina::Physics

#endif
