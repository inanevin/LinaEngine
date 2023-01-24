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

#include "Core/Application.hpp"
#include "Log/Log.hpp"

#ifdef LINA_PLATFORM_WINDOWS
#include <Windows.h>
typedef Lina::IPlugin*(__cdecl* CreatePluginFunc)();
typedef void(__cdecl* DestroyPluginFunc)(Lina::IPlugin*);
#endif

namespace Lina
{
    void Application::Initialize(const SystemInitializationInfo& initInfo)
    {
    }

    void Application::LoadPlugins()
    {
        LoadPlugin("GamePlugin.dll");
    }

    void Application::PostInitialize()
    {
    }

    void Application::UnloadPlugins()
    {
        for (auto p : m_plugins)
            UnloadPlugin(p);

        m_plugins.clear();
    }

    void Application::Shutdown()
    {
    }

    void Application::Tick()
    {
    }

    void Application::LoadPlugin(const char* name)
    {
#ifdef LINA_PLATFORM_WINDOWS
        HINSTANCE hinstLib;
        BOOL      fFreeResult = FALSE;
        hinstLib              = LoadLibrary(TEXT(name));

        // If the handle is valid, try to get the function address.
        if (hinstLib != NULL)
        {
            CreatePluginFunc createPluginAddr = (CreatePluginFunc)GetProcAddress(hinstLib, "CreatePlugin");

            // If the function address is valid, call the function.

            if (NULL != createPluginAddr)
            {
                IPlugin* plugin = (createPluginAddr)();
                plugin->OnAttached();
                AddListener(plugin);
                AddPlugin(plugin);
                m_pluginHandles[plugin] = static_cast<void*>(hinstLib);
            }
        }
#else
        LINA_ASSERT(false, "Not implemented!");
#endif
    }
    void Application::UnloadPlugin(IPlugin* plugin)
    {
        RemoveListener(plugin);
        plugin->OnDetached();

#ifdef LINA_PLATFORM_WINDOWS

        HINSTANCE hinstLib = static_cast<HINSTANCE>(m_pluginHandles[plugin]);

        DestroyPluginFunc destroyPluginAddr = (DestroyPluginFunc)GetProcAddress(hinstLib, "DestroyPlugin");

        if (destroyPluginAddr != NULL)
        {
        }

        // Free the DLL module.
        BOOL fFreeResult = FreeLibrary(hinstLib);
#else
        LINA_ASSERT(false, "Not implemented!");
#endif
    }
} // namespace Lina
