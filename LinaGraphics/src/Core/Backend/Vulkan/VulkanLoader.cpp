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

#include "Core/Backend/Vulkan/VulkanLoader.hpp"
#include "Core/Log.hpp"
#include "Core/Backend/Vulkan/VulkanFunctions.hpp"
#include "Math/Math.hpp"

namespace Lina::Graphics
{

#ifdef LINA_WINDOWS
	std::vector<char const*>  m_desiredInstanceExtensions = { VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME };
#elif LINA_LINUX
#ifdef LINA_LINUX_XLIB
	std::vector<char const*>  m_desiredInstanceExtensions = { VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_XLIB_SURFACE_EXTENSION_NAME };
#elif
	std::vector<char const*>  m_desiredInstanceExtensions = { VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_XCB_SURFACE_EXTENSION_NAME };
#endif
#endif
	std::vector<char const*>  m_desiredDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	VkPresentModeKHR m_desiredPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
	VkImageUsageFlags m_desiredSwapchainUsages = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	VkSurfaceTransformFlagBitsKHR desiredTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	VkSurfaceFormatKHR desiredSurfaceFormat;

	bool VulkanLoader::InitializeVulkan(ApplicationInfo* appInfo, VulkanData* data)
	{
		m_appInfo = appInfo;
		m_vulkanData = data;

#ifdef LINA_WINDOWS
		m_vulkanLibrary = LoadLibrary("vulkan-1.dll");
		if (m_vulkanLibrary == nullptr)
		{
			LINA_CRITICAL("[Vulkan Loader] -> Render Engine Vulkan -> Can not connect with Vulkan runtime library.");
			return false;
		}

#elif LINA_LINUX
		m_vulkanLibrary = dlopen("vulkan-1.dll");
		if (m_vulkanLibrary == nullptr)
		{
			LINA_CRITICAL("[Vulkan Loader] -> Render Engine Vulkan -> Can not connect with Vulkan runtime library.");
			return false;
		}
#endif

		if (!LoadExportedFunction())
			return false;

		LINA_TRACE("[Vulkan Loader] -> Loaded Vulkan export function.");

		if (!LoadGlobalFunctions())
			return false;

		LINA_TRACE("[Vulkan Loader] -> Loaded Vulkan global functions.");

		if (!EnumerateAvailableInstanceExtensions())
			return false;

		if (!CreateVulkanInstance())
			return false;

		LINA_TRACE("[Vulkan Loader] -> Created Vulkan instance.");

		if (!LoadInstanceFunctions())
			return false;

		LINA_TRACE("[Vulkan Loader] -> Loaded Vulkan instance functions.");

		return true;
	}

	bool VulkanLoader::CreateLogicalDevice()
	{
		std::vector<VkPhysicalDevice> availablePhysicalDevices;


		if (!EnumerateAvailablePhysicalDevices())
			return false;

		if (!ChoosePhysicalDevice())
			return false;

		if (!LoadDeviceFunctions())
			return false;

		LINA_TRACE("[Vulkan Loader] -> Loaded Vulkan device functions.");

		vkGetDeviceQueue(m_vulkanData->m_logicalDevice, m_vulkanData->m_graphicsQueueFamilyIndex, 0, &m_vulkanData->m_graphicsQueue);
		vkGetDeviceQueue(m_vulkanData->m_logicalDevice, m_vulkanData->m_computeQueueFamilyIndex, 0, &m_vulkanData->m_computeQueue);
		vkGetDeviceQueue(m_vulkanData->m_logicalDevice, m_vulkanData->m_presentationQueueFamilyIndex, 0, &m_vulkanData->m_presentationQueue);

		return true;
	}

	bool VulkanLoader::CreateVulkanInstance()
	{
		// Extension check.
		for (auto& extension : m_desiredInstanceExtensions)
		{
			if (!IsExtensionSupported(extension, m_availableInstanceExtensions))
			{
				LINA_ERR("[Vulkan Loader] -> Instance extension named '{0}' is not supported!", extension);
				return false;
			}
		}

		// App & m_instance info
		VkApplicationInfo application_info =
		{
			VK_STRUCTURE_TYPE_APPLICATION_INFO,
			nullptr,
			m_appInfo->m_appName,
			VK_MAKE_VERSION(m_appInfo->m_appMajor,m_appInfo->m_appMinor,m_appInfo->m_appPatch),
			"Lina Engine",
			VK_MAKE_VERSION(LINA_VMAJOR, LINA_VMINOR, LINA_VPATCH),
			VK_MAKE_VERSION(1, 0, 0)
		};

		VkInstanceCreateInfo instance_create_info = {
			 VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			 nullptr,
			 0,
			 &application_info,
			 0,
			 nullptr,
			 static_cast<uint32_t>(m_desiredInstanceExtensions.size()),
			 m_desiredInstanceExtensions.data()
		};

		// Create m_instance
		VkResult result = vkCreateInstance(&instance_create_info, nullptr, &m_vulkanData->m_instance);

		if ((result != VK_SUCCESS) || (m_vulkanData->m_instance == VK_NULL_HANDLE))
		{
			LINA_ERR("[Vulkan Loader] -> Could not create Vulkan instance.");
			return false;
		}
		return true;
	}

	bool VulkanLoader::EnumerateAvailableInstanceExtensions()
	{
		uint32_t extensionsCount = 0;
		VkResult result = VK_SUCCESS;

		// Get extension count.
		result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, nullptr);
		if ((result != VK_SUCCESS) || (extensionsCount == 0))
		{
			LINA_ERR("[Vulkan Loader] -> Could not get the number of instance extensions.");;
			return false;
		}

		// Store extensions.
		m_availableInstanceExtensions.resize(extensionsCount);
		result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, m_availableInstanceExtensions.data());
		if ((result != VK_SUCCESS) || (extensionsCount == 0))
		{
			LINA_ERR("[Vulkan Loader] -> Could not enumerate instance extensions.");
			return false;
		}

		LINA_TRACE("[Vulkan Loader] -> Available extensions are: ");
		for (auto& ex : m_availableInstanceExtensions)
			LINA_TRACE("[Vulkan Loader] -> {0} {1}", ex.extensionName, ex.specVersion);

		return true;
	}

	bool VulkanLoader::EnumerateAvailablePhysicalDevices()
	{
		uint32_t devicesCount = 0;
		VkResult result = VK_SUCCESS;

		result = vkEnumeratePhysicalDevices(m_vulkanData->m_instance, &devicesCount, nullptr);
		if ((result != VK_SUCCESS) || (devicesCount == 0))
		{
			LINA_ERR("[Vulkan Loader] -> Could not get the number of available physical devices.");
			return false;
		}

		m_availablePhysicalDevices.resize(devicesCount);
		result = vkEnumeratePhysicalDevices(m_vulkanData->m_instance, &devicesCount, m_availablePhysicalDevices.data());
		if ((result != VK_SUCCESS) || (devicesCount == 0))
		{
			LINA_ERR("[Vulkan Loader] -> Could not enumerate physical devices.");
			return false;
		}

		if (m_availablePhysicalDevices.size() == 0)
		{
			LINA_ERR("[Vulkan Loader] -> No physical device was found, how do you even?");
			return false;
		}

		LINA_TRACE("[Vulkan Loader] -> List of available physical devices: ");

#ifdef LINA_DEBUG_BUILD
		for (auto& device : m_availablePhysicalDevices)
		{
			VkPhysicalDeviceProperties deviceProperties;
			vkGetPhysicalDeviceProperties(device, &deviceProperties);
			LINA_TRACE("[Vulkan Loader] -> Name: {0} Type: {1} API: {2}", deviceProperties.deviceName, deviceProperties.deviceType, deviceProperties.apiVersion);
		}
#endif

		return true;
	}

	bool VulkanLoader::EnumerateAvailablePhysicalDeviceExtensions()
	{
		uint32_t extensionsCount = 0;
		VkResult result = VK_SUCCESS;

		result = vkEnumerateDeviceExtensionProperties(m_vulkanData->m_physicalDevice, nullptr, &extensionsCount, nullptr);
		if ((result != VK_SUCCESS) || (extensionsCount == 0))
		{
			LINA_ERR("[Vulkan Loader] -> Could not get the number of device extensions.");
			return false;
		}

		m_availablePhysicalDeviceExtensions.resize(extensionsCount);
		result = vkEnumerateDeviceExtensionProperties(m_vulkanData->m_physicalDevice, nullptr, &extensionsCount, m_availablePhysicalDeviceExtensions.data());
		if ((result != VK_SUCCESS) || (extensionsCount == 0))
		{
			LINA_ERR("[Vulkan Loader] -> Could not enumerate device extensions.");
			return false;
		}

		/*
		#ifdef LINA_DEBUG_BUILD
				VkPhysicalDeviceProperties deviceProperties;
				vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
				LINA_TRACE("[Vulkan Loader] -> Available extensions are for the physicalDevice {0} are: ", deviceProperties.deviceName);
				for (auto& ex : availableDeviceExtensions)
					LINA_TRACE("[Vulkan Loader] -> {0} {1}", ex.extensionName, ex.specVersion);
		#endif
		*/
	}

	bool VulkanLoader::ChoosePhysicalDevice()
	{
		for (auto& physicalDevice : m_availablePhysicalDevices)
		{
			m_vulkanData->m_physicalDevice = physicalDevice;

			// Get features & props.
			VkPhysicalDeviceFeatures deviceFeatures;
			VkPhysicalDeviceProperties deviceProperties;
			vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);
			vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

			if (!deviceFeatures.geometryShader)
				continue;
			else
			{
				deviceFeatures = {};
				deviceFeatures.geometryShader = VK_TRUE;
			}

			// Get physicalDevice queue families.
			uint32_t queueFamiliesCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamiliesCount, nullptr);
			if (queueFamiliesCount == 0)
			{
				LINA_ERR("[Vulkan Loader] -> Could not get the number of queue families.");
				return false;
			}

			// Get queue family properties.
			std::vector<VkQueueFamilyProperties> queueFamilies;
			queueFamilies.resize(queueFamiliesCount);
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamiliesCount, queueFamilies.data());
			if (queueFamiliesCount == 0)
			{
				LINA_ERR("[Vulkan Loader] -> Could not acquire properties of queue families.");
				return false;
			}

			// Check if the family contains desired capabilitieis.
			if (!SelectQueueFamilyIndexForCapability(queueFamilies, m_vulkanData->m_graphicsQueueFamilyIndex, VK_QUEUE_GRAPHICS_BIT))
				continue;

			if (!SelectQueueFamilyIndexForCapability(queueFamilies, m_vulkanData->m_computeQueueFamilyIndex, VK_QUEUE_COMPUTE_BIT))
				continue;

			// Check if the family supports presentation surface.
			if (!SelectQueueFamilyIndexForPresentation(queueFamilies))
				continue;

			// Check if the containing physicalDevice has physicalDevice extensions required.
			EnumerateAvailablePhysicalDeviceExtensions();

			// Extension check.
			for (auto& extension : m_desiredDeviceExtensions)
			{
				if (!IsExtensionSupported(extension, m_availablePhysicalDeviceExtensions))
				{
					LINA_ERR("[Vulkan Loader] -> Device extension named '{0}' is not supported!", extension);
					return false;
				}
			}

			// Store info about our desired capabilities' queues.
			std::vector<QueueInfo> queueInfos = { { m_vulkanData->m_graphicsQueueFamilyIndex, { 1.0f } } };
			if (m_vulkanData->m_graphicsQueueFamilyIndex != m_vulkanData->m_computeQueueFamilyIndex)
				queueInfos.push_back({ m_vulkanData->m_computeQueueFamilyIndex, { 1.0f } });
			if (m_vulkanData->m_presentationQueueFamilyIndex != m_vulkanData->m_graphicsQueueFamilyIndex)
				queueInfos.push_back({ m_vulkanData->m_presentationQueueFamilyIndex, {1.0f} });

			// Create queue info for vulkan based in our vector.
			std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
			for (auto& info : queueInfos) {
				queueCreateInfos.push_back(
					{
						VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
						nullptr,
						0,
						info.m_familyIndex,
						static_cast<uint32_t>(info.m_priorities.size()),
						&info.m_priorities[0]
					});
			};

			// Create physicalDevice info.
			VkDeviceCreateInfo deviceCreateInfo =
			{
			  VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			  nullptr,
			  0,
			  static_cast<uint32_t>(queueCreateInfos.size()),
			  queueCreateInfos.size() > 0 ? &queueCreateInfos[0] : nullptr,
			  0,
			  nullptr,
			  static_cast<uint32_t>(m_desiredDeviceExtensions.size()),
			  m_desiredDeviceExtensions.size() > 0 ? &m_desiredDeviceExtensions[0] : nullptr,
			  &deviceFeatures
			};

			VkResult result = vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &m_vulkanData->m_logicalDevice);
			if ((result != VK_SUCCESS) || (m_vulkanData->m_logicalDevice == VK_NULL_HANDLE))
			{
				LINA_ERR("[Vulkan Loader] -> Could not create logical device.");
				return false;
			}
			else
			{
				LINA_TRACE("[Vulkan Loader] -> Created logical device on {0}", deviceProperties.deviceName);
				return true;
			}
		}

		LINA_ERR("[Vulkan Loader] -> Could not find any physical device with requested extensions | features | queues.");
		return false;
	}

	bool VulkanLoader::SelectQueueFamilyIndexForCapability(std::vector<VkQueueFamilyProperties>& queueFamilies, uint32_t& queueFamilyIndex, VkQueueFlags capability)
	{
		for (uint32_t index = 0; index < static_cast<uint32_t>(queueFamilies.size()); ++index)
		{
			if ((queueFamilies[index].queueCount > 0) && (queueFamilies[index].queueFlags & capability))
			{
				queueFamilyIndex = index;
				return true;
			}
		}
		return false;
	}

	bool VulkanLoader::SelectQueueFamilyIndexForPresentation(std::vector<VkQueueFamilyProperties>& queueFamilies)
	{
		uint32_t index = 0;
		bool found = false;
		for (auto& family : queueFamilies)
		{
			VkBool32 presentationSupported = VK_FALSE;
			VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(m_vulkanData->m_physicalDevice, index, m_vulkanData->m_surface, &presentationSupported);

			if (result == VK_SUCCESS && presentationSupported == VK_TRUE)
			{
				m_vulkanData->m_presentationQueueFamilyIndex = index;
				return true;
			}
			index++;
		}
		return false;
	}

	bool VulkanLoader::IsExtensionSupported(const char* desiredExtension, std::vector<VkExtensionProperties>& availableExtensions)
	{
		for (auto& ex : availableExtensions)
		{
			if (strstr(ex.extensionName, desiredExtension))
				return true;
		}

		return false;
	}

	void VulkanLoader::Unload()
	{
		LINA_TRACE("[Vulkan Loader] -> Unloading vulkan instance & device.");

		// Destroy physicalDevice.
		if (m_vulkanData->m_logicalDevice)
		{
			vkDestroyDevice(m_vulkanData->m_logicalDevice, nullptr);
			m_vulkanData->m_logicalDevice = VK_NULL_HANDLE;
		}

		if (m_vulkanData->m_instance)
		{
			vkDestroyInstance(m_vulkanData->m_instance, nullptr);
			m_vulkanData->m_instance = VK_NULL_HANDLE;
		}

#ifdef LINA_WINDOWS
		FreeLibrary(m_vulkanLibrary);
#elif LINA_LINUX
		dlclose(m_vulkanLibrary);
#endif
		m_vulkanLibrary = nullptr;
	}


	bool VulkanLoader::LoadExportedFunction()
	{
#ifdef LINA_WINDOWS
#include <Windows.h>
#define LoadFunction GetProcAddress
#elif LINA_LINUX
#define LoadFunction dlsym
#endif

#define EXPORTED_VULKAN_FUNCTION( name )										\
    name = (PFN_##name)LoadFunction( m_vulkanLibrary, #name );					\
    if( name == nullptr ) {														\
      LINA_CRITICAL("[Vulkan Loader] -> Could not load exported Vulkan function named: {0}"	\
	  , #name);																	\
      return false;}															\

#include "Core/Backend/Vulkan/VulkanFunctionList.inl"
		return true;
	}

	bool VulkanLoader::LoadGlobalFunctions()
	{

#define GLOBAL_LEVEL_VULKAN_FUNCTION( name )									\
    name = (PFN_##name)vkGetInstanceProcAddr( nullptr, #name );			\
    if( name == nullptr ) {														\
      LINA_CRITICAL("[Vulkan Loader] -> Could not load exported Vulkan function named: {0}"	\
	  , #name);																	\
      return false;}															\

#include "Core/Backend/Vulkan/VulkanFunctionList.inl"

		return true;
	}


	bool VulkanLoader::LoadInstanceFunctions()
	{
		// InitializeVulkan core Vulkan API m_instance-level functions
#define INSTANCE_LEVEL_VULKAN_FUNCTION( name )													\
    name = (PFN_##name)vkGetInstanceProcAddr( m_vulkanData->m_instance, #name );								\
    if( name == nullptr ) {																		\
		LINA_ERR("[Vulkan Loader] -> Could not load instance-level Vulkan function named: {0}"  \
        ,#name);																				\
      return false;	}																			\

	// InitializeVulkan m_instance-level functions from enabled extensions
#define INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION( name, extension )							\
    for( auto& ex : m_desiredInstanceExtensions ) {															\
      if( std::string( ex ) == std::string( extension ) ) {											\
        name = (PFN_##name)vkGetInstanceProcAddr( m_vulkanData->m_instance, #name );								\
        if( name == nullptr ){																		\
          LINA_ERR("[Vulkan Loader] -> Could not load instance-level Vulkan function named: {0} "   \
            ,#name);	return false; }}}															\


#include "Core/Backend/Vulkan/VulkanFunctionList.inl"

		return true;
	}


	bool VulkanLoader::LoadDeviceFunctions()
	{
		// InitializeVulkan core Vulkan API physicalDevice-level functions
#define DEVICE_LEVEL_VULKAN_FUNCTION( name )                                    \
    name = (PFN_##name)vkGetDeviceProcAddr( m_vulkanData->m_logicalDevice, #name );				\
    if( name == nullptr ) {                                                     \
       LINA_ERR("[Vulkan Loader] -> Could not load device-level Vulkan function named: {0}"		\
        ,#name);																\
      return false;                                                             \
    }

	// InitializeVulkan physicalDevice-level functions from enabled extensions
#define DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION( name, extension )          \
    for( auto & enabledExtension : m_desiredDeviceExtensions ) {                \
      if( std::string( enabledExtension ) == std::string( extension ) ) {		\
        name = (PFN_##name)vkGetDeviceProcAddr( m_vulkanData->m_logicalDevice, #name );			\
        if( name == nullptr ) {                                                 \
          LINA_ERR("[Vulkan Loader] -> Could not load device-level Vulkan function named: {0} "    \
           ,#name);																\
          return false;                                                         \
        }                                                                       \
      }                                                                         \
    }

#include "Core/Backend/Vulkan/VulkanFunctionList.inl"

		return true;
	}
}