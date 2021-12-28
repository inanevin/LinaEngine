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
Class: FrustumSystem



Timestamp: 12/24/2021 12:59:35 AM
*/

#pragma once

#ifndef FrustumSystem_HPP
#define FrustumSystem_HPP

// Headers here.
#include "Core/CommonECS.hpp"
#include "ECS/System.hpp"

namespace Lina
{
    class Vector3;
}

namespace Lina::ECS
{
    class FrustumSystem : public System
    {

    public:
        FrustumSystem()  = default;
        ~FrustumSystem() = default;

        virtual void Initialize() override;
        virtual void UpdateComponents(float delta);

        /// <summary>
        /// Sets the bounds position (global) and half-extents if the entity contains a renderable component.
        /// Returns whether entity contains any bounds (renderable or not).
        /// </summary>
        bool GetEntityBounds(Entity ent, Vector3& boundsPosition, Vector3& boundsHalfExtent);

    private:
    };
} // namespace Lina::ECS

#endif
