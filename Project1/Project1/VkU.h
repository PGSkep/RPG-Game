#ifndef	VKU_H
#define VKU_H

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan\vulkan.h>
#define VK_RESERVED_FOR_FUTURE_USE 0
#define VK_FLAGS_NONE 0

#include <string>
#include <vector>
#include <fstream>

#include "Error.h"
#include "Console.h"

namespace VkU
{
	std::string GetStrFromVkResult(VkResult _result)
	{
		switch (_result)
		{
		case VK_SUCCESS:							return "VK_SUCCESS";
		case VK_NOT_READY:							return "VK_NOT_READY";
		case VK_TIMEOUT:							return "VK_TIMEOUT";
		case VK_EVENT_SET:							return "VK_EVENT_SET";
		case VK_EVENT_RESET:						return "VK_EVENT_RESET";
		case VK_INCOMPLETE:							return "VK_INCOMPLETE";
		case VK_ERROR_OUT_OF_HOST_MEMORY:			return "VK_ERROR_OUT_OF_HOST_MEMORY";
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:			return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
		case VK_ERROR_INITIALIZATION_FAILED:		return "VK_ERROR_INITIALIZATION_FAILED";
		case VK_ERROR_DEVICE_LOST:					return "VK_ERROR_DEVICE_LOST";
		case VK_ERROR_MEMORY_MAP_FAILED:			return "VK_ERROR_MEMORY_MAP_FAILED";
		case VK_ERROR_LAYER_NOT_PRESENT:			return "VK_ERROR_LAYER_NOT_PRESENT";
		case VK_ERROR_EXTENSION_NOT_PRESENT:		return "VK_ERROR_EXTENSION_NOT_PRESENT";
		case VK_ERROR_FEATURE_NOT_PRESENT:			return "VK_ERROR_FEATURE_NOT_PRESENT";
		case VK_ERROR_INCOMPATIBLE_DRIVER:			return "VK_ERROR_INCOMPATIBLE_DRIVER";
		case VK_ERROR_TOO_MANY_OBJECTS:				return "VK_ERROR_TOO_MANY_OBJECTS";
		case VK_ERROR_FORMAT_NOT_SUPPORTED:			return "VK_ERROR_FORMAT_NOT_SUPPORTED";
		case VK_ERROR_FRAGMENTED_POOL:				return "VK_ERROR_FRAGMENTED_POOL";
		case VK_ERROR_SURFACE_LOST_KHR:				return "VK_ERROR_SURFACE_LOST_KHR";
		case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:		return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
		case VK_SUBOPTIMAL_KHR:						return "VK_SUBOPTIMAL_KHR";
		case VK_ERROR_OUT_OF_DATE_KHR:				return "VK_ERROR_OUT_OF_DATE_KHR";
		case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:		return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
		case VK_ERROR_VALIDATION_FAILED_EXT:		return "VK_ERROR_VALIDATION_FAILED_EXT";
		case VK_ERROR_INVALID_SHADER_NV:			return "VK_ERROR_INVALID_SHADER_NV";
		case VK_ERROR_OUT_OF_POOL_MEMORY_KHR:		return "VK_ERROR_OUT_OF_POOL_MEMORY_KHR";
		case VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR:	return "VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR";
		default: return "UNKNOWN";
		}
	}
	inline VkPhysicalDeviceFeatures PhysicalDevivceGetFeaturesFromStrArr(std::vector<const char*> _featureNames)
	{
		VkPhysicalDeviceFeatures physicalDeviceFeatures = {};

		for (size_t i = 0; i != _featureNames.size(); ++i)
		{
			if (strcmp(_featureNames[i], "robustBufferAccess") == 0)
				physicalDeviceFeatures.robustBufferAccess = VK_TRUE;
			if (strcmp(_featureNames[i], "fullDrawIndexUint32") == 0)
				physicalDeviceFeatures.fullDrawIndexUint32 = VK_TRUE;
			if (strcmp(_featureNames[i], "imageCubeArray") == 0)
				physicalDeviceFeatures.imageCubeArray = VK_TRUE;
			if (strcmp(_featureNames[i], "independentBlend") == 0)
				physicalDeviceFeatures.independentBlend = VK_TRUE;
			if (strcmp(_featureNames[i], "geometryShader") == 0)
				physicalDeviceFeatures.geometryShader = VK_TRUE;
			if (strcmp(_featureNames[i], "tessellationShader") == 0)
				physicalDeviceFeatures.tessellationShader = VK_TRUE;
			if (strcmp(_featureNames[i], "sampleRateShading") == 0)
				physicalDeviceFeatures.sampleRateShading = VK_TRUE;
			if (strcmp(_featureNames[i], "dualSrcBlend") == 0)
				physicalDeviceFeatures.dualSrcBlend = VK_TRUE;
			if (strcmp(_featureNames[i], "logicOp") == 0)
				physicalDeviceFeatures.logicOp = VK_TRUE;
			if (strcmp(_featureNames[i], "multiDrawIndirect") == 0)
				physicalDeviceFeatures.multiDrawIndirect = VK_TRUE;
			if (strcmp(_featureNames[i], "drawIndirectFirstInstance") == 0)
				physicalDeviceFeatures.drawIndirectFirstInstance = VK_TRUE;
			if (strcmp(_featureNames[i], "depthClamp") == 0)
				physicalDeviceFeatures.depthClamp = VK_TRUE;
			if (strcmp(_featureNames[i], "depthBiasClamp") == 0)
				physicalDeviceFeatures.depthBiasClamp = VK_TRUE;
			if (strcmp(_featureNames[i], "fillModeNonSolid") == 0)
				physicalDeviceFeatures.fillModeNonSolid = VK_TRUE;
			if (strcmp(_featureNames[i], "depthBounds") == 0)
				physicalDeviceFeatures.depthBounds = VK_TRUE;
			if (strcmp(_featureNames[i], "wideLines") == 0)
				physicalDeviceFeatures.wideLines = VK_TRUE;
			if (strcmp(_featureNames[i], "largePoints") == 0)
				physicalDeviceFeatures.largePoints = VK_TRUE;
			if (strcmp(_featureNames[i], "alphaToOne") == 0)
				physicalDeviceFeatures.alphaToOne = VK_TRUE;
			if (strcmp(_featureNames[i], "multiViewport") == 0)
				physicalDeviceFeatures.multiViewport = VK_TRUE;
			if (strcmp(_featureNames[i], "samplerAnisotropy") == 0)
				physicalDeviceFeatures.samplerAnisotropy = VK_TRUE;
			if (strcmp(_featureNames[i], "textureCompressionETC2") == 0)
				physicalDeviceFeatures.textureCompressionETC2 = VK_TRUE;
			if (strcmp(_featureNames[i], "textureCompressionASTC_LDR") == 0)
				physicalDeviceFeatures.textureCompressionASTC_LDR = VK_TRUE;
			if (strcmp(_featureNames[i], "textureCompressionBC") == 0)
				physicalDeviceFeatures.textureCompressionBC = VK_TRUE;
			if (strcmp(_featureNames[i], "occlusionQueryPrecise") == 0)
				physicalDeviceFeatures.occlusionQueryPrecise = VK_TRUE;
			if (strcmp(_featureNames[i], "pipelineStatisticsQuery") == 0)
				physicalDeviceFeatures.pipelineStatisticsQuery = VK_TRUE;
			if (strcmp(_featureNames[i], "vertexPipelineStoresAndAtomics") == 0)
				physicalDeviceFeatures.vertexPipelineStoresAndAtomics = VK_TRUE;
			if (strcmp(_featureNames[i], "fragmentStoresAndAtomics") == 0)
				physicalDeviceFeatures.fragmentStoresAndAtomics = VK_TRUE;
			if (strcmp(_featureNames[i], "shaderTessellationAndGeometryPointSize") == 0)
				physicalDeviceFeatures.shaderTessellationAndGeometryPointSize = VK_TRUE;
			if (strcmp(_featureNames[i], "shaderImageGatherExtended") == 0)
				physicalDeviceFeatures.shaderImageGatherExtended = VK_TRUE;
			if (strcmp(_featureNames[i], "shaderStorageImageExtendedFormats") == 0)
				physicalDeviceFeatures.shaderStorageImageExtendedFormats = VK_TRUE;
			if (strcmp(_featureNames[i], "shaderStorageImageMultisample") == 0)
				physicalDeviceFeatures.shaderStorageImageMultisample = VK_TRUE;
			if (strcmp(_featureNames[i], "shaderStorageImageReadWithoutFormat") == 0)
				physicalDeviceFeatures.shaderStorageImageReadWithoutFormat = VK_TRUE;
			if (strcmp(_featureNames[i], "shaderStorageImageWriteWithoutFormat") == 0)
				physicalDeviceFeatures.shaderStorageImageWriteWithoutFormat = VK_TRUE;
			if (strcmp(_featureNames[i], "shaderUniformBufferArrayDynamicIndexing") == 0)
				physicalDeviceFeatures.shaderUniformBufferArrayDynamicIndexing = VK_TRUE;
			if (strcmp(_featureNames[i], "shaderSampledImageArrayDynamicIndexing") == 0)
				physicalDeviceFeatures.shaderSampledImageArrayDynamicIndexing = VK_TRUE;
			if (strcmp(_featureNames[i], "shaderStorageBufferArrayDynamicIndexing") == 0)
				physicalDeviceFeatures.shaderStorageBufferArrayDynamicIndexing = VK_TRUE;
			if (strcmp(_featureNames[i], "shaderStorageImageArrayDynamicIndexing") == 0)
				physicalDeviceFeatures.shaderStorageImageArrayDynamicIndexing = VK_TRUE;
			if (strcmp(_featureNames[i], "shaderClipDistance") == 0)
				physicalDeviceFeatures.shaderClipDistance = VK_TRUE;
			if (strcmp(_featureNames[i], "shaderCullDistance") == 0)
				physicalDeviceFeatures.shaderCullDistance = VK_TRUE;
			if (strcmp(_featureNames[i], "shaderFloat64") == 0)
				physicalDeviceFeatures.shaderFloat64 = VK_TRUE;
			if (strcmp(_featureNames[i], "shaderInt64") == 0)
				physicalDeviceFeatures.shaderInt64 = VK_TRUE;
			if (strcmp(_featureNames[i], "shaderInt16") == 0)
				physicalDeviceFeatures.shaderInt16 = VK_TRUE;
			if (strcmp(_featureNames[i], "shaderResourceResidency") == 0)
				physicalDeviceFeatures.shaderResourceResidency = VK_TRUE;
			if (strcmp(_featureNames[i], "shaderResourceMinLod") == 0)
				physicalDeviceFeatures.shaderResourceMinLod = VK_TRUE;
			if (strcmp(_featureNames[i], "sparseBinding") == 0)
				physicalDeviceFeatures.sparseBinding = VK_TRUE;
			if (strcmp(_featureNames[i], "sparseResidencyBuffer") == 0)
				physicalDeviceFeatures.sparseResidencyBuffer = VK_TRUE;
			if (strcmp(_featureNames[i], "sparseResidencyImage2D") == 0)
				physicalDeviceFeatures.sparseResidencyImage2D = VK_TRUE;
			if (strcmp(_featureNames[i], "sparseResidencyImage3D") == 0)
				physicalDeviceFeatures.sparseResidencyImage3D = VK_TRUE;
			if (strcmp(_featureNames[i], "sparseResidency2Samples") == 0)
				physicalDeviceFeatures.sparseResidency2Samples = VK_TRUE;
			if (strcmp(_featureNames[i], "sparseResidency4Samples") == 0)
				physicalDeviceFeatures.sparseResidency4Samples = VK_TRUE;
			if (strcmp(_featureNames[i], "sparseResidency8Samples") == 0)
				physicalDeviceFeatures.sparseResidency8Samples = VK_TRUE;
			if (strcmp(_featureNames[i], "sparseResidency16Samples") == 0)
				physicalDeviceFeatures.sparseResidency16Samples = VK_TRUE;
			if (strcmp(_featureNames[i], "sparseResidencyAliased") == 0)
				physicalDeviceFeatures.sparseResidencyAliased = VK_TRUE;
			if (strcmp(_featureNames[i], "variableMultisampleRate") == 0)
				physicalDeviceFeatures.variableMultisampleRate = VK_TRUE;
			if (strcmp(_featureNames[i], "inheritedQueries") == 0)
				physicalDeviceFeatures.inheritedQueries = VK_TRUE;
		}

		return physicalDeviceFeatures;
	}
	uint32_t FindMemoryType(VkPhysicalDeviceMemoryProperties _physicalDeviceMemoryProperties, uint32_t _memoryTypeBits, VkMemoryPropertyFlags _memoryPropertyFlags)
	{
		for (uint32_t i = 0; i < _physicalDeviceMemoryProperties.memoryTypeCount; i++)
			if ((_memoryTypeBits & (1 << i)) && (_physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & _memoryPropertyFlags) == _memoryPropertyFlags)
				return i;

		return -1;
	}

	std::vector<Error> GetShaderCode(const char* _filename, std::vector<char>& _code)
	{
		std::vector<Error> error;
		size_t fileSize = 0;

		std::ifstream file(_filename, std::ios::ate | std::ios::binary);

		if (file.good())
		{
			fileSize = (size_t)file.tellg();
			file.seekg(0);

			if (file.good())
			{
				_code.resize(fileSize);
				file.read(_code.data(), fileSize);
				file.close();
			}
			else
			{
				file.close();
				PUSH_ERROR(_filename);
				return error;
			}
		}
		else
		{
			file.close();
			PUSH_ERROR(_filename);
			return error;
		}

		return error;
	}

	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugReportCallback(VkDebugReportFlagsEXT _flags, VkDebugReportObjectTypeEXT _objType, uint64_t _obj, size_t _location, int32_t _code, const char* _layerPrefix, const char* _msg, void* _userData)
	{
		if (_flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (Con::Color::RED + Con::Intensity::HIGH) + ((Con::Color::BLACK + Con::Intensity::LOW) * 16)); std::cout << "ERROR:";
		}
		else if (_flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) {
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (Con::Color::PINK + Con::Intensity::HIGH) + ((Con::Color::BLACK + Con::Intensity::LOW) * 16)); std::cout << "PERFORMANCE:";
		}
		else if (_flags & VK_DEBUG_REPORT_WARNING_BIT_EXT) {
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (Con::Color::YELLOW + Con::Intensity::HIGH) + ((Con::Color::BLACK + Con::Intensity::LOW) * 16)); std::cout << "WARNING:";
		}
		else if (_flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT) {
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (Con::Color::GREEN + Con::Intensity::HIGH) + ((Con::Color::BLACK + Con::Intensity::LOW) * 16)); std::cout << "	DEBUG:";
		}
		else if (_flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT) {
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (Con::Color::WHITE + Con::Intensity::HIGH) + ((Con::Color::BLACK + Con::Intensity::LOW) * 16)); std::cout << "	INFORMATION:";
		}

		std::cout << _msg << '\n';
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (Con::Color::WHITE + Con::Intensity::LOW) + ((Con::Color::BLACK + Con::Intensity::LOW) * 16));

		return VK_FALSE; // VK_FALSE wont't abort the function that made this call
	}
	static LRESULT WINAPI MainWndProc(HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam)
	{
		return DefWindowProc(_hWnd, _uMsg, _wParam, _lParam);
	}

	std::vector<Error> CreateInstance(VkInstanceCreateInfo* _pCreateInfo, VkInstance& _pInstance)
	{
		std::vector<Error> error;

		VkResult vkResult = vkCreateInstance(_pCreateInfo, nullptr, &_pInstance);
		switch (vkResult)
		{
		case VK_SUCCESS:						break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:		PUSH_ERROR("");
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:		PUSH_ERROR("");
		case VK_ERROR_INITIALIZATION_FAILED:	PUSH_ERROR("");
		case VK_ERROR_LAYER_NOT_PRESENT:		PUSH_ERROR("");
		case VK_ERROR_EXTENSION_NOT_PRESENT:	PUSH_ERROR("");
		case VK_ERROR_INCOMPATIBLE_DRIVER:		PUSH_ERROR("");
		default:								PUSH_DEFAULT(vkResult);
		}

		return error;
	}
	std::vector<Error> CreateDebugReportCallbackEXT(VkInstance _instance, const VkDebugReportCallbackCreateInfoEXT* _pCreateInfo, VkDebugReportCallbackEXT& _pCallback)
	{
		std::vector<Error> error;

		PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT_FP = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(_instance, "vkCreateDebugReportCallbackEXT");
		if (vkCreateDebugReportCallbackEXT_FP == nullptr)
		{
			PUSH_ERROR("vkCreateDebugReportCallbackFP == nullptr"); return error;
		}
		VkResult vkResult = vkCreateDebugReportCallbackEXT_FP(_instance, _pCreateInfo, nullptr, &_pCallback);
		switch (vkResult)
		{
		case VK_SUCCESS:	break;
		default:			PUSH_DEFAULT(vkResult);
		}

		return error;
	}
	std::vector<Error> CreateWin32SurfaceKHR(VkInstance _instance, const VkWin32SurfaceCreateInfoKHR* _pCreateInfo, VkSurfaceKHR& _surface)
	{
		std::vector<Error> error;

		VkResult vkResult = vkCreateWin32SurfaceKHR(_instance, _pCreateInfo, nullptr, &_surface);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	PUSH_ERROR("");
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	PUSH_ERROR("");
		default:							PUSH_DEFAULT(vkResult);
		}

		return error;
	}
	std::vector<Error> EnumeratePhysicalDevices(VkInstance _instance, std::vector<VkPhysicalDevice>& _physicalDevices)
	{
		std::vector<Error> error;

		uint32_t propertyCount = 0;
		VkResult vkResult = vkEnumeratePhysicalDevices(_instance, &propertyCount, nullptr);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_INCOMPLETE:					PUSH_ERROR("");
		case VK_ERROR_OUT_OF_HOST_MEMORY:	PUSH_ERROR(""); return error;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	PUSH_ERROR(""); return error;
		default:							PUSH_ERROR("");
		}

		_physicalDevices.resize(propertyCount);
		vkResult = vkEnumeratePhysicalDevices(_instance, &propertyCount, _physicalDevices.data());
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_INCOMPLETE:					PUSH_ERROR("");
		case VK_ERROR_OUT_OF_HOST_MEMORY:	PUSH_ERROR(""); return error;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	PUSH_ERROR(""); return error;
		default:							PUSH_ERROR("");
		}

		return error;
	}
	std::vector<Error> GetPhysicalDeviceSurfaceSupportKHR(VkPhysicalDevice _physicalDevice, uint32_t _queueFamilyIndex, VkSurfaceKHR _surface, VkBool32* _pSupported)
	{
		std::vector<Error> error;

		VkResult vkResult = vkGetPhysicalDeviceSurfaceSupportKHR(_physicalDevice, _queueFamilyIndex, _surface, _pSupported);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	PUSH_ERROR(""); return error;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	PUSH_ERROR(""); return error;
		case VK_ERROR_SURFACE_LOST_KHR:		PUSH_ERROR(""); return error;
		default:							PUSH_ERROR("");
		}

		return error;
	}
	std::vector<Error> GetPhysicalDeviceSurfacePresentModesKHR(VkPhysicalDevice _physicalDevice, VkSurfaceKHR _surface, std::vector<VkPresentModeKHR>& _pPresentModes)
	{
		std::vector<Error> error;

		uint32_t propertyCount = 0;
		VkResult vkResult = vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalDevice, _surface, &propertyCount, nullptr);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	PUSH_ERROR("");	return error;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	PUSH_ERROR("");	return error;
		case VK_ERROR_SURFACE_LOST_KHR:		PUSH_ERROR("");	return error;
		case VK_INCOMPLETE:					PUSH_ERROR("");	break;
		default:							PUSH_DEFAULT;	break;
		}
		_pPresentModes.resize(propertyCount);
		vkResult = vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalDevice, _surface, &propertyCount, _pPresentModes.data());
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	PUSH_ERROR("");	return error;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	PUSH_ERROR("");	return error;
		case VK_ERROR_SURFACE_LOST_KHR:		PUSH_ERROR("");	return error;
		case VK_INCOMPLETE:					PUSH_ERROR("");	break;
		default:							PUSH_DEFAULT;	break;
		}

		return error;
	}
	std::vector<Error> EnumerateDeviceLayerProperties(VkPhysicalDevice _physicalDevice, std::vector<VkLayerProperties>& _layers)
	{
		std::vector<Error> error;

		uint32_t propertyCount = 0;
		VkResult vkResult = vkEnumerateDeviceLayerProperties(_physicalDevice, &propertyCount, nullptr);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_INCOMPLETE:					PUSH_ERROR("");
		case VK_ERROR_OUT_OF_HOST_MEMORY:	PUSH_ERROR(""); return error;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	PUSH_ERROR(""); return error;
		default:							PUSH_ERROR("");
		}

		_layers.resize(propertyCount);
		vkResult = vkEnumerateDeviceLayerProperties(_physicalDevice, &propertyCount, _layers.data());
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_INCOMPLETE:					PUSH_ERROR("");
		case VK_ERROR_OUT_OF_HOST_MEMORY:	PUSH_ERROR(""); return error;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	PUSH_ERROR(""); return error;
		default:							PUSH_ERROR("");
		}

		return error;
	}
	std::vector<Error> EnumerateDeviceExtensionProperties(VkPhysicalDevice _physicalDevice, std::vector<VkExtensionProperties>& _extensions)
	{
		std::vector<Error> error;

		uint32_t propertyCount = 0;
		VkResult vkResult = vkEnumerateDeviceExtensionProperties(_physicalDevice, nullptr, &propertyCount, nullptr);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_INCOMPLETE:					PUSH_ERROR("");
		case VK_ERROR_OUT_OF_HOST_MEMORY:	PUSH_ERROR(""); return error;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	PUSH_ERROR(""); return error;
		case VK_ERROR_LAYER_NOT_PRESENT:	PUSH_ERROR(""); return error;
		default:							PUSH_ERROR("");
		}

		_extensions.resize(propertyCount);
		vkResult = vkEnumerateDeviceExtensionProperties(_physicalDevice, nullptr, &propertyCount, _extensions.data());
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_INCOMPLETE:					PUSH_ERROR("");
		case VK_ERROR_OUT_OF_HOST_MEMORY:	PUSH_ERROR(""); return error;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	PUSH_ERROR(""); return error;
		case VK_ERROR_LAYER_NOT_PRESENT:	PUSH_ERROR(""); return error;
		default:							PUSH_ERROR("");
		}

		return error;
	}
	std::vector<Error> CreateDevice(VkPhysicalDevice _physicalDevice, const VkDeviceCreateInfo* _pCreateInfo, VkDevice& _pDevice)
	{
		std::vector<Error> error;

		VkResult vkResult = vkCreateDevice(_physicalDevice, _pCreateInfo, nullptr, &_pDevice);
		switch (vkResult)
		{
		case VK_SUCCESS:						break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:		PUSH_ERROR("");	return error;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:		PUSH_ERROR("");	return error;
		case VK_ERROR_INITIALIZATION_FAILED:	PUSH_ERROR("");	return error;
		case VK_ERROR_EXTENSION_NOT_PRESENT:	PUSH_ERROR("");	return error;
		case VK_ERROR_FEATURE_NOT_PRESENT:		PUSH_ERROR("");	return error;
		case VK_ERROR_TOO_MANY_OBJECTS:			PUSH_ERROR("");	return error;
		default:								PUSH_DEFAULT;	break;
		}

		return error;
	}
	std::vector<Error> GetPhysicalDeviceSurfaceCapabilitiesKHR(VkPhysicalDevice _physicalDevice, VkSurfaceKHR _surface, VkSurfaceCapabilitiesKHR* _pSurfaceCapabilities)
	{
		std::vector<Error> error;

		VkResult vkResult = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_physicalDevice, _surface, _pSurfaceCapabilities);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	PUSH_ERROR("");	return error;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	PUSH_ERROR("");	return error;
		case VK_ERROR_SURFACE_LOST_KHR:		PUSH_ERROR("");	return error;
		default:							PUSH_DEFAULT;	break;
		}

		return error;
	}
	std::vector<Error> GetPhysicalDeviceSurfaceFormatsKHR(VkPhysicalDevice _physicalDevice, VkSurfaceKHR _surface, std::vector<VkSurfaceFormatKHR>& _pSurfaceFormats)
	{
		std::vector<Error> error;

		uint32_t propertyCount = 0;
		VkResult vkResult = vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalDevice, _surface, &propertyCount, nullptr);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	PUSH_ERROR("");	return error;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	PUSH_ERROR("");	return error;
		case VK_ERROR_SURFACE_LOST_KHR:		PUSH_ERROR("");	return error;
		case VK_INCOMPLETE:					PUSH_ERROR("");	break;
		default:							PUSH_DEFAULT;	break;
		}
		_pSurfaceFormats.resize(propertyCount);
		vkResult = vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalDevice, _surface, &propertyCount, _pSurfaceFormats.data());
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	PUSH_ERROR("");	return error;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	PUSH_ERROR("");	return error;
		case VK_ERROR_SURFACE_LOST_KHR:		PUSH_ERROR("");	return error;
		case VK_INCOMPLETE:					PUSH_ERROR("");	break;
		default:							PUSH_DEFAULT;	break;
		}

		return error;
	}
	std::vector<Error> CreateRenderPass(VkDevice _device, const VkRenderPassCreateInfo* _pCreateInfo, VkRenderPass* _pRenderPass)
	{
		std::vector<Error> error;

		VkResult vkResult = vkCreateRenderPass(_device, _pCreateInfo, nullptr, _pRenderPass);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	PUSH_ERROR("");	return error;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	PUSH_ERROR("");	return error;
		default:							PUSH_DEFAULT;	break;
		}

		return error;
	}
	std::vector<Error> GetSwapchainImagesKHR(VkDevice _device, VkSwapchainKHR _swapchain, std::vector<VkImage>& _images)
	{
		std::vector<Error> error;

		uint32_t propertyCount = 0;
		VkResult vkResult = vkGetSwapchainImagesKHR(_device, _swapchain, &propertyCount, nullptr);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	PUSH_ERROR("");	return error;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	PUSH_ERROR("");	return error;
		case VK_INCOMPLETE:					PUSH_ERROR("");	break;
		default:							PUSH_DEFAULT;	break;
		}
		_images.resize(propertyCount);
		vkResult = vkGetSwapchainImagesKHR(_device, _swapchain, &propertyCount, _images.data());
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	PUSH_ERROR("");	return error;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	PUSH_ERROR("");	return error;
		case VK_INCOMPLETE:					PUSH_ERROR("");	break;
		default:							PUSH_DEFAULT;	break;
		}

		return error;
	}
	std::vector<Error> CreateCommandPool(VkDevice _device, const VkCommandPoolCreateInfo* _pCreateInfo, VkCommandPool* _pCommandPool)
	{
		std::vector<Error> error;

		VkResult vkResult = vkCreateCommandPool(_device, _pCreateInfo, nullptr, _pCommandPool);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	PUSH_ERROR("");	return error;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	PUSH_ERROR("");	return error;
		default:							PUSH_DEFAULT;	break;
		}

		return error;
	}
	std::vector<Error> CreateSwapchain(VkDevice _device, const VkSwapchainCreateInfoKHR* _pCreateInfo, VkSwapchainKHR* _pSwapchain)
	{
		std::vector<Error> error;

		VkResult vkResult = vkCreateSwapchainKHR(_device, _pCreateInfo, nullptr, _pSwapchain);
		switch (vkResult)
		{
		case VK_SUCCESS:						break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:		PUSH_ERROR("");	return error;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:		PUSH_ERROR("");	return error;
		case VK_ERROR_DEVICE_LOST:				PUSH_ERROR("");	return error;
		case VK_ERROR_SURFACE_LOST_KHR:			PUSH_ERROR("");	return error;
		case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:	PUSH_ERROR("");	return error;
		default:								PUSH_DEFAULT;	break;
		}

		return error;
	}
	std::vector<Error> CreateImage(VkDevice _device, const VkImageCreateInfo* _pCreateInfo, VkImage* _pImage)
	{
		std::vector<Error> error;

		VkResult vkResult = vkCreateImage(_device, _pCreateInfo, nullptr, _pImage);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	PUSH_ERROR("");	return error;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	PUSH_ERROR("");	return error;
		default:							PUSH_DEFAULT; break;
		}

		return error;
	}
	std::vector<Error> CreateImageView(VkDevice _device, const VkImageViewCreateInfo* _pCreateInfo, VkImageView* _pView)
	{
		std::vector<Error> error;

		VkResult vkResult = vkCreateImageView(_device, _pCreateInfo, nullptr, _pView);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	PUSH_ERROR("");	return error;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	PUSH_ERROR("");	return error;
		default:							PUSH_DEFAULT;	break;
		}

		return error;
	}
	std::vector<Error> AllocateMemory(VkDevice _device, const VkMemoryAllocateInfo* _pAllocateInfo, VkDeviceMemory* _pMemory)
	{
		std::vector<Error> error;

		VkResult vkResult = vkAllocateMemory(_device, _pAllocateInfo, nullptr, _pMemory);
		switch (vkResult)
		{
		case VK_SUCCESS:							break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:			PUSH_ERROR("");	return error;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:			PUSH_ERROR("");	return error;
		case VK_ERROR_TOO_MANY_OBJECTS:				PUSH_ERROR("");	return error;
		case VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR:	PUSH_ERROR("");	return error;
		default:									PUSH_DEFAULT;	break;
		}

		return error;
	}
	std::vector<Error> BindImageMemory(VkDevice _device, VkImage _image, VkDeviceMemory _memory, VkDeviceSize _memoryOffset)
	{
		std::vector<Error> error;

		VkResult vkResult = vkBindImageMemory(_device, _image, _memory, _memoryOffset);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	PUSH_ERROR("");	return error;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	PUSH_ERROR("");	return error;
		default:							PUSH_DEFAULT;	break;
		}

		return error;
	}
	std::vector<Error> CreateFence(VkDevice _device, const VkFenceCreateInfo* _pCreateInfo, VkFence* _pFence)
	{
		std::vector<Error> error;

		VkResult vkResult = vkCreateFence(_device, _pCreateInfo, nullptr, _pFence);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	PUSH_ERROR("");	return error;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	PUSH_ERROR("");	return error;
		default:							PUSH_DEFAULT;	break;
		}

		return error;
	}
	std::vector<Error> AllocateCommandBuffers(VkDevice _device, const VkCommandBufferAllocateInfo* _pAllocateInfo, VkCommandBuffer* _pCommandBuffers)
	{
		std::vector<Error> error;

		VkResult vkResult = vkAllocateCommandBuffers(_device, _pAllocateInfo, _pCommandBuffers);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	PUSH_ERROR("");	return error;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	PUSH_ERROR("");	return error;
		default:							PUSH_DEFAULT;	break;
		}

		return error;
	}
	std::vector<Error> WaitForFences(VkDevice _device, uint32_t _fenceCount, const VkFence* _pFences, VkBool32 _waitAll, uint64_t _timeout)
	{
		std::vector<Error> error;

		VkResult vkResult = vkWaitForFences(_device, _fenceCount, _pFences, _waitAll, _timeout);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	PUSH_ERROR("");	return error;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	PUSH_ERROR("");	return error;
		case VK_ERROR_DEVICE_LOST:			PUSH_ERROR("");	return error;
		case VK_TIMEOUT:					PUSH_ERROR("");	break;
		default:							PUSH_DEFAULT;	break;
		}

		return error;
	}
	std::vector<Error> ResetFences(VkDevice _device, uint32_t _fenceCount, const VkFence* _pFences)
	{
		std::vector<Error> error;

		VkResult vkResult = vkResetFences(_device, _fenceCount, _pFences);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	PUSH_ERROR("");	return error;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	PUSH_ERROR("");	return error;
		default:							PUSH_DEFAULT;	break;
		}

		return error;
	}
	std::vector<Error> BeginCommandBuffer(VkCommandBuffer _commandBuffer, const VkCommandBufferBeginInfo* _pBeginInfo)
	{
		std::vector<Error> error;

		VkResult vkResult = vkBeginCommandBuffer(_commandBuffer, _pBeginInfo);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	PUSH_ERROR("");	return error;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	PUSH_ERROR("");	return error;
		default:							PUSH_DEFAULT;	break;
		}

		return error;
	}
	std::vector<Error> EndCommandBuffer(VkCommandBuffer _commandBuffer)
	{
		std::vector<Error> error;

		VkResult vkResult = vkEndCommandBuffer(_commandBuffer);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	PUSH_ERROR("");	return error;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	PUSH_ERROR("");	return error;
		default:							PUSH_DEFAULT;	break;
		}

		return error;
	}
	std::vector<Error> QueueSubmit(VkQueue _queue, uint32_t _submitCount, const VkSubmitInfo* _pSubmits, VkFence _fence)
	{
		std::vector<Error> error;

		VkResult vkResult = vkQueueSubmit(_queue, _submitCount, _pSubmits, _fence);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	PUSH_ERROR("");	return error;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	PUSH_ERROR("");	return error;
		case VK_ERROR_DEVICE_LOST:			PUSH_ERROR("");	return error;
		default:							PUSH_DEFAULT;	break;
		}

		return error;
	}
	std::vector<Error> CreateSemaphore_(VkDevice _device, const VkSemaphoreCreateInfo* _pCreateInfo, VkSemaphore* _pSemaphore)
	{
		std::vector<Error> error;

		VkResult vkResult = vkCreateSemaphore(_device, _pCreateInfo, nullptr, _pSemaphore);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	PUSH_ERROR("");	return error;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	PUSH_ERROR("");	return error;
		default:							PUSH_DEFAULT;	break;
		}

		return error;
	}
	std::vector<Error> CreateFramebuffer(VkDevice _device, const VkFramebufferCreateInfo* _pCreateInfo, VkFramebuffer* _pFramebuffer)
	{
		std::vector<Error> error;

		VkResult vkResult = vkCreateFramebuffer(_device, _pCreateInfo, nullptr, _pFramebuffer);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	PUSH_ERROR("");	return error;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	PUSH_ERROR("");	return error;
		default:							PUSH_DEFAULT;	break;
		}

		return error;
	}
	std::vector<Error> CreateDescriptorSetLayout(VkDevice _device, const VkDescriptorSetLayoutCreateInfo* _pCreateInfo, VkDescriptorSetLayout* _pSetLayout)
	{
		std::vector<Error> error;

		VkResult vkResult = vkCreateDescriptorSetLayout(_device, _pCreateInfo, nullptr, _pSetLayout);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	PUSH_ERROR("");	return error;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	PUSH_ERROR("");	return error;
		default:							PUSH_DEFAULT;	break;
		}

		return error;
	}
	std::vector<Error> CreatePipelineLayout(VkDevice _device, const VkPipelineLayoutCreateInfo* _pCreateInfo, VkPipelineLayout* _pPipelineLayout)
	{
		std::vector<Error> error;

		VkResult vkResult = vkCreatePipelineLayout(_device, _pCreateInfo, nullptr, _pPipelineLayout);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	PUSH_ERROR("");	return error;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	PUSH_ERROR("");	return error;
		default:							PUSH_DEFAULT;	break;
		}

		return error;
	}
	std::vector<Error> CreateShaderModule(VkDevice _device, const VkShaderModuleCreateInfo* _pCreateInfo, VkShaderModule* _pShaderModule)
	{
		std::vector<Error> error;

		VkResult vkResult = vkCreateShaderModule(_device, _pCreateInfo, nullptr, _pShaderModule);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	PUSH_ERROR("");	return error;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	PUSH_ERROR("");	return error;
		default:							PUSH_DEFAULT;	break;
		}

		return error;
	}
	std::vector<Error> CreateGraphicsPipelines(VkDevice _device, uint32_t _createInfoCount, const VkGraphicsPipelineCreateInfo* _pCreateInfos, VkPipeline* _pPipelines)
	{
		std::vector<Error> error;

		VkResult vkResult = vkCreateGraphicsPipelines(_device, VK_NULL_HANDLE, _createInfoCount, _pCreateInfos, nullptr, _pPipelines);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	PUSH_ERROR("");	return error;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	PUSH_ERROR("");	return error;
		default:							PUSH_DEFAULT;	break;
		}

		return error;
	}
	std::vector<Error> AcquireNextImageKHR(VkDevice _device, VkSwapchainKHR _swapchain, uint64_t _timeout, VkSemaphore _semaphore, VkFence _fence, uint32_t* _pImageIndex)
	{
		std::vector<Error> error;

		VkResult vkResult = vkAcquireNextImageKHR(_device, _swapchain, _timeout, _semaphore, _fence, _pImageIndex);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	PUSH_ERROR("");	return error;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	PUSH_ERROR("");	return error;
		case VK_ERROR_DEVICE_LOST:			PUSH_ERROR("");	return error;
		case VK_ERROR_OUT_OF_DATE_KHR:		PUSH_ERROR("");	return error;
		case VK_ERROR_SURFACE_LOST_KHR:		PUSH_ERROR("");	return error;
		case VK_TIMEOUT:					PUSH_ERROR("");	break;
		case VK_NOT_READY:					PUSH_ERROR("");	break;
		case VK_SUBOPTIMAL_KHR:				PUSH_ERROR("");	break;
		default:							PUSH_DEFAULT;	break;
		}

		return error;
	}
	std::vector<Error> QueuePresentKHR(VkQueue _queue, const VkPresentInfoKHR* _pPresentInfo)
	{
		std::vector<Error> error;

		VkResult vkResult = vkQueuePresentKHR(_queue, _pPresentInfo);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	PUSH_ERROR("");	return error;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	PUSH_ERROR("");	return error;
		case VK_ERROR_DEVICE_LOST:			PUSH_ERROR("");	return error;
		case VK_ERROR_OUT_OF_DATE_KHR:		PUSH_ERROR("");	return error;
		case VK_ERROR_SURFACE_LOST_KHR:		PUSH_ERROR("");	return error;
		case VK_SUBOPTIMAL_KHR:				PUSH_ERROR("");	break;
		default:							PUSH_DEFAULT;	break;
		}

		return error;
	}
	std::vector<Error> CreateBuffer(VkDevice _device, const VkBufferCreateInfo* _pCreateInfo, VkBuffer* _pBuffer)
	{
		std::vector<Error> error;

		VkResult vkResult = vkCreateBuffer(_device, _pCreateInfo, nullptr, _pBuffer);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	PUSH_ERROR("");	return error;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	PUSH_ERROR("");	return error;
		default:							PUSH_DEFAULT;	break;
		}

		return error;
	}
	std::vector<Error> BindBufferMemory(VkDevice _device, VkBuffer _buffer, VkDeviceMemory _memory, VkDeviceSize _memoryOffset)
	{
		std::vector<Error> error;

		VkResult vkResult = vkBindBufferMemory(_device, _buffer, _memory, _memoryOffset);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	PUSH_ERROR("");	return error;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	PUSH_ERROR("");	return error;
		default:							PUSH_DEFAULT;	break;
		}

		return error;
	}
	std::vector<Error> MapMemory(VkDevice _device, VkDeviceMemory _memory, VkDeviceSize _offset, VkDeviceSize _size, void** _ppData)
	{
		std::vector<Error> error;

		VkResult vkResult = vkMapMemory(_device, _memory, _offset, _size, VK_FLAGS_NONE, _ppData);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	PUSH_ERROR("");	return error;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	PUSH_ERROR("");	return error;
		case VK_ERROR_MEMORY_MAP_FAILED:	PUSH_ERROR(""); return error;
		default:							PUSH_DEFAULT;	break;
		}

		return error;
	}
}

#endif