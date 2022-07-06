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

#include "Core/Bootstrap.hpp"
#include "Log/Log.hpp"
#include "Core/CommonApplication.hpp"
#include "Data/Vector.hpp"
#include "GLFW/glfw3.h"

namespace Lina::Graphics
{

#define VK_CHECK(x)                                                   \
    do                                                                \
    {                                                                 \
        VkResult err = x;                                             \
        if (err)                                                      \
        {                                                             \
            LINA_ERR("[Vulkan Backend] -> Error detected: {0}", err); \
            abort();                                                  \
        }                                                             \
    } while (0)

    const Vector<const char*> VkValidationLayers = {
        "VK_LAYER_KHRONOS_validation"};

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

    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
    {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr)
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        else
            return VK_ERROR_EXTENSION_NOT_PRESENT;
    }

    bool Bootstrap::InitVulkan(const ApplicationInfo& appInfo, VkInstance& instance, VkAllocationCallbacks* pAllocator)
    {
        VkApplicationInfo info = VkApplicationInfo{
            .sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pApplicationName   = appInfo.windowProperties.title.c_str(),
            .applicationVersion = VK_MAKE_VERSION(LINA_MAJOR, LINA_MINOR, LINA_PATCH),
            .pEngineName        = "Lina Engine",
            .engineVersion      = VK_MAKE_VERSION(LINA_MAJOR, LINA_MINOR, LINA_PATCH),
            .apiVersion         = VK_API_VERSION_1_0};

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

        // Validation layers
        if (!CheckValidationLayers())
            LINA_ERR("[Vulkan Bootstrap] -> Not all of the requested validation layers are not available! Vulkan validation might not work.");

        VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo;
        GetDebugMessengerCreateInfo(debugMessengerCreateInfo);
#endif

        VkInstanceCreateInfo createInfo = VkInstanceCreateInfo{
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
#ifdef LINA_DEBUG
            .pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugMessengerCreateInfo,
#endif
            .pApplicationInfo        = &info,
            .enabledLayerCount       = static_cast<uint32_t>(VkValidationLayers.size()),
            .ppEnabledLayerNames     = VkValidationLayers.data(),
            .enabledExtensionCount   = static_cast<uint32_t>(requiredExtensions.size()),
            .ppEnabledExtensionNames = requiredExtensions.data(),
        };

        VkResult result = vkCreateInstance(&createInfo, pAllocator, &instance);

        if (result != VK_SUCCESS)
        {
            LINA_ERR("[Vulkan Bootstrap] -> Vulkan init failed!");
            return false;
        }

        LINA_TRACE("[Vulkan Bootstrap] -> Vulkan inited successfuly.");

#ifdef LINA_DEBUG

        // Data for querying current extensions
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

        Vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

        for (uint32_t i = 0; i < requiredExtensions.size(); i++)
        {
            VkExtensionProperties* prop = linatl::find_if(extensions.begin(), extensions.end(), [&](const VkExtensionProperties& ext) {
                return requiredExtensions[i] == ext.extensionName;
            });

            if (prop != nullptr)
                LINA_TRACE("[Vulkan Bootstrap] -> Extension added: {0}", requiredExtensions[i]);
        }

#endif

        return true;
    }

    void Bootstrap::CreateDebugMessenger(VkInstance instance, VkDebugUtilsMessengerEXT& messenger)
    {
#ifdef LINA_DEBUG
        // Debug messenger.
        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        GetDebugMessengerCreateInfo(createInfo);

        if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &messenger) != VK_SUCCESS)
            LINA_ERR("[Vulkan Bootstrap] -> Failed to create debug messenger!");
#endif
    }

    void Bootstrap::DestroyDebugMessenger(VkInstance instance, VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks* pAllocator)
    {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr)
        {
            func(instance, messenger, pAllocator);
        }
    }

    void Bootstrap::GetDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
    {
        createInfo = VkDebugUtilsMessengerCreateInfoEXT{
            .sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
            .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
            .messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
            .pfnUserCallback = VkDebugCallback,
            .pUserData       = nullptr, // Optional
        };
    }

    bool Bootstrap::CheckValidationLayers()
    {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char* layerName : VkValidationLayers)
        {
            bool layerFound = false;

            for (const auto& layerProperties : availableLayers)
            {
                if (strcmp(layerName, layerProperties.layerName) == 0)
                {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound)
            {
                return false;
            }
        }

        return true;
    }

} // namespace Lina::Graphics
