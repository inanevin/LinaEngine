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

#ifndef Win32Window_HPP
#define Win32Window_HPP

#include "Core/Window.hpp"

struct HWND__;
struct HINSTANCE__;

namespace Lina::Graphics
{
    class Win32Window : public Window
    {
    public:
        // Inherited via Window
        virtual void SetSize(const Vector2i& newSize) override;
        virtual void SetPos(const Vector2i& newPos) override;
        virtual void SetPosCentered(const Vector2i& newPos) override;
        virtual void SetVsync(VsyncMode mode) override;
        virtual void SetTitle(const String& title) override;

        void OnWin32Close();

        inline static Win32Window* GetWin32()
        {
            return s_win32Window;
        }

        inline HWND__* GetWindowPtr()
        {
            return m_window;
        }

        inline HINSTANCE__* GetInstancePtr()
        {
            return m_hinst;
        }

        void UpdateButtonLayoutForDpi(HWND__* hwnd);

    protected:
        virtual bool Initialize(const WindowProperties& props) override;
        virtual void Tick() override;
        virtual void Shutdown() override;
        virtual void Close() override;

    private:
    private:
        static Win32Window* s_win32Window;
        HWND__*             m_window      = nullptr;
        HINSTANCE__*        m_hinst       = nullptr;
        Vector2i            m_initialPos  = Vector2i();
        Vector2i            m_initialSize = Vector2i();
    };
} // namespace Lina::Graphics

#endif
