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

#ifndef CameraComponent_HPP
#define CameraComponent_HPP

// Headers here.
#include "World/Core/Component.hpp"
#include "Reflection/ReflectionCommon.hpp"

namespace Lina::Graphics
{
    LINA_COMPONENT("Camera Component", "Graphics")
    class CameraComponent : public World::Component
    {
    public:
        float fieldOfView = 90.0f;
        float zNear       = 0.01f;
        float zFar        = 1000.0f;

        virtual void SaveToArchive(Serialization::Archive<OStream>& oarchive) override
        {
            Component::SaveToArchive(oarchive);
            oarchive(fieldOfView, zNear, zFar);
        };

        virtual void LoadFromArchive(Serialization::Archive<IStream>& iarchive) override
        {
            Component::LoadFromArchive(iarchive);
            iarchive(fieldOfView, zNear, zFar);
        };

        virtual TypeID GetTID() override
        {
            return GetTypeID<CameraComponent>();
        }
    };
} // namespace Lina::Graphics

#endif
