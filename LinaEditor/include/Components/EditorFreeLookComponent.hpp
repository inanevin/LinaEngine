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

#ifndef FreeLookComponent_HPP
#define FreeLookComponent_HPP

#include "Core/Component.hpp"
#include "Core/CommonReflection.hpp"
#include "Math/Vector.hpp"

namespace Lina::World
{
    LINA_COMPONENT("Editor Free Look", "")
    class EditorFreeLookComponent : public Component
    {
    public:
        LINA_FIELD("Movement Speed", "Float")
        float movementSpeed = 12.0f;

        LINA_FIELD("Rotation Power", "float")
        float rotationPower = 1.5f;

        virtual TypeID GetTID() override
        {
            return GetTypeID<EditorFreeLookComponent>();
        }

        virtual void SaveToArchive(cereal::PortableBinaryOutputArchive& oarchive) override
        {
            Component::SaveToArchive(oarchive);
            oarchive(movementSpeed, rotationPower, m_angles);
        }

        virtual void LoadFromArchive(cereal::PortableBinaryInputArchive& iarchive) override
        {
            Component::LoadFromArchive(iarchive);
            iarchive(movementSpeed, rotationPower, m_angles);
        }

        virtual void OnTick(const Event::ETick& ev) override;

        virtual Bitmask GetComponentMask() override
        {
            return ComponentMask::ReceiveOnTick;
        }

    private:
        Vector2 m_lastMousePos = Vector2::Zero;
        Vector2 m_angles       = Vector2::Zero;
    };
} // namespace Lina::World

#endif
