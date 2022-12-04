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

#ifndef CameraSystem_HPP
#define CameraSystem_HPP

#include "Math/Matrix.hpp"
#include "Math/Vector.hpp"

namespace Lina::Graphics
{
    class CameraComponent;
    class CameraSystem
    {
    public:
        void SetActiveCamera(CameraComponent* c)
        {
            m_activeCamera = c;
        }

        inline const Vector3& GetPos() const
        {
            return m_pos;
        }
        inline const Matrix& GetView() const
        {
            return m_view;
        }

        inline const Matrix& GetProj() const
        {
            return m_proj;
        }

    private:
        friend class Renderer;
        CameraSystem()  = default;
        ~CameraSystem() = default;

        void Tick();

    private:
        Vector3          m_pos          = Vector3::Zero;
        CameraComponent* m_activeCamera = nullptr;
        Matrix           m_view         = Matrix::Identity();
        Matrix           m_proj         = Matrix::Identity();
    };
} // namespace Lina::Graphics

#endif
