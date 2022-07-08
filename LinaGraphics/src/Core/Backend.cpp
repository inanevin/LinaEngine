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

#include "Core/Backend.hpp"
#include "Log/Log.hpp"
#include "Utility/VkBootstrap.h"
#include "Core/Window.hpp"
#include "Profiling/Profiler.hpp"
#include "EventSystem/WindowEvents.hpp"
#include "EventSystem/EventSystem.hpp"
#include <GLFW/glfw3.h>

namespace Lina::Graphics
{

    static VKAPI_ATTR VkBool32 VKAPI_CALL VkDebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT             messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void*                                       pUserData)
    {
        switch (messageSeverity)
        {
        case VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            LINA_ERR("[Vulkan] -> Validation Layer: {0}", pCallbackData->pMessage);
            break;
        case VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            LINA_WARN("[Vulkan] -> Validation Layer: {0}", pCallbackData->pMessage);
            break;
        default:
            LINA_INFO("[Vulkan] -> Validation Layer: {0}", pCallbackData->pMessage);
            break;
        }
        return VK_FALSE;
    }

    bool Backend::Initialize(const ApplicationInfo& appInfo)
    {
        LINA_TRACE("[Initialization] -> Vulkan Backend ({0})", typeid(*this).name());
        m_appInfo   = appInfo;
        m_allocator = nullptr;
        Event::EventSystem::Get()->Connect<Event::EWindowResized, &Backend::OnWindowResized>(this);

        // Data for glfw extensions.
        uint32_t     glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        // Total extensions
        Vector<const char*> requiredExtensions;
        for (uint32_t i = 0; i < glfwExtensionCount; i++)
            requiredExtensions.push_back(glfwExtensions[i]);

#ifdef LINA_DEBUG
        // Debug messenger
        requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

        // Instance builder
        vkb::InstanceBuilder builder;
        builder = builder.set_app_name(appInfo.windowProperties.title.c_str()).request_validation_layers(true).require_api_version(1, 1, 0);

        // Extensions
        for (auto ext : requiredExtensions)
            builder.enable_extension(ext);

        // Messenger
        builder.set_debug_callback(VkDebugCallback);

#ifdef LINA_DEBUG
        // VkDebugUtilsMessageSeverityFlagsEXT severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
        VkDebugUtilsMessageSeverityFlagsEXT severity = 0;
#else
        VkDebugUtilsMessageSeverityFlagsEXT severity = 0;
#endif
        builder.set_debug_messenger_severity(severity | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT);
        builder.set_debug_messenger_type(VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT);

        vkb::Instance inst = builder.build().value();
        m_vkInstance       = inst.instance;
        m_debugMessenger   = inst.debug_messenger;

        VkResult glfwResult = glfwCreateWindowSurface(m_vkInstance, Window::Get()->GetGLFWWindow(), m_allocator, &m_surface);

        if (glfwResult == VK_ERROR_INITIALIZATION_FAILED)
        {
            LINA_ERR("[Vulkan Backend] -> Could not create a Vulkan surface!");
            return false;
        }
        else if (glfwResult == VK_ERROR_EXTENSION_NOT_PRESENT)
        {
            LINA_ERR("[Vulkan Backend] -> Could not create a Vulkan surface because the required GLFW extensions were not present!");
            return false;
        }

        LINA_TRACE("[Vulkan Backend] -> Created GLFW surface.");

        vkb::PreferredDeviceType targetDeviceType = vkb::PreferredDeviceType::discrete;

        // if (appInfo.preferredGPU == PreferredGPUType::CPU)
        //     targetDeviceType = vkb::PreferredDeviceType::cpu;
        // else if (appInfo.preferredGPU == PreferredGPUType::Integrated)
        //     targetDeviceType = vkb::PreferredDeviceType::integrated;

        // Physical device
        vkb::PhysicalDeviceSelector selector{inst};
        vkb::PhysicalDevice         physicalDevice = selector
                                                 .set_minimum_version(1, 1)
                                                 .set_surface(m_surface)
                                                 .prefer_gpu_device_type(targetDeviceType)
                                                 .allow_any_gpu_device_type(false)
                                                 .select(vkb::DeviceSelectionMode::partially_and_fully_suitable)
                                                 .value();

        // create the final Vulkan device
        vkb::DeviceBuilder deviceBuilder{physicalDevice};
        vkb::Device        vkbDevice = deviceBuilder.build().value();
        m_device                     = vkbDevice.device;
        m_gpu                        = physicalDevice.physical_device;

        VkPhysicalDeviceProperties gpuProps;
        vkGetPhysicalDeviceProperties(m_gpu, &gpuProps);

        VkPhysicalDeviceMemoryProperties gpuMemProps;
        vkGetPhysicalDeviceMemoryProperties(m_gpu, &gpuMemProps);

#ifdef LINA_ENABLE_PROFILING
        DeviceGPUInfo profilerInfo;
        profilerInfo.m_memoryHeapCount = gpuMemProps.memoryHeapCount;
        profilerInfo.m_memoryHeapSize  = gpuMemProps.memoryHeaps->size;
        Profiler::Get()->SetGPUInfo(profilerInfo);
#endif

        LINA_TRACE("[Vulkan Backend] -> Selected GPU: {0} - {1} mb", gpuProps.deviceName, gpuMemProps.memoryHeaps->size / 1000000);

        m_swapchain = Swapchain{
            .width       = static_cast<uint32>(appInfo.windowProperties.width),
            .height      = static_cast<uint32>(appInfo.windowProperties.height),
            .format      = Format::B8G8R8A8_SRGB,
            .colorSpace  = ColorSpace::SRGB_NONLINEAR,
            .presentMode = PresentMode::Immediate,
        };
        m_swapchain.Create(m_gpu, m_device, m_surface);
        LINA_TRACE("[Swapchain] -> Swapchain created: {0}x{1}", m_swapchain.width, m_swapchain.height);

        // Query queue family indices.
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(m_gpu, &queueFamilyCount, nullptr);

        Vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(m_gpu, &queueFamilyCount, queueFamilies.data());

        uint32 i = 0;
        for (const auto& queueFamily : queueFamilies)
        {
            QueueFamily family = QueueFamily{.flags = queueFamily.queueFlags, .count = queueFamily.queueCount};
            m_queueFamilies.push_back(family);
            i++;
        }

        return true;
    }

    void Backend::OnWindowResized(const Event::EWindowResized& ev)
    {
        m_swapchain.Destroy(m_device, m_allocator);
        m_swapchain.width  = static_cast<uint32>(ev.newSize.x);
        m_swapchain.height = static_cast<uint32>(ev.newSize.y);
        m_swapchain.Create(m_gpu, m_device, m_surface);
    }

    void Backend::Shutdown()
    {
        LINA_TRACE("[Shutdown] -> Vulkan Backend  ({0})", typeid(*this).name());

        m_swapchain.Destroy(m_device, m_allocator);
        vkDestroyDevice(m_device, m_allocator);
        vkDestroySurfaceKHR(m_vkInstance, m_surface, m_allocator);
        vkb::destroy_debug_utils_messenger(m_vkInstance, m_debugMessenger);
        vkDestroyInstance(m_vkInstance, m_allocator);
    }

    uint32 Backend::GetQueueFamilyIndex(QueueFamilies family)
    {
        uint32 index = 0;
        for (auto& f : m_queueFamilies)
        {
            if (f.flags & static_cast<VkQueueFlagBits>(family))
                return index;
            index++;
        }

        LINA_ASSERT(false, "Requested queue family does not exists in the device! {0}", static_cast<uint32>(family));
        return 0;
    }

} // namespace Lina::Graphics
