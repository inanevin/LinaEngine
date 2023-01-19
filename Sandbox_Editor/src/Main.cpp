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

#include "Data/String.hpp"
#include "Data/Vector.hpp"
#include "Profiling/Profiler.hpp"

class testClass;

class test2
{
private:
    friend class testClass;
    test2()
    {
    }
    ~test2(){};
};

class testClass
{
public:
    void yo()
    {
        test2* ptr = Lina::GNew<test2>();
        Lina::GDelete(ptr);
    }
    int a = 5;
};
#ifdef LINA_PLATFORM_WINDOWS
#include <Windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
{
    Lina::Profiler prof;
    Lina::Profiler::s_instance = &prof;

    // Win32 stuff
    MSG msg    = {0};
    msg.wParam = 0;

    testClass test;
    test.yo();

    if (AllocConsole() == FALSE)
    {
    }

    // while (true)
    //{
    //     while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    //     {
    //         TranslateMessage(&msg);
    //         DispatchMessage(&msg);
    //     }
    //
    //     if (msg.message == WM_QUIT || msg.message == WM_DESTROY)
    //         break;
    // }

    Lina::PROFILER_DUMP_LEAKS("Leaks.txt");
    FreeConsole();
}

#else

int main(int argc, char* argv[])
{
    return 0;
}

#endif