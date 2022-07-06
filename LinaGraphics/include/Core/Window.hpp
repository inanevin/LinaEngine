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

#ifndef Window_HPP
#define Window_HPP

#include "Core/CommonApplication.hpp"
#include "Math/Vector.hpp"

struct GLFWwindow;

namespace Lina::Graphics
{
    class Window
    {
        void SetSize(const Vector2i& newSize);
        void SetPos(const Vector2i& newPos);
        void SetPosCentered(const Vector2i& newPos);
        void SetVsync(VsyncMode mode);

        static Window* Get()
        {
            return s_instance;
        }

    private:
        friend class RenderEngine;

        Window()  = default;
        ~Window() = default;

        bool Initialize(ApplicationInfo& appInfo);
        void Shutdown();
        void Close();

        static Window*  s_instance;
        ApplicationInfo m_appInfo;
        GLFWwindow*     m_glfwWindow = nullptr;
        void*           m_userPtr    = nullptr;
    };
} // namespace Lina::Graphics

#endif
