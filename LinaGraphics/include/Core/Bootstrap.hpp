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

#ifndef Bootstrap_HPP
#define Bootstrap_HPP
#include "vulkan/vulkan.h"

namespace Lina
{
    struct ApplicationInfo;
}
namespace Lina::Graphics
{
    class Bootstrap
    {

    private:
        friend class Backend;

        Bootstrap()  = default;
        ~Bootstrap() = default;

        static bool InitVulkan(const ApplicationInfo& appInfo, VkInstance& instance, VkAllocationCallbacks* pAllocator);
        static bool CheckValidationLayers();
        static void CreateDebugMessenger(VkInstance inst, VkDebugUtilsMessengerEXT& messenger);
        static void DestroyDebugMessenger(VkInstance instance, VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks* pAllocator);
        static void GetDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    };
} // namespace Lina::Graphics

#endif
