/* 
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

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

/*
Class: VulkanLoader

Responsible for connecting to vulkan API, loading vulkan device instances, global & device functions as well as creating a logical device.

Timestamp: 12/26/2020 9:37:35 PM
*/

#pragma once

#ifndef VulkanLoader_HPP
#define VulkanLoader_HPP

// Headers here.
#include "VulkanCommon.hpp"
#include "Core/Common.hpp"
#include "Core/MacroDetection.hpp"
#include <vector>


#ifdef LINA_WINDOWS
#include <Windows.h>
#define VULKAN_LIB_TYPE HMODULE
#elif LINA_LINUX
#endif

namespace Lina::Graphics
{
	class WindowVulkan;

	class VulkanLoader
	{		
	private:

		friend class RenderEngineVulkan;

		VulkanLoader() {};
		~VulkanLoader() {};

		bool InitializeVulkan(ApplicationInfo* appInfo, VulkanData* data);
		bool CreateLogicalDevice();

	private:

		bool CreateVulkanInstance();
		bool EnumerateAvailableInstanceExtensions();
		bool EnumerateAvailablePhysicalDevices();
		bool EnumerateAvailablePhysicalDeviceExtensions();
		bool EnumerateAvailableValidationLayers();
		bool ChoosePhysicalDevice();
		bool SelectQueueFamilyIndexForCapability(std::vector<VkQueueFamilyProperties>& queueFamilies, uint32_t& queueFamilyIndex, VkQueueFlags capability);
		bool SelectQueueFamilyIndexForPresentation(std::vector<VkQueueFamilyProperties>& queueFamilies);
		bool IsExtensionSupported(const char* ext, std::vector<VkExtensionProperties>& availableExtensions);
		bool IsValidationLayerSupported(const char* layer, std::vector<VkLayerProperties>& availableLayers);
		void Unload();
		bool LoadExportedFunction();
		bool LoadGlobalFunctions();
		bool LoadInstanceFunctions();
		bool LoadDeviceFunctions();

	private:

		ApplicationInfo* m_appInfo = nullptr;
		VULKAN_LIB_TYPE m_vulkanLibrary;
		VulkanData* m_vulkanData = nullptr;
		std::vector<VkExtensionProperties> m_availableInstanceExtensions;
		std::vector<VkExtensionProperties> m_availablePhysicalDeviceExtensions;
		std::vector<VkPhysicalDevice> m_availablePhysicalDevices;
		std::vector<VkLayerProperties> m_availableValidationLayers;
	};
}

#endif
