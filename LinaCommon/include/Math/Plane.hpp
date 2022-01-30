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
Class: Plane



Timestamp: 1/12/2022 2:02:49 AM
*/

#pragma once

#ifndef Plane_HPP
#define Plane_HPP

// Headers here.
#include "Math/Vector.hpp"

namespace Lina
{
    class Plane
    {

    public:
        Plane() = default;
        Plane(const Vector3& normal, float dist)
            : m_normal(normal), m_distance(dist){};
        ~Plane() = default;

        void Normalize();
        float GetSignedDistance(const Vector3& point) const;

        Vector3 m_normal   = Vector3::Zero;
        float   m_distance = 0.0f;

    private:
    };
} // namespace Lina

#endif
