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

#include "Reflection/ReflectionSystem.hpp"
#include "Math/AABB.hpp"

#ifdef LINA_PLATFORM_WINDOWS
#include <Windows.h>

typedef void(__cdecl* MyFunc)(Lina::AABB&);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
{
    // Win32 stuff
    MSG msg    = {0};
    msg.wParam = 0;


    if (AllocConsole() == FALSE)
    {
        // err
    }

    HINSTANCE hinstLib;
    BOOL      fFreeResult = FALSE;

    // Get a handle to the DLL module.
    hinstLib = LoadLibrary(TEXT("GameCode.dll"));

    Lina::AABB hm;
    hm.boundsMax = Lina::Vector3(5,5,5);
    // If the handle is valid, try to get the function address.
    if (hinstLib != NULL)
    {
        MyFunc ProcAdd = (MyFunc)GetProcAddress(hinstLib, "ExampleFunc");

        // If the function address is valid, call the function.

        if (NULL != ProcAdd)
        {
            (ProcAdd)(hm);
        }
        // Free the DLL module.
    }

    fFreeResult = FreeLibrary(hinstLib);

    //   while (true)
    //   {
    //       while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    //       {
    //           TranslateMessage(&msg);
    //           DispatchMessage(&msg);
    //       }
    //
    //       if (msg.message == WM_QUIT || msg.message == WM_DESTROY)
    //           break;
    //   }

    FreeConsole();
}

#else

int main(int argc, char* argv[])
{
    return 0;
}

#endif