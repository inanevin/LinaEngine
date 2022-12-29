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

#ifdef LINA_PLATFORM_WINDOWS
#define VK_USE_PLATFORM_WIN32_KHR
#include "Graphics/Platform/Win32/Win32Window.hpp"
#endif

#include "Graphics/Core/Backend.hpp"
#include "Log/Log.hpp"
#include "Graphics/Utility/Vulkan/VkBootstrap.h"
#include "Graphics/Utility/Vulkan/SPIRVUtility.hpp"
#include "Graphics/Core/Window.hpp"
#include "Profiling/Profiler.hpp"
#include "EventSystem/GraphicsEvents.hpp"
#include "EventSystem/WindowEvents.hpp"
#include "EventSystem/EventSystem.hpp"

#include <vulkan/vulkan.h>

#define VMA_IMPLEMENTATION
#include "Graphics/Utility/Vulkan/vk_mem_alloc.h"

namespace Lina::Graphics
{
    Backend* Backend::s_instance = nullptr;

    static VKAPI_ATTR VkBool32 VKAPI_CALL VkDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
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

    Swapchain* Backend::CreateAdditionalSwapchain(StringID sid, void* windowPtr, int width, int height)
    {
        VkSurfaceKHR surface;

#ifdef LINA_PLATFORM_WINDOWS
        VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = VkWin32SurfaceCreateInfoKHR{
            .sType     = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
            .pNext     = nullptr,
            .hinstance = Win32Window::GetWin32()->GetInstancePtr(),
            .hwnd      = static_cast<HWND>(windowPtr),
        };
        vkCreateWin32SurfaceKHR(m_vkInstance, &surfaceCreateInfo, nullptr, &surface);
#else
        LINA_ASSERT(false, "Not implemented!");
#endif

        m_additionalSwapchains[sid] = Swapchain();
        auto& swp                   = m_additionalSwapchains[sid];

        swp = Swapchain{
            .size        = Vector2i(static_cast<uint32>(width), static_cast<uint32>(height)),
            .format      = Format::B8G8R8A8_UNORM,
            .colorSpace  = ColorSpace::SRGB_NONLINEAR,
            .presentMode = m_currentPresentMode,
        };

        swp.surface = surface;
        swp.Create();
        LINA_TRACE("[Swapchain] -> Swapchain created: {0} x {1}", swp.size.x, swp.size.y);
        return &m_additionalSwapchains[sid];
    }

    void Backend::DestroyAdditionalSwapchain(StringID sid)
    {
        auto& swp     = m_additionalSwapchains[sid];
        auto  surface = swp.surface;
        swp.Destroy();
        vkDestroySurfaceKHR(m_vkInstance, surface, m_allocator);
    }

    bool Backend::Initialize(const InitInfo& initInfo)
    {
        LINA_TRACE("[Initialization] -> Vulkan Backend ({0})", typeid(*this).name());
        m_allocator = nullptr;
        Event::EventSystem::Get()->Connect<Event::EVsyncModeChanged, &Backend::OnVsyncModeChanged>(this);

        // Total extensions
        Vector<const char*> requiredExtensions;
        requiredExtensions.push_back("VK_KHR_surface");

#ifdef LINA_DEBUG
        // Debug messenger
        requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

        // Instance builder
        vkb::InstanceBuilder builder;
        builder = builder.set_app_name(initInfo.windowProperties.title).request_validation_layers(true).require_api_version(1, 2, 0);

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

        auto res = builder.build();
        if (!res)
        {
            LINA_ERR("[Vulkan Backend] -> Vulkan builder failed!");
            return false;
        }

        vkb::Instance inst = res.value();
        m_vkInstance       = inst.instance;
        m_debugMessenger   = inst.debug_messenger;

#ifdef LINA_PLATFORM_WINDOWS

        VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = VkWin32SurfaceCreateInfoKHR{
            .sType     = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
            .pNext     = nullptr,
            .hinstance = Win32Window::GetWin32()->GetInstancePtr(),
            .hwnd      = Win32Window::GetWin32()->GetWindowPtr(),
        };
        vkCreateWin32SurfaceKHR(m_vkInstance, &surfaceCreateInfo, nullptr, &m_mainSurface);
#else
        LINA_ASSERT(false, "Not implemented");
#endif

        LINA_TRACE("[Vulkan Backend] -> Created surface.");

        vkb::PreferredDeviceType targetDeviceType = vkb::PreferredDeviceType::discrete;

        if (initInfo.preferredGPU == PreferredGPUType::CPU)
            targetDeviceType = vkb::PreferredDeviceType::cpu;
        else if (initInfo.preferredGPU == PreferredGPUType::Integrated)
            targetDeviceType = vkb::PreferredDeviceType::integrated;

        // Physical device
        VkPhysicalDeviceFeatures features{};
        features.multiDrawIndirect         = true;
        features.drawIndirectFirstInstance = true;
        features.samplerAnisotropy         = true;
        features.fillModeNonSolid          = true;

        vkb::PhysicalDeviceSelector selector{inst};
        vkb::PhysicalDevice         physicalDevice = selector.set_minimum_version(1, 1)
                                                 .set_surface(m_mainSurface)
                                                 .prefer_gpu_device_type(targetDeviceType)
                                                 .allow_any_gpu_device_type(false)
                                                 .set_required_features(features)
                                                 .select(vkb::DeviceSelectionMode::partially_and_fully_suitable)
                                                 .value();

        // create the final Vulkan device
        vkb::DeviceBuilder                           deviceBuilder{physicalDevice};
        VkPhysicalDeviceShaderDrawParametersFeatures shaderDrawParamsFeature = VkPhysicalDeviceShaderDrawParametersFeatures{
            .sType                = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES,
            .pNext                = nullptr,
            .shaderDrawParameters = VK_TRUE,
        };

        // For using UPDATE_AFTER_BIND_BIT on material bindings
        VkPhysicalDeviceDescriptorIndexingFeatures descFeatures = VkPhysicalDeviceDescriptorIndexingFeatures{
            .sType                                         = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES,
            .pNext                                         = nullptr,
            .descriptorBindingUniformBufferUpdateAfterBind = VK_TRUE,
            .descriptorBindingSampledImageUpdateAfterBind  = VK_TRUE,
        };

        deviceBuilder.add_pNext(&shaderDrawParamsFeature);
        deviceBuilder.add_pNext(&descFeatures);

        bool hasDedicatedTransferQueue = physicalDevice.has_dedicated_transfer_queue();
        bool hasDedicatedComputeQueue  = physicalDevice.has_dedicated_compute_queue();

        auto queue_families = physicalDevice.get_queue_families();

        uint32 transferQueueFamily = 0;
        uint32 transferQueueIndex  = 0;
        uint32 graphicsQueueFamily = 0;
        uint32 graphicsQueueIndex  = 0;
        uint32 computeQueueFamily  = 0;
        uint32 computeQueueIndex   = 0;

        std::vector<vkb::CustomQueueDescription> queue_descriptions;
        for (uint32_t i = 0; i < static_cast<uint32_t>(queue_families.size()); i++)
        {
            uint32 count = 1;

            if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                graphicsQueueFamily = i;

                if (queue_families[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
                {
                    transferQueueFamily = i;

                    if (queue_families[i].queueCount > count + 1)
                        count++;

                    transferQueueIndex = count - 1;
                }

                if (!hasDedicatedComputeQueue && queue_families[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
                {
                    computeQueueFamily = i;

                    if (queue_families[i].queueCount > count + 1)
                        count++;
                    computeQueueIndex = count - 1;
                }
            }

            queue_descriptions.push_back(vkb::CustomQueueDescription(i, count, std::vector<float>(count, 1.0f)));
        }

        deviceBuilder.custom_queue_setup(queue_descriptions);

        // deviceBuilder.custom_queue_setup(desc);
        vkb::Device vkbDevice = deviceBuilder.build().value();
        m_device              = vkbDevice.device;
        m_gpu                 = physicalDevice.physical_device;


        // if (hasDedicatedTransferQueue)
        // {
        //     auto res            = vkbDevice.get_dedicated_queue_index(vkb::QueueType::transfer);
        //     transferQueueFamily = res.value();
        //     transferQueueIndex  = 0;
        // }

        if (hasDedicatedComputeQueue)
        {
            auto res           = vkbDevice.get_dedicated_queue_index(vkb::QueueType::compute);
            computeQueueFamily = res.value();
            computeQueueIndex  = 0;
        }

        m_graphicsQueueIndices = linatl::make_pair(graphicsQueueFamily, graphicsQueueIndex);
        m_transferQueueIndices = linatl::make_pair(transferQueueFamily, transferQueueIndex);
        m_computeQueueIndices  = linatl::make_pair(computeQueueFamily, computeQueueIndex);

        VkPhysicalDeviceProperties gpuProps;
        vkGetPhysicalDeviceProperties(m_gpu, &gpuProps);

        VkPhysicalDeviceMemoryProperties gpuMemProps;
        vkGetPhysicalDeviceMemoryProperties(m_gpu, &gpuMemProps);

        m_minUniformBufferOffsetAlignment = gpuProps.limits.minUniformBufferOffsetAlignment;

#ifdef LINA_ENABLE_PROFILING
        DeviceGPUInfo profilerInfo;
        profilerInfo.m_memoryHeapCount = gpuMemProps.memoryHeapCount;
        profilerInfo.m_memoryHeapSize  = gpuMemProps.memoryHeaps->size;
        Profiler::Get()->SetGPUInfo(profilerInfo);
#endif

        LINA_TRACE("[Vulkan Backend] -> Selected GPU: {0} - {1} mb", gpuProps.deviceName, gpuMemProps.memoryHeaps->size / 1000000);

        m_currentPresentMode = VsyncToPresentMode(initInfo.windowProperties.vsync);
        m_mainSwapchain      = Swapchain{
                 .size        = Vector2i(static_cast<uint32>(initInfo.windowProperties.width), static_cast<uint32>(initInfo.windowProperties.height)),
                 .format      = Format::B8G8R8A8_UNORM,
                 .colorSpace  = ColorSpace::SRGB_NONLINEAR,
                 .presentMode = m_currentPresentMode,
        };

        m_mainSwapchain.surface = m_mainSurface;
        m_mainSwapchain.Create();
        LINA_TRACE("[Swapchain] -> Swapchain created: {0} x {1}", m_mainSwapchain.size.x, m_mainSwapchain.size.y);

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

        SPIRVUtility::Initialize();

        VmaAllocatorCreateInfo allocatorInfo = {};
        allocatorInfo.physicalDevice         = m_gpu;
        allocatorInfo.device                 = m_device;
        allocatorInfo.instance               = m_vkInstance;
        vmaCreateAllocator(&allocatorInfo, &m_vmaAllocator);

        m_graphicsQueue.Get(m_graphicsQueueIndices.first, m_graphicsQueueIndices.second);
        m_transferQueue.Get(m_transferQueueIndices.first, m_transferQueueIndices.second);
        m_computeQueue.Get(m_computeQueueIndices.first, m_computeQueueIndices.second);

        if (m_transferQueueIndices.first != m_graphicsQueueIndices.first)
            m_supportsAsyncTransferQueue = true;
        else if (m_transferQueueIndices.second != m_graphicsQueueIndices.second)
            m_supportsAsyncTransferQueue = true;
        else
            m_supportsAsyncTransferQueue = false;

        if (m_computeQueueIndices.first != m_graphicsQueueIndices.first)
            m_supportsAsyncComputeQueue = true;
        else if (m_computeQueueIndices.second != m_graphicsQueueIndices.second)
            m_supportsAsyncComputeQueue = true;
        else
            m_supportsAsyncComputeQueue = false;

        return true;
    }

    void Backend::OnVsyncModeChanged(const Event::EVsyncModeChanged& ev)
    {
        m_mainSwapchain.Destroy();
        m_mainSwapchain.presentMode = VsyncToPresentMode(ev.newMode);
        m_currentPresentMode        = m_mainSwapchain.presentMode;
        m_mainSwapchain.Create();
    }

    PresentMode Backend::VsyncToPresentMode(VsyncMode mode)
    {
        PresentMode pMode = PresentMode::Immediate;

        if (mode == VsyncMode::StrongVsync)
            pMode = PresentMode::FIFO;
        else if (mode == VsyncMode::Adaptive)
            pMode = PresentMode::FIFORelaxed;
        else if (mode == VsyncMode::TripleBuffer)
            pMode = PresentMode::Mailbox;

        return pMode;
    }

    void Backend::Shutdown()
    {
        LINA_TRACE("[Shutdown] -> Vulkan Backend  ({0})", typeid(*this).name());

        SPIRVUtility::Shutdown();
        Event::EventSystem::Get()->Disconnect<Event::EVsyncModeChanged>(this);

        vmaDestroyAllocator(m_vmaAllocator);

        m_mainSwapchain.Destroy();

        vkDestroyDevice(m_device, m_allocator);
        vkDestroySurfaceKHR(m_vkInstance, m_mainSurface, m_allocator);
        vkb::destroy_debug_utils_messenger(m_vkInstance, m_debugMessenger);
        vkDestroyInstance(m_vkInstance, m_allocator);
    }

    void Backend::WaitIdle()
    {
        vkDeviceWaitIdle(m_device);
    }

} // namespace Lina::Graphics
