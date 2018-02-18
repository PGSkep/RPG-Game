#ifndef	RENDERER_H
#define RENDERER_H

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan\vulkan.h>
#define VK_RESERVED_FOR_FUTURE_USE 0
#define VK_FLAGS_NONE 0

#define PUSH_ERROR(contextSTR) \
error.push_back({__LINE__, __FILE__, contextSTR});
#define PUSH_DEFAULT \
error.push_back({__LINE__, __FILE__, GetStrFromVkResult(vkResult)});

#include <math.h>
#include <vector>
#include <fstream>

#include "Console.h"

struct Window
{
	HINSTANCE hInstance = NULL;
	std::string name;
	HWND hWnd = NULL;
};
struct PhysicalDevice
{
	VkPhysicalDevice handle = VK_NULL_HANDLE;

	VkPhysicalDeviceFeatures					features;
	VkPhysicalDeviceMemoryProperties			memoryProperties;
	VkPhysicalDeviceProperties					properties;
	std::vector<VkQueueFamilyProperties>		queueFamilyProperties;
	struct Presentability
	{
		VkBool32 win32Presentability = VK_FALSE;
		std::vector<VkBool32> perSurfacePresentability;
	};
	std::vector<Presentability>					queueFamilyPresentability;

	std::vector<VkLayerProperties>				layers;
	std::vector<VkExtensionProperties>			extensions;
	VkFormat									depthFormat;

	struct SurfaceProperties
	{
		VkSurfaceCapabilitiesKHR surfaceCapabilities;
		std::vector<VkSurfaceFormatKHR> surfaceFormats;
		std::vector<VkPresentModeKHR> presentModes;
	};
	std::vector<SurfaceProperties> surfaceProperties;
};
struct Device
{
	struct QueueGroup
	{
		uint32_t						familyIndex = -1;
		VkQueueFlags					flags = VK_NULL_HANDLE;
		PhysicalDevice::Presentability	presentability;

		std::vector<float>				priority;
		std::vector<VkQueue>			queues;

		VkCommandPool					commandPool;

		struct Requirements
		{
			VkQueueFlags	flags;
			bool			presentability;
			float			priority;
			uint32_t		count;

			static inline Requirements GetRequirements(VkQueueFlags	_flags, bool _presentability, float _priority, uint32_t _count)
			{
				return { _flags, _presentability, _priority, _count };
			}
		};
	};

	VkDevice				handle = VK_NULL_HANDLE;
	PhysicalDevice*			physicalDevice = nullptr;
	std::vector<QueueGroup>	queueGroups;

	struct QueueFamilyRequirements
	{
		VkQueueFlags		flags;
		bool				presentability;
		std::vector<float>	priorities;

		static QueueFamilyRequirements Get(VkQueueFlags _flags, bool _presentability, std::vector<float> _priorities)
		{
			QueueFamilyRequirements result;
			result.flags = _flags;
			result.presentability = _presentability;
			result.priorities = _priorities;
			return result;
		}
	};
};
struct Image
{
	VkImage handle;
	VkDeviceMemory memory;
	VkImageView view;
};
struct Swapchain
{
	VkSwapchainKHR handle;
	VkRenderPass* renderPass;
	VkExtent2D extent;
	std::vector<VkImage> images;
	std::vector<VkImageView> views;
	std::vector<VkFramebuffer> framebuffers;

	Image depthImage;

	std::vector<VkFence> fences;
	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderDoneSemaphore;

	std::vector<VkCommandBuffer> commandBuffers;
};
struct Buffer
{
	VkBuffer handle;
	VkDeviceMemory memory;
};

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
	
	struct Error
	{
		int32_t line;
		std::string file;
		std::string context;
	};
	void ErrorAdd(std::vector<Error>& _errorsA, std::vector<Error> _errorsB)
	{
		size_t errorACount = _errorsA.size();
		_errorsA.resize(_errorsA.size() + _errorsB.size());

		for (size_t i = 0; i != _errorsB.size(); ++i)
			_errorsA[errorACount + i] = _errorsB[i];
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

class Renderer
{
	struct Gpu
	{
		VkInstance instance = VK_NULL_HANDLE;
		VkDebugReportCallbackEXT debugReportCallback = VK_NULL_HANDLE;
		std::vector<Window> windows;
		std::vector<VkSurfaceKHR> surfaces;
		std::vector<PhysicalDevice> physicalDevices;
	};
	Gpu gpu;

	struct Controller
	{
		Gpu* gpu;

		Device device;
		std::vector<uint32_t> pickedIndices;
		std::vector<Device::QueueFamilyRequirements> queueFamilyRequirements;
		
		uint32_t renderQueueID;
		uint32_t renderGroupID;

		struct TransferManager
		{
			uint32_t groupID;
			uint32_t queueID;

			VkQueue queue;
			VkCommandBuffer commandBuffer;
			VkFence fence;

			Buffer buffer;
		};
		TransferManager transferManager;
		Buffer vertexBuffer;
		Buffer indexBuffer;

		std::vector<VkRenderPass> renderPasses;
		std::vector<Swapchain> swapchains;

		VkFence imageAvailable;
		VkFence renderDone;
		VkCommandBuffer renderCommandBuffer;
		VkQueue renderQueue;

		VkDescriptorSetLayout descriptorSetLayout;
		VkPipelineLayout pipelineLayout;
		VkShaderModule shaderVert;
		VkShaderModule shaderFrag;

		VkPipeline pipeline;

		void Init()
		{
			SetupDevice();
			SetupTransferManager();
			SetupMemory();
			SetupSwapchain();
			SetupRender();
			SetupLayouts();
			SetupPipeline();
			SetupDescriptor();

			PrepareForFirstRun();
		}
		void SetupDevice()
		{
			std::vector<VkU::Error> error;

			/// DEVICE
			uint32_t physicalDevicePick = 0;
			uint32_t validIndexPick = 0;
			device.physicalDevice = &gpu->physicalDevices[physicalDevicePick];
			std::vector<std::vector<uint32_t>> validIndices;
			bool found;
			std::vector<const char*> deviceExtensions = { "VK_KHR_swapchain" };
			VkPhysicalDeviceFeatures features = VkU::PhysicalDevivceGetFeaturesFromStrArr({ "fillModeNonSolid", "multiViewport", "tessellationShader", "geometryShader" });
			queueFamilyRequirements = {
				Device::QueueFamilyRequirements::Get(VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT,	true,{ 1.0f }),
			};

			// validate extensions
			for (size_t i = 0; i != deviceExtensions.size(); ++i)
			{
				found = false;
				std::vector<VkExtensionProperties>* availableExtensions = &gpu->physicalDevices[physicalDevicePick].extensions;
				for (size_t j = 0; j != availableExtensions->size(); ++j)
				{
					if (strcmp(deviceExtensions[i], (*availableExtensions)[j].extensionName) == 0)
						found = true;
				}
				if (found == false)
				{
					PUSH_ERROR(deviceExtensions[i]);
				}
			}

			// validate features
			for (size_t i = 0; i != sizeof(VkPhysicalDeviceFeatures) / sizeof(VkBool32); ++i)
			{
				if (((VkBool32*)&features)[i] == VK_TRUE && ((VkBool32*)&gpu->physicalDevices[physicalDevicePick].features)[i] == VK_FALSE)
				{
					PUSH_ERROR("feature " + std::to_string(i));
				}
			}

			// Get valid indices
			size_t queueCount = gpu->physicalDevices[physicalDevicePick].queueFamilyProperties.size();

			// get number of iterations
			size_t numberOfIterations = (size_t)pow(queueCount, queueFamilyRequirements.size());

			// iterate
			std::vector<uint32_t> indices(queueFamilyRequirements.size());
			for (size_t iterationNum = 0; iterationNum != numberOfIterations; ++iterationNum)
			{
				// overflow to next when needed
				if (queueFamilyRequirements.size() > 1)
				{
					for (size_t iReq = 0; iReq != queueFamilyRequirements.size(); ++iReq)
					{
						if (indices[iReq] == indices.size())
						{
							indices[iReq] = 0;

							++indices[iReq + 1];
						}
					}
				}

				// check if indice group is valid
				std::vector<uint32_t> familyQueueUseCount(queueCount);
				bool valid = true;
				for (size_t iReq = 0; iReq != queueFamilyRequirements.size(); ++iReq)
				{
					uint32_t familyIndex = indices[iReq];

					// count
					if (gpu->physicalDevices[physicalDevicePick].queueFamilyProperties[familyIndex].queueCount < familyQueueUseCount[familyIndex] + queueFamilyRequirements[iReq].priorities.size())
					{
						valid = false;
						break;
					}

					// flags
					if ((queueFamilyRequirements[iReq].flags & gpu->physicalDevices[physicalDevicePick].queueFamilyProperties[familyIndex].queueFlags) != queueFamilyRequirements[iReq].flags)
					{
						valid = false;
						break;
					}

					// presentation
					if (queueFamilyRequirements[iReq].presentability == true)
					{
						if (gpu->physicalDevices[physicalDevicePick].queueFamilyPresentability[familyIndex].win32Presentability == VK_FALSE)
						{
							valid = false;
							break;
						}

						VkBool32 surfaceSupport = VK_TRUE;
						gpu->physicalDevices[physicalDevicePick].queueFamilyPresentability[familyIndex].perSurfacePresentability.resize(gpu->surfaces.size());
						for (size_t iSurface = 0; iSurface != gpu->surfaces.size(); ++iSurface)
						{
							if (gpu->physicalDevices[physicalDevicePick].queueFamilyPresentability[familyIndex].perSurfacePresentability[iSurface] == VK_FALSE)
							{
								valid = false;
								break;
							}
						}
						if (!valid)
						{
							break;
						}
					}
				}

				if (valid)
				{
					for (size_t i = 0; i != indices.size(); ++i)
						std::cout << indices[0] << " | ";
					std::cout << "valid\n";
					validIndices.push_back(indices);
				}
				else
				{
					for (size_t i = 0; i != indices.size(); ++i)
						std::cout << indices[0] << " | ";
					std::cout << "invalid\n";
				}

				++indices[0];
			}

			if (validIndices.size() == 0)
			{
				PUSH_ERROR("");
			}

			// Get deviceQueueCreateInfo
			std::vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfo;
			std::vector<std::vector<float>> priorities;

			for (size_t iValid = 0; iValid != validIndices[validIndexPick].size(); ++iValid)
			{
				bool found = false;
				for (size_t iInfo = 0; iInfo != deviceQueueCreateInfo.size(); ++iInfo)
				{
					if (validIndices[validIndexPick][iValid] == deviceQueueCreateInfo[iInfo].queueFamilyIndex)
					{
						found = true;
						deviceQueueCreateInfo[iInfo].queueCount += (uint32_t)queueFamilyRequirements[iValid].priorities.size();

						for (size_t iPriorities = 0; iPriorities != queueFamilyRequirements[iValid].priorities.size(); ++iPriorities)
						{
							priorities[iInfo].push_back(queueFamilyRequirements[iValid].priorities[iPriorities]);
						}

						deviceQueueCreateInfo[iInfo].pQueuePriorities = priorities[iInfo].data();
					}
				}

				if (found == false)
				{

					deviceQueueCreateInfo.push_back({});
					deviceQueueCreateInfo[deviceQueueCreateInfo.size() - 1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
					deviceQueueCreateInfo[deviceQueueCreateInfo.size() - 1].pNext = nullptr;
					deviceQueueCreateInfo[deviceQueueCreateInfo.size() - 1].flags = VK_RESERVED_FOR_FUTURE_USE;
					deviceQueueCreateInfo[deviceQueueCreateInfo.size() - 1].queueFamilyIndex = validIndices[validIndexPick][iValid];
					deviceQueueCreateInfo[deviceQueueCreateInfo.size() - 1].queueCount = (uint32_t)queueFamilyRequirements[iValid].priorities.size();

					priorities.push_back({});
					for (size_t iPriorities = 0; iPriorities != queueFamilyRequirements[iValid].priorities.size(); ++iPriorities)
					{
						priorities[priorities.size() - 1].push_back(queueFamilyRequirements[iValid].priorities[iPriorities]);
					}

					deviceQueueCreateInfo[deviceQueueCreateInfo.size() - 1].pQueuePriorities = priorities[priorities.size() - 1].data();
				}
			}

			for (size_t iInfo = 0; iInfo != deviceQueueCreateInfo.size(); ++iInfo)
			{
				deviceQueueCreateInfo[iInfo].queueCount = (uint32_t)priorities[iInfo].size();
				deviceQueueCreateInfo[iInfo].pQueuePriorities = priorities[iInfo].data();
			}

			VkDeviceCreateInfo deviceCreateInfo;
			deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			deviceCreateInfo.pNext = nullptr;
			deviceCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;
			deviceCreateInfo.queueCreateInfoCount = (uint32_t)deviceQueueCreateInfo.size();
			deviceCreateInfo.pQueueCreateInfos = deviceQueueCreateInfo.data();
			deviceCreateInfo.enabledLayerCount = 0; // deprecated
			deviceCreateInfo.ppEnabledLayerNames = nullptr; // deprecated
			deviceCreateInfo.enabledExtensionCount = (uint32_t)deviceExtensions.size();
			deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
			deviceCreateInfo.pEnabledFeatures = &features;
			VkU::ErrorAdd(error, VkU::CreateDevice(gpu->physicalDevices[physicalDevicePick].handle, &deviceCreateInfo, device.handle));
			device.physicalDevice = &gpu->physicalDevices[physicalDevicePick];

			transferManager.groupID = 0;
			transferManager.queueID = 0;

			renderGroupID = 0;
			renderQueueID = 0;

			/// QUEUE
			device.queueGroups.resize(deviceQueueCreateInfo.size());
			for (size_t iGroup = 0; iGroup != device.queueGroups.size(); ++iGroup)
			{
				device.queueGroups[iGroup].familyIndex = deviceQueueCreateInfo[iGroup].queueFamilyIndex;
				device.queueGroups[iGroup].flags = device.physicalDevice->queueFamilyProperties[device.queueGroups[iGroup].familyIndex].queueFlags;
				device.queueGroups[iGroup].presentability = device.physicalDevice->queueFamilyPresentability[device.queueGroups[iGroup].familyIndex];
				device.queueGroups[iGroup].priority = priorities[device.queueGroups[iGroup].familyIndex];
				device.queueGroups[iGroup].queues.resize(device.queueGroups[iGroup].priority.size());

				for (size_t iQueue = 0; iQueue != device.queueGroups[iGroup].queues.size(); ++iQueue)
					vkGetDeviceQueue(device.handle, device.queueGroups[iGroup].familyIndex, (uint32_t)iQueue, &device.queueGroups[iGroup].queues[iQueue]);
			}

			/// COMMAND POOL
			VkCommandPoolCreateInfo commandPoolCreateInfo;
			commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			commandPoolCreateInfo.pNext = nullptr;
			commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			for (size_t i = 0; i != device.queueGroups.size(); ++i)
			{
				commandPoolCreateInfo.queueFamilyIndex = device.queueGroups[i].familyIndex;
				VkU::ErrorAdd(error, VkU::CreateCommandPool(device.handle, &commandPoolCreateInfo, &device.queueGroups[i].commandPool));
			}
		}
		void SetupTransferManager()
		{
			std::vector<VkU::Error> error;

			// Fence
			VkFenceCreateInfo fenceCreateInfo;
			fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fenceCreateInfo.pNext = nullptr;
			fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
			VkU::ErrorAdd(error, VkU::CreateFence(device.handle, &fenceCreateInfo, &transferManager.fence));

			// Command buffer
			VkCommandBufferAllocateInfo commandBufferAllocateInfo;
			commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			commandBufferAllocateInfo.pNext = nullptr;
			commandBufferAllocateInfo.commandPool = device.queueGroups[transferManager.groupID].commandPool;
			commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			commandBufferAllocateInfo.commandBufferCount = 1;
			VkU::ErrorAdd(error, VkU::AllocateCommandBuffers(device.handle, &commandBufferAllocateInfo, &transferManager.commandBuffer));

			// queue
			transferManager.queue = device.queueGroups[transferManager.groupID].queues[transferManager.queueID];

			// buffer
			VkBufferCreateInfo bufferCreateInfo;
			bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferCreateInfo.pNext = nullptr;
			bufferCreateInfo.flags = VK_FLAGS_NONE;
			bufferCreateInfo.size = sizeof(float) * 1024;
			bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
			bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			bufferCreateInfo.queueFamilyIndexCount = 0;
			bufferCreateInfo.pQueueFamilyIndices = nullptr;
			VkU::ErrorAdd(error, VkU::CreateBuffer(device.handle, &bufferCreateInfo, &transferManager.buffer.handle));

			VkMemoryRequirements memoryRequirements;
			vkGetBufferMemoryRequirements(device.handle, transferManager.buffer.handle, &memoryRequirements);

			VkMemoryAllocateInfo memoryAllocateInfo;
			memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			memoryAllocateInfo.pNext = nullptr;
			memoryAllocateInfo.allocationSize = memoryRequirements.size;
			memoryAllocateInfo.memoryTypeIndex = VkU::FindMemoryType(device.physicalDevice->memoryProperties, memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			VkU::ErrorAdd(error, VkU::AllocateMemory(device.handle, &memoryAllocateInfo, &transferManager.buffer.memory));

			VkU::ErrorAdd(error, VkU::BindBufferMemory(device.handle, transferManager.buffer.handle, transferManager.buffer.memory, 0));
		}
		void SetupMemory()
		{
			std::vector<VkU::Error> error;

			// data
			float vertData[] = {
				//x    y            r     g     b
				-0.5f, -0.86603f,	1.0f, 0.0f, 0.0f,
				-0.5f, +0.86603f,	0.0f, 1.0f, 0.0f,
				-1.0f, +0.00000f,	0.0f, 0.0f, 1.0f,
			};

			uint16_t indxData[] = {
				0, 1, 2,
			};

			size_t stagingMemorySize = sizeof(vertData) + sizeof(indxData);

			// vertex
			VkBufferCreateInfo bufferCreateInfo;
			bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferCreateInfo.pNext = nullptr;
			bufferCreateInfo.flags = VK_FLAGS_NONE;
			bufferCreateInfo.size = sizeof(vertData);
			bufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
			bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			bufferCreateInfo.queueFamilyIndexCount = 0;
			bufferCreateInfo.pQueueFamilyIndices = nullptr;
			VkU::ErrorAdd(error, VkU::CreateBuffer(device.handle, &bufferCreateInfo, &vertexBuffer.handle));

			VkMemoryRequirements memoryRequirements;
			vkGetBufferMemoryRequirements(device.handle, vertexBuffer.handle, &memoryRequirements);

			VkMemoryAllocateInfo memoryAllocateInfo;
			memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			memoryAllocateInfo.pNext = nullptr;
			memoryAllocateInfo.allocationSize = memoryRequirements.size;
			memoryAllocateInfo.memoryTypeIndex = VkU::FindMemoryType(device.physicalDevice->memoryProperties, memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
			VkU::ErrorAdd(error, VkU::AllocateMemory(device.handle, &memoryAllocateInfo, &vertexBuffer.memory));

			VkU::ErrorAdd(error, VkU::BindBufferMemory(device.handle, vertexBuffer.handle, vertexBuffer.memory, 0));

			// index

			// memcpy
			void* data = nullptr;
			size_t dataPos = 0;
			VkU::ErrorAdd(error, VkU::MapMemory(device.handle, transferManager.buffer.memory, 0, stagingMemorySize, &data));

			memcpy((void*)((size_t)data + dataPos), vertData, sizeof(vertData));
			dataPos += sizeof(vertData);

			memcpy((void*)((size_t)data + dataPos), indxData, sizeof(indxData));
			dataPos += sizeof(indxData);

			vkUnmapMemory(device.handle, transferManager.buffer.memory);

			// transfer
			VkU::ErrorAdd(error, VkU::WaitForFences(device.handle, 1, &transferManager.fence, VK_TRUE, ~0U));
			VkU::ErrorAdd(error, VkU::ResetFences(device.handle, 1, &transferManager.fence));

			VkCommandBufferBeginInfo commandBufferBeginInfo;
			commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			commandBufferBeginInfo.pNext = nullptr;
			commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			commandBufferBeginInfo.pInheritanceInfo = nullptr;
			VkU::ErrorAdd(error, VkU::BeginCommandBuffer(transferManager.commandBuffer, &commandBufferBeginInfo));

			VkBufferCopy bufferCopy;
			bufferCopy.srcOffset = 0;
			bufferCopy.dstOffset = 0;
			bufferCopy.size = sizeof(vertData);
			vkCmdCopyBuffer(transferManager.commandBuffer, transferManager.buffer.handle, vertexBuffer.handle, 1, &bufferCopy);

			VkU::ErrorAdd(error, VkU::EndCommandBuffer(transferManager.commandBuffer));

			VkSubmitInfo submitInfo;
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.pNext = nullptr;
			submitInfo.waitSemaphoreCount = 0;
			submitInfo.pWaitSemaphores = nullptr;
			submitInfo.pWaitDstStageMask = nullptr;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &transferManager.commandBuffer;
			submitInfo.signalSemaphoreCount = 0;
			submitInfo.pSignalSemaphores = nullptr;
			VkU::QueueSubmit(transferManager.queue, 1, &submitInfo, transferManager.fence);
		}
		void SetupSwapchain()
		{
			std::vector<VkU::Error> error;

			VkU::ErrorAdd(error, VkU::WaitForFences(device.handle, 1, &transferManager.fence, VK_TRUE, ~0U));
			VkU::ErrorAdd(error, VkU::ResetFences(device.handle, 1, &transferManager.fence));

			VkCommandBufferBeginInfo commandBufferBeginInfo;
			commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			commandBufferBeginInfo.pNext = nullptr;
			commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			commandBufferBeginInfo.pInheritanceInfo = nullptr;
			VkU::ErrorAdd(error, VkU::BeginCommandBuffer(transferManager.commandBuffer, &commandBufferBeginInfo));

			/// SWAPCHAIN & RENDER PASS
			uint32_t targetImageCount = 3;
			std::vector<VkCompositeAlphaFlagBitsKHR> preferedCompositeAlphas = { VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR, VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR };
			std::vector<VkPresentModeKHR> preferedPresentModes = { VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_FIFO_KHR };

			swapchains.resize(device.physicalDevice->surfaceProperties.size());
			std::vector<VkSwapchainCreateInfoKHR> swapchainCreateInfos(swapchains.size());
			for (size_t iSwapchain = 0; iSwapchain != swapchains.size(); ++iSwapchain)
			{
				swapchainCreateInfos[iSwapchain].sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
				swapchainCreateInfos[iSwapchain].pNext = nullptr;
				swapchainCreateInfos[iSwapchain].flags = VK_RESERVED_FOR_FUTURE_USE;
				swapchainCreateInfos[iSwapchain].surface = gpu->surfaces[iSwapchain];

				// image count
				swapchainCreateInfos[iSwapchain].minImageCount = targetImageCount;
				if (swapchainCreateInfos[iSwapchain].minImageCount > device.physicalDevice->surfaceProperties[iSwapchain].surfaceCapabilities.maxImageCount)
					swapchainCreateInfos[iSwapchain].minImageCount = device.physicalDevice->surfaceProperties[iSwapchain].surfaceCapabilities.maxImageCount;
				else if (swapchainCreateInfos[iSwapchain].minImageCount < device.physicalDevice->surfaceProperties[iSwapchain].surfaceCapabilities.minImageCount)
					swapchainCreateInfos[iSwapchain].minImageCount = device.physicalDevice->surfaceProperties[iSwapchain].surfaceCapabilities.minImageCount;

				// surface formats
				VkSurfaceFormatKHR surfaceFormat;
				if (device.physicalDevice->surfaceProperties[iSwapchain].surfaceFormats.size() == 1 && device.physicalDevice->surfaceProperties[iSwapchain].surfaceFormats[0].format == VK_FORMAT_UNDEFINED)
					surfaceFormat = { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
				else
					surfaceFormat = device.physicalDevice->surfaceProperties[iSwapchain].surfaceFormats[0];

				swapchainCreateInfos[iSwapchain].imageFormat = surfaceFormat.format;
				swapchainCreateInfos[iSwapchain].imageColorSpace = surfaceFormat.colorSpace;

				// extent
				swapchainCreateInfos[iSwapchain].imageExtent.width = device.physicalDevice->surfaceProperties[iSwapchain].surfaceCapabilities.currentExtent.width;
				swapchainCreateInfos[iSwapchain].imageExtent.height = device.physicalDevice->surfaceProperties[iSwapchain].surfaceCapabilities.currentExtent.height;

				if (swapchainCreateInfos[iSwapchain].imageExtent.width > device.physicalDevice->surfaceProperties[iSwapchain].surfaceCapabilities.maxImageExtent.width)
					swapchainCreateInfos[iSwapchain].imageExtent.width = device.physicalDevice->surfaceProperties[iSwapchain].surfaceCapabilities.maxImageExtent.width;
				else if (swapchainCreateInfos[iSwapchain].imageExtent.width < device.physicalDevice->surfaceProperties[iSwapchain].surfaceCapabilities.minImageExtent.width)
					swapchainCreateInfos[iSwapchain].imageExtent.width = device.physicalDevice->surfaceProperties[iSwapchain].surfaceCapabilities.minImageExtent.width;

				if (swapchainCreateInfos[iSwapchain].imageExtent.height > device.physicalDevice->surfaceProperties[iSwapchain].surfaceCapabilities.maxImageExtent.height)
					swapchainCreateInfos[iSwapchain].imageExtent.height = device.physicalDevice->surfaceProperties[iSwapchain].surfaceCapabilities.maxImageExtent.height;
				else if (swapchainCreateInfos[iSwapchain].imageExtent.height < device.physicalDevice->surfaceProperties[iSwapchain].surfaceCapabilities.minImageExtent.height)
					swapchainCreateInfos[iSwapchain].imageExtent.height = device.physicalDevice->surfaceProperties[iSwapchain].surfaceCapabilities.minImageExtent.height;

				swapchainCreateInfos[iSwapchain].imageArrayLayers = 1;
				swapchainCreateInfos[iSwapchain].imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
				swapchainCreateInfos[iSwapchain].imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
				swapchainCreateInfos[iSwapchain].queueFamilyIndexCount = 0;
				swapchainCreateInfos[iSwapchain].pQueueFamilyIndices = nullptr;

				// preTransform
				swapchainCreateInfos[iSwapchain].preTransform = device.physicalDevice->surfaceProperties[iSwapchain].surfaceCapabilities.currentTransform;

				// composite alpha
				swapchainCreateInfos[iSwapchain].compositeAlpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
				for (size_t i = 0; i != preferedCompositeAlphas.size(); ++i)
				{
					if ((preferedCompositeAlphas[i] & device.physicalDevice->surfaceProperties[iSwapchain].surfaceCapabilities.supportedCompositeAlpha) == preferedCompositeAlphas[i])
					{
						swapchainCreateInfos[iSwapchain].compositeAlpha = preferedCompositeAlphas[i];
						break;
					}
				}

				// present mode
				swapchainCreateInfos[iSwapchain].presentMode = VK_PRESENT_MODE_FIFO_KHR;
				for (size_t iPrefered = 0; iPrefered != preferedPresentModes.size(); ++iPrefered)
				{
					for (size_t iMode = 0; iMode != device.physicalDevice->surfaceProperties[iSwapchain].presentModes.size(); ++iMode)
					{
						if (device.physicalDevice->surfaceProperties[iSwapchain].presentModes[iMode] == preferedPresentModes[iPrefered])
						{
							swapchainCreateInfos[iSwapchain].presentMode = preferedPresentModes[iPrefered];
							iPrefered = preferedPresentModes.size() - 1;
							break;
						}
					}
				}

				swapchainCreateInfos[iSwapchain].clipped = VK_TRUE;
				swapchainCreateInfos[iSwapchain].oldSwapchain = VK_NULL_HANDLE;

				VkU::ErrorAdd(error, VkU::CreateSwapchain(device.handle, &swapchainCreateInfos[iSwapchain], &swapchains[iSwapchain].handle));

				swapchains[iSwapchain].extent = swapchainCreateInfos[iSwapchain].imageExtent;

				/// RENDER PASS
				bool found = false;
				for (size_t i = 0; i != iSwapchain; ++i)
				{
					if (swapchainCreateInfos[iSwapchain].imageFormat == swapchainCreateInfos[i].imageFormat)
					{
						found = true;
						swapchains[iSwapchain].renderPass = &renderPasses[(size_t)swapchains[iSwapchain].renderPass];
						break;
					}
				}

				if (found == false)
				{
					renderPasses.push_back({});
					VkAttachmentDescription attachmentDescriptions[2];
					VkAttachmentDescription* colorAttachmentDescription = &attachmentDescriptions[0];
					colorAttachmentDescription->flags = VK_FLAGS_NONE;
					colorAttachmentDescription->format = swapchainCreateInfos[iSwapchain].imageFormat;
					colorAttachmentDescription->samples = VK_SAMPLE_COUNT_1_BIT;
					colorAttachmentDescription->loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
					colorAttachmentDescription->storeOp = VK_ATTACHMENT_STORE_OP_STORE;
					colorAttachmentDescription->stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
					colorAttachmentDescription->stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
					colorAttachmentDescription->initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
					colorAttachmentDescription->finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
					VkAttachmentDescription* depthAttachmentDescription = &attachmentDescriptions[1];
					depthAttachmentDescription->flags = VK_FLAGS_NONE;
					depthAttachmentDescription->format = device.physicalDevice->depthFormat;
					depthAttachmentDescription->samples = VK_SAMPLE_COUNT_1_BIT;
					depthAttachmentDescription->loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
					depthAttachmentDescription->storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
					depthAttachmentDescription->stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
					depthAttachmentDescription->stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
					depthAttachmentDescription->initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
					depthAttachmentDescription->finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
					VkAttachmentReference colorAttachmentReference;
					colorAttachmentReference.attachment = 0;
					colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
					VkAttachmentReference depthAttachmentReference;
					depthAttachmentReference.attachment = 1;
					depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
					VkSubpassDescription subpassDescription;
					subpassDescription.flags = VK_FLAGS_NONE;
					subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
					subpassDescription.inputAttachmentCount = 0;
					subpassDescription.pInputAttachments = nullptr;
					subpassDescription.colorAttachmentCount = 1;
					subpassDescription.pColorAttachments = &colorAttachmentReference;
					subpassDescription.pResolveAttachments = nullptr;
					subpassDescription.pDepthStencilAttachment = &depthAttachmentReference;
					subpassDescription.preserveAttachmentCount = 0;
					subpassDescription.pPreserveAttachments = nullptr;
					VkSubpassDependency subpassDependency;
					subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
					subpassDependency.dstSubpass = 0;
					subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
					subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
					subpassDependency.srcAccessMask = 0;
					subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
					subpassDependency.dependencyFlags = VK_FLAGS_NONE;
					VkRenderPassCreateInfo renderPassCreateInfo;
					renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
					renderPassCreateInfo.pNext = nullptr;
					renderPassCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;
					renderPassCreateInfo.attachmentCount = 2;
					renderPassCreateInfo.pAttachments = attachmentDescriptions;
					renderPassCreateInfo.subpassCount = 1;
					renderPassCreateInfo.pSubpasses = &subpassDescription;
					renderPassCreateInfo.dependencyCount = 1;
					renderPassCreateInfo.pDependencies = &subpassDependency;
					VkU::ErrorAdd(error, VkU::CreateRenderPass(device.handle, &renderPassCreateInfo, &renderPasses[renderPasses.size() - 1]));

					//swapchains[iSwapchain].renderPass = (VkRenderPass*)(renderPasses.size() - 1);
					swapchains[iSwapchain].renderPass = &renderPasses[renderPasses.size() - 1];
				}

				/// SWAPCHAIN OTHER
				// images
				VkU::ErrorAdd(error, VkU::GetSwapchainImagesKHR(device.handle, swapchains[iSwapchain].handle, swapchains[iSwapchain].images));

				// views
				VkImageViewCreateInfo imageViewCreateInfo;
				imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
				imageViewCreateInfo.pNext = nullptr;
				imageViewCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;
				imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
				imageViewCreateInfo.format = swapchainCreateInfos[iSwapchain].imageFormat;
				imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
				imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
				imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
				imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
				imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
				imageViewCreateInfo.subresourceRange.layerCount = 1;
				imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
				imageViewCreateInfo.subresourceRange.levelCount = 1;

				swapchains[iSwapchain].views.resize(swapchains[iSwapchain].images.size());
				for (size_t iView = 0; iView != swapchains[iSwapchain].views.size(); ++iView)
				{
					imageViewCreateInfo.image = swapchains[iSwapchain].images[iView];
					VkU::ErrorAdd(error, VkU::CreateImageView(device.handle, &imageViewCreateInfo, &swapchains[iSwapchain].views[iView]));
				}

				// Depth image
				VkImageCreateInfo imageCreateInfo;
				imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
				imageCreateInfo.pNext = nullptr;
				imageCreateInfo.flags = 0;
				imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
				imageCreateInfo.format = device.physicalDevice->depthFormat;
				imageCreateInfo.extent = { swapchainCreateInfos[iSwapchain].imageExtent.width, swapchainCreateInfos[iSwapchain].imageExtent.height, 1 };
				imageCreateInfo.mipLevels = 1;
				imageCreateInfo.arrayLayers = 1;
				imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
				imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
				imageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
				imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
				imageCreateInfo.queueFamilyIndexCount = 0;
				imageCreateInfo.pQueueFamilyIndices = nullptr;
				imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
				VkU::ErrorAdd(error, VkU::CreateImage(device.handle, &imageCreateInfo, &swapchains[iSwapchain].depthImage.handle));

				// Depth memory
				VkMemoryRequirements memoryRequirements;
				vkGetImageMemoryRequirements(device.handle, swapchains[iSwapchain].depthImage.handle, &memoryRequirements);

				VkMemoryAllocateInfo memoryAllocateInfo;
				memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
				memoryAllocateInfo.pNext = nullptr;
				memoryAllocateInfo.allocationSize = memoryRequirements.size;
				memoryAllocateInfo.memoryTypeIndex = VkU::FindMemoryType(device.physicalDevice->memoryProperties, memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
				VkU::ErrorAdd(error, VkU::AllocateMemory(device.handle, &memoryAllocateInfo, &swapchains[iSwapchain].depthImage.memory));

				VkU::ErrorAdd(error, VkU::BindImageMemory(device.handle, swapchains[iSwapchain].depthImage.handle, swapchains[iSwapchain].depthImage.memory, 0));

				// Depth View
				imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
				imageViewCreateInfo.pNext = nullptr;
				imageViewCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;
				imageViewCreateInfo.image = swapchains[iSwapchain].depthImage.handle;
				imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
				imageViewCreateInfo.format = device.physicalDevice->depthFormat;
				imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
				imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
				imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
				imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
				imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
				imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
				imageViewCreateInfo.subresourceRange.layerCount = 1;
				imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
				imageViewCreateInfo.subresourceRange.levelCount = 1;
				VkU::ErrorAdd(error, VkU::CreateImageView(device.handle, &imageViewCreateInfo, &swapchains[iSwapchain].depthImage.view));

				// transition depth layout
				VkImageMemoryBarrier imageMemoryBarrier;
				imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
				imageMemoryBarrier.pNext = nullptr;
				imageMemoryBarrier.srcAccessMask = 0;
				imageMemoryBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
				imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
				imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				imageMemoryBarrier.image = swapchains[iSwapchain].depthImage.handle;
				imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
				imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
				imageMemoryBarrier.subresourceRange.levelCount = 1;
				imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
				imageMemoryBarrier.subresourceRange.layerCount = 1;
				vkCmdPipelineBarrier(transferManager.commandBuffer, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

				// Framebuffers
				VkFramebufferCreateInfo framebufferCreateInfo;
				framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
				framebufferCreateInfo.pNext = nullptr;
				framebufferCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;
				framebufferCreateInfo.renderPass = *swapchains[iSwapchain].renderPass;
				framebufferCreateInfo.width = swapchainCreateInfos[iSwapchain].imageExtent.width;
				framebufferCreateInfo.height = swapchainCreateInfos[iSwapchain].imageExtent.height;
				framebufferCreateInfo.layers = 1;

				swapchains[iSwapchain].framebuffers.resize(swapchains[iSwapchain].views.size());
				for (size_t iFramebuffer = 0; iFramebuffer != swapchains[iSwapchain].framebuffers.size(); ++iFramebuffer)
				{
					std::vector<VkImageView> attachments;
					attachments = { swapchains[iSwapchain].views[iFramebuffer], swapchains[iSwapchain].depthImage.view };

					framebufferCreateInfo.attachmentCount = (uint32_t)attachments.size();
					framebufferCreateInfo.pAttachments = attachments.data();

					VkU::ErrorAdd(error, VkU::CreateFramebuffer(device.handle, &framebufferCreateInfo, &swapchains[iSwapchain].framebuffers[iFramebuffer]));
				}

				// Fences
				VkFenceCreateInfo fenceCreateInfo;
				fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
				fenceCreateInfo.pNext = nullptr;
				fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
				swapchains[iSwapchain].fences.resize(swapchains[iSwapchain].images.size());
				for (size_t iFence = 0; iFence != swapchains[iSwapchain].views.size(); ++iFence)
					VkU::ErrorAdd(error, VkU::CreateFence(device.handle, &fenceCreateInfo, &swapchains[iSwapchain].fences[iFence]));

				// Semaphores
				VkSemaphoreCreateInfo semaphoreCreateInfo;
				semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
				semaphoreCreateInfo.pNext = nullptr;
				semaphoreCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;
				VkU::ErrorAdd(error, VkU::CreateSemaphore_(device.handle, &semaphoreCreateInfo, &swapchains[iSwapchain].imageAvailableSemaphore));
				VkU::ErrorAdd(error, VkU::CreateSemaphore_(device.handle, &semaphoreCreateInfo, &swapchains[iSwapchain].renderDoneSemaphore));

				// Command Buffer
				swapchains[iSwapchain].commandBuffers.resize(swapchains[iSwapchain].images.size());
				VkCommandBufferAllocateInfo commandBufferAllocateInfo;
				commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
				commandBufferAllocateInfo.pNext = nullptr;
				commandBufferAllocateInfo.commandPool = device.queueGroups[renderGroupID].commandPool;
				commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
				commandBufferAllocateInfo.commandBufferCount = (uint32_t)swapchains[iSwapchain].commandBuffers.size();
				VkU::ErrorAdd(error, VkU::AllocateCommandBuffers(device.handle, &commandBufferAllocateInfo, swapchains[iSwapchain].commandBuffers.data()));
			}

			VkU::ErrorAdd(error, VkU::EndCommandBuffer(transferManager.commandBuffer));

			VkSubmitInfo submitInfo;
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.pNext = nullptr;
			submitInfo.waitSemaphoreCount = 0;
			submitInfo.pWaitSemaphores = nullptr;
			submitInfo.pWaitDstStageMask = nullptr;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &transferManager.commandBuffer;
			submitInfo.signalSemaphoreCount = 0;
			submitInfo.pSignalSemaphores = nullptr;
			VkU::QueueSubmit(transferManager.queue, 1, &submitInfo, transferManager.fence);
		}
		void SetupRender()
		{
			std::vector<VkU::Error> error;

			VkFenceCreateInfo fenceCreateInfo;
			fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fenceCreateInfo.pNext = nullptr;
			fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
			VkU::ErrorAdd(error, VkU::CreateFence(device.handle, &fenceCreateInfo, &imageAvailable));
			VkU::ErrorAdd(error, VkU::CreateFence(device.handle, &fenceCreateInfo, &renderDone));

			VkCommandBufferAllocateInfo commandBufferAllocateInfo;
			commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			commandBufferAllocateInfo.pNext = nullptr;
			commandBufferAllocateInfo.commandPool = device.queueGroups[0].commandPool;
			commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			commandBufferAllocateInfo.commandBufferCount = 1;
			VkU::ErrorAdd(error, VkU::AllocateCommandBuffers(device.handle, &commandBufferAllocateInfo, &renderCommandBuffer));

			renderQueue = device.queueGroups[renderGroupID].queues[renderQueueID];
		}
		void SetupLayouts()
		{
			std::vector<VkU::Error> error;

			/// DESCRIPTOR SET LAYOUT
			VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo;
			descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			descriptorSetLayoutCreateInfo.pNext = nullptr;
			descriptorSetLayoutCreateInfo.flags = VK_FLAGS_NONE;
			descriptorSetLayoutCreateInfo.bindingCount = 0;
			descriptorSetLayoutCreateInfo.pBindings = nullptr;
			VkU::ErrorAdd(error, VkU::CreateDescriptorSetLayout(device.handle, &descriptorSetLayoutCreateInfo, &descriptorSetLayout));

			/// PIPELINE LAYOUT
			VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo;
			pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			pipelineLayoutCreateInfo.pNext = nullptr;
			pipelineLayoutCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;
			pipelineLayoutCreateInfo.setLayoutCount = 0;
			pipelineLayoutCreateInfo.pSetLayouts = nullptr;
			pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
			pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;
			VkU::ErrorAdd(error, VkU::CreatePipelineLayout(device.handle, &pipelineLayoutCreateInfo, &pipelineLayout));

			/// SHADER MODULE
			std::vector<char> shaderCode;
			VkShaderModuleCreateInfo shaderModuleCreateInfo;
			shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			shaderModuleCreateInfo.pNext = nullptr;
			shaderModuleCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;

			VkU::GetShaderCode("../../Data/Shaders/vert.spv", shaderCode);
			shaderModuleCreateInfo.codeSize = shaderCode.size();
			shaderModuleCreateInfo.pCode = (uint32_t*)shaderCode.data();
			VkU::ErrorAdd(error, VkU::CreateShaderModule(device.handle, &shaderModuleCreateInfo, &shaderVert));

			VkU::GetShaderCode("../../Data/Shaders/frag.spv", shaderCode);
			shaderModuleCreateInfo.codeSize = shaderCode.size();
			shaderModuleCreateInfo.pCode = (uint32_t*)shaderCode.data();
			VkU::ErrorAdd(error, VkU::CreateShaderModule(device.handle, &shaderModuleCreateInfo, &shaderFrag));
		}
		void SetupPipeline()
		{
			std::vector<VkU::Error> error;

			std::vector<VkPipelineShaderStageCreateInfo> pipelineShaderStageCreateInfo;
			pipelineShaderStageCreateInfo.resize(2);
			pipelineShaderStageCreateInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			pipelineShaderStageCreateInfo[0].pNext = nullptr;
			pipelineShaderStageCreateInfo[0].flags = VK_FLAGS_NONE;
			pipelineShaderStageCreateInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
			pipelineShaderStageCreateInfo[0].module = shaderVert;
			pipelineShaderStageCreateInfo[0].pName = "main";
			pipelineShaderStageCreateInfo[0].pSpecializationInfo = nullptr;
			pipelineShaderStageCreateInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			pipelineShaderStageCreateInfo[1].pNext = nullptr;
			pipelineShaderStageCreateInfo[1].flags = VK_FLAGS_NONE;
			pipelineShaderStageCreateInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			pipelineShaderStageCreateInfo[1].module = shaderFrag;
			pipelineShaderStageCreateInfo[1].pName = "main";
			pipelineShaderStageCreateInfo[1].pSpecializationInfo = nullptr;

			VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo;
			pipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			pipelineVertexInputStateCreateInfo.pNext = nullptr;
			pipelineVertexInputStateCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;
			pipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = 0;
			pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = nullptr;
			pipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = 0;
			pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = nullptr;

			VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo;
			pipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			pipelineInputAssemblyStateCreateInfo.pNext = nullptr;
			pipelineInputAssemblyStateCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;
			pipelineInputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			pipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

			VkPipelineTessellationStateCreateInfo pipelineTessellationStateCreateInfo;
			pipelineTessellationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
			pipelineTessellationStateCreateInfo.pNext = nullptr;
			pipelineTessellationStateCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;
			pipelineTessellationStateCreateInfo.patchControlPoints = 0;

			VkViewport viewport = {};
			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = (float)swapchains[0].extent.width;
			viewport.height = (float)swapchains[0].extent.height;
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;

			VkRect2D scissor = {};
			scissor.offset = { 0, 0 };
			scissor.extent = swapchains[0].extent;

			VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo;
			pipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			pipelineViewportStateCreateInfo.pNext = nullptr;
			pipelineViewportStateCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;
			pipelineViewportStateCreateInfo.viewportCount = 1;
			pipelineViewportStateCreateInfo.pViewports = &viewport;
			pipelineViewportStateCreateInfo.scissorCount = 1;
			pipelineViewportStateCreateInfo.pScissors = &scissor;

			VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo;
			pipelineRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			pipelineRasterizationStateCreateInfo.pNext = nullptr;
			pipelineRasterizationStateCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;
			pipelineRasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
			pipelineRasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
			pipelineRasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
			pipelineRasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
			pipelineRasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
			pipelineRasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
			pipelineRasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
			pipelineRasterizationStateCreateInfo.depthBiasClamp = 0.0f;
			pipelineRasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;
			pipelineRasterizationStateCreateInfo.lineWidth = 1.0f;

			VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo;
			pipelineMultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			pipelineMultisampleStateCreateInfo.pNext = nullptr;
			pipelineMultisampleStateCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;
			pipelineMultisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
			pipelineMultisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
			pipelineMultisampleStateCreateInfo.minSampleShading = 0.0f;
			pipelineMultisampleStateCreateInfo.pSampleMask = nullptr;
			pipelineMultisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
			pipelineMultisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;

			VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo;
			pipelineDepthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
			pipelineDepthStencilStateCreateInfo.pNext = nullptr;
			pipelineDepthStencilStateCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;
			pipelineDepthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
			pipelineDepthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
			pipelineDepthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
			pipelineDepthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
			pipelineDepthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
			pipelineDepthStencilStateCreateInfo.front = {};
			pipelineDepthStencilStateCreateInfo.back = {};
			pipelineDepthStencilStateCreateInfo.minDepthBounds = 0.0f;
			pipelineDepthStencilStateCreateInfo.maxDepthBounds = 1.0f;

			VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState;
			pipelineColorBlendAttachmentState.blendEnable = VK_FALSE;
			pipelineColorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
			pipelineColorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
			pipelineColorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
			pipelineColorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
			pipelineColorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
			pipelineColorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
			pipelineColorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

			VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo;
			pipelineColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			pipelineColorBlendStateCreateInfo.pNext = nullptr;
			pipelineColorBlendStateCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;
			pipelineColorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
			pipelineColorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
			pipelineColorBlendStateCreateInfo.attachmentCount = 1;
			pipelineColorBlendStateCreateInfo.pAttachments = &pipelineColorBlendAttachmentState;
			pipelineColorBlendStateCreateInfo.blendConstants[0] = 0.0f;
			pipelineColorBlendStateCreateInfo.blendConstants[1] = 0.0f;
			pipelineColorBlendStateCreateInfo.blendConstants[2] = 0.0f;
			pipelineColorBlendStateCreateInfo.blendConstants[3] = 0.0f;

			std::vector<VkDynamicState> states = { 
				VK_DYNAMIC_STATE_VIEWPORT,
				VK_DYNAMIC_STATE_SCISSOR,
			};

			VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo;
			pipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
			pipelineDynamicStateCreateInfo.pNext = nullptr;
			pipelineDynamicStateCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;
			pipelineDynamicStateCreateInfo.dynamicStateCount = (uint32_t)states.size();
			pipelineDynamicStateCreateInfo.pDynamicStates = states.data();

			// pipeline data
			VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo;
			graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			graphicsPipelineCreateInfo.pNext = nullptr;
			graphicsPipelineCreateInfo.flags = VK_FLAGS_NONE;
			graphicsPipelineCreateInfo.stageCount = (uint32_t)pipelineShaderStageCreateInfo.size();
			graphicsPipelineCreateInfo.pStages = pipelineShaderStageCreateInfo.data();
			graphicsPipelineCreateInfo.pVertexInputState = &pipelineVertexInputStateCreateInfo;
			graphicsPipelineCreateInfo.pInputAssemblyState = &pipelineInputAssemblyStateCreateInfo;
			graphicsPipelineCreateInfo.pTessellationState = &pipelineTessellationStateCreateInfo; // nullptr
			graphicsPipelineCreateInfo.pViewportState = &pipelineViewportStateCreateInfo;
			graphicsPipelineCreateInfo.pRasterizationState = &pipelineRasterizationStateCreateInfo;
			graphicsPipelineCreateInfo.pMultisampleState = &pipelineMultisampleStateCreateInfo;
			graphicsPipelineCreateInfo.pDepthStencilState = &pipelineDepthStencilStateCreateInfo;
			graphicsPipelineCreateInfo.pColorBlendState = &pipelineColorBlendStateCreateInfo;
			graphicsPipelineCreateInfo.pDynamicState = nullptr;//&pipelineDynamicStateCreateInfo; // nullptr
			graphicsPipelineCreateInfo.layout = pipelineLayout;
			graphicsPipelineCreateInfo.renderPass = renderPasses[0];
			graphicsPipelineCreateInfo.subpass = 0;
			graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
			graphicsPipelineCreateInfo.basePipelineIndex = 0;

			VkU::ErrorAdd(error, VkU::CreateGraphicsPipelines(device.handle, 1, &graphicsPipelineCreateInfo, &pipeline));
		}
		void SetupDescriptor()
		{
			std::vector<VkU::Error> error;


		}

		void PrepareForFirstRun()
		{
			std::vector<VkU::Error> error;

			// wait for any transfers being done
			VkU::ErrorAdd(error, VkU::WaitForFences(device.handle, 1, &transferManager.fence, VK_TRUE, ~0U));
		}

		void Run()
		{
			std::vector<VkU::Error> error;

			uint32_t imageIndex = ~0U;
			Swapchain* swapchain = &swapchains[0];

			// Draw
			VkU::ErrorAdd(error, VkU::WaitForFences(device.handle, 1, &imageAvailable, VK_TRUE, ~0U));
			VkU::ErrorAdd(error, VkU::ResetFences(device.handle, 1, &imageAvailable));
			VkU::ErrorAdd(error, VkU::AcquireNextImageKHR(device.handle, swapchains[0].handle, ~0U, swapchain->imageAvailableSemaphore, imageAvailable, &imageIndex));

			VkCommandBufferInheritanceInfo commandBufferInheritanceInfo;
			commandBufferInheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
			commandBufferInheritanceInfo.pNext = nullptr;
			commandBufferInheritanceInfo.renderPass = *swapchain->renderPass;
			commandBufferInheritanceInfo.subpass = 0;
			commandBufferInheritanceInfo.framebuffer = swapchain->framebuffers[imageIndex];
			commandBufferInheritanceInfo.occlusionQueryEnable = VK_FALSE;
			commandBufferInheritanceInfo.queryFlags = 0;
			commandBufferInheritanceInfo.pipelineStatistics = 0;

			VkCommandBufferBeginInfo commandBufferBeginInfo;
			commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			commandBufferBeginInfo.pNext = nullptr;
			commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
			commandBufferBeginInfo.pInheritanceInfo = &commandBufferInheritanceInfo;
			VkU::ErrorAdd(error, VkU::BeginCommandBuffer(swapchain->commandBuffers[imageIndex], &commandBufferBeginInfo));

			VkDeviceSize offset = 0;
			vkCmdBindPipeline(swapchain->commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
			vkCmdDraw(swapchain->commandBuffers[imageIndex], 3, 1, 0, 0);

			VkU::ErrorAdd(error, VkU::EndCommandBuffer(swapchain->commandBuffers[imageIndex]));

			// Update Buffers
			VkU::ErrorAdd(error, VkU::WaitForFences(device.handle, 1, &renderDone, VK_TRUE, ~0U));
			VkU::ErrorAdd(error, VkU::ResetFences(device.handle, 1, &renderDone));

			// Render
			commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			commandBufferBeginInfo.pNext = nullptr;
			commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			commandBufferBeginInfo.pInheritanceInfo = nullptr;
			VkU::ErrorAdd(error, VkU::BeginCommandBuffer(renderCommandBuffer, &commandBufferBeginInfo));

			VkClearValue clearColor[2];
			clearColor[0].color = { 0.0f, 0.0f, 0.0f, 0.0f };
			clearColor[1].depthStencil = { 1.0f, 0 };

			VkRenderPassBeginInfo renderPassBeginInfo;
			renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassBeginInfo.pNext = nullptr;
			renderPassBeginInfo.renderPass = *swapchain->renderPass;
			renderPassBeginInfo.framebuffer = swapchain->framebuffers[imageIndex];
			renderPassBeginInfo.renderArea.offset = { 0, 0 };
			renderPassBeginInfo.renderArea.extent = { swapchain->extent.width, swapchain->extent.height };
			renderPassBeginInfo.clearValueCount = sizeof(clearColor) / sizeof(VkClearValue);
			renderPassBeginInfo.pClearValues = clearColor;

			vkCmdBeginRenderPass(renderCommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
			vkCmdExecuteCommands(renderCommandBuffer, 1, &swapchain->commandBuffers[imageIndex]);
			vkCmdEndRenderPass(renderCommandBuffer);

			VkU::ErrorAdd(error, VkU::EndCommandBuffer(renderCommandBuffer));

			VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
			std::vector<VkSemaphore> waitSemaphores = { swapchain->imageAvailableSemaphore };

			VkSubmitInfo submitInfo;
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.pNext = nullptr;
			submitInfo.waitSemaphoreCount = (uint32_t)waitSemaphores.size();
			submitInfo.pWaitSemaphores = waitSemaphores.data();
			submitInfo.pWaitDstStageMask = waitStages;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &renderCommandBuffer;
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = &swapchain->renderDoneSemaphore;
			VkU::ErrorAdd(error, VkU::QueueSubmit(renderQueue, 1, &submitInfo, renderDone));

			// Present
			VkPresentInfoKHR presentInfo;
			presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			presentInfo.pNext = nullptr;
			presentInfo.waitSemaphoreCount = 1;
			presentInfo.pWaitSemaphores = &swapchain->renderDoneSemaphore;
			presentInfo.swapchainCount = 1;
			presentInfo.pSwapchains = &swapchain->handle;
			presentInfo.pImageIndices = &imageIndex;
			presentInfo.pResults = nullptr;
			VkU::ErrorAdd(error, VkU::QueuePresentKHR(renderQueue, &presentInfo));

			// Window Management
			//MSG msg;
			//while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			//{
			//	TranslateMessage(&msg);
			//	DispatchMessage(&msg);
			//}
		}

		void ShutDown()
		{
			// pipeline
			vkDestroyPipeline(device.handle, pipeline, nullptr);

			// layouts
			vkDestroyShaderModule(device.handle, shaderFrag, nullptr);
			vkDestroyShaderModule(device.handle, shaderVert, nullptr);
			vkDestroyPipelineLayout(device.handle, pipelineLayout, nullptr);
			vkDestroyDescriptorSetLayout(device.handle, descriptorSetLayout, nullptr);

			// render objects
			vkDestroyFence(device.handle, imageAvailable, nullptr);
			vkDestroyFence(device.handle, renderDone, nullptr);

			// swapchain
			for (size_t iSwapchain = 0; iSwapchain != swapchains.size(); ++iSwapchain)
			{
				vkDestroySemaphore(device.handle, swapchains[iSwapchain].imageAvailableSemaphore, nullptr);
				vkDestroySemaphore(device.handle, swapchains[iSwapchain].renderDoneSemaphore, nullptr);

				for (size_t iFence = 0; iFence != swapchains[iSwapchain].fences.size(); ++iFence)
					vkDestroyFence(device.handle, swapchains[iSwapchain].fences[iFence], nullptr);

				for (size_t iFramebuffer = 0; iFramebuffer != swapchains[iSwapchain].framebuffers.size(); ++iFramebuffer)
					vkDestroyFramebuffer(device.handle, swapchains[iSwapchain].framebuffers[iFramebuffer], nullptr);

				vkDestroyImageView(device.handle, swapchains[iSwapchain].depthImage.view, nullptr);
				vkFreeMemory(device.handle, swapchains[iSwapchain].depthImage.memory, nullptr);
				vkDestroyImage(device.handle, swapchains[iSwapchain].depthImage.handle, nullptr);

				for (size_t iView = 0; iView != swapchains[iSwapchain].views.size(); ++iView)
					vkDestroyImageView(device.handle, swapchains[iSwapchain].views[iView], nullptr);

				vkDestroySwapchainKHR(device.handle, swapchains[iSwapchain].handle, nullptr);
			}

			// render pass
			for (size_t iRenderpass = 0; iRenderpass != renderPasses.size(); ++iRenderpass)
				vkDestroyRenderPass(device.handle, renderPasses[iRenderpass], nullptr);

			// buffers
			vkDestroyBuffer(device.handle, vertexBuffer.handle, nullptr);
			vkFreeMemory(device.handle, vertexBuffer.memory, nullptr);

			vkDestroyBuffer(device.handle, transferManager.buffer.handle, nullptr);
			vkFreeMemory(device.handle, transferManager.buffer.memory, nullptr);

			// transition
			vkDestroyFence(device.handle, transferManager.fence, nullptr);

			// device
			for (size_t i = 0; i != device.queueGroups.size(); ++i)
				vkDestroyCommandPool(device.handle, device.queueGroups[i].commandPool, nullptr);

			vkDestroyDevice(device.handle, nullptr);
		}
	};

	Controller controller;

public:
	void Init()
	{
		std::vector<VkU::Error> error;

		/// INSTANCE
		VkValidationCheckEXT disabled_check = VK_VALIDATION_CHECK_ALL_EXT;
		VkValidationFlagsEXT validationFlags;
		validationFlags.sType = VK_STRUCTURE_TYPE_VALIDATION_FLAGS_EXT;
		validationFlags.pNext = nullptr;
		validationFlags.disabledValidationCheckCount = 1;
		validationFlags.pDisabledValidationChecks = &disabled_check;
		VkDebugReportCallbackCreateInfoEXT debugReportCallbackCreateInfo;
		debugReportCallbackCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
		debugReportCallbackCreateInfo.pNext = (void*)&validationFlags;
		debugReportCallbackCreateInfo.flags = VK_DEBUG_REPORT_INFORMATION_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT | VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_DEBUG_BIT_EXT;
		debugReportCallbackCreateInfo.pfnCallback = VkU::DebugReportCallback;
		debugReportCallbackCreateInfo.pUserData = nullptr;
		VkApplicationInfo applicationInfo;
		applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		applicationInfo.pNext = nullptr;
		applicationInfo.pApplicationName = "App Name";
		applicationInfo.applicationVersion = 0;
		applicationInfo.pEngineName = "Engine Name";
		applicationInfo.engineVersion = 0;
		applicationInfo.apiVersion = VK_MAKE_VERSION(1, 0, VK_HEADER_VERSION);
		std::vector<const char*> instanceLayers = { "VK_LAYER_LUNARG_standard_validation" };
		std::vector<const char*> instanceExtensions = { VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME, VK_EXT_DEBUG_REPORT_EXTENSION_NAME };
		VkInstanceCreateInfo instanceCreateInfo;
		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceCreateInfo.pNext = &debugReportCallbackCreateInfo;
		instanceCreateInfo.flags = 0;
		instanceCreateInfo.pApplicationInfo = &applicationInfo;
		instanceCreateInfo.enabledLayerCount = (uint32_t)instanceLayers.size();
		instanceCreateInfo.ppEnabledLayerNames = instanceLayers.data();
		instanceCreateInfo.enabledExtensionCount = (uint32_t)instanceExtensions.size();
		instanceCreateInfo.ppEnabledExtensionNames = instanceExtensions.data();
		VkU::ErrorAdd(error, VkU::CreateInstance(&instanceCreateInfo, gpu.instance));

		/// DEBUG
		debugReportCallbackCreateInfo.pNext = nullptr;
		VkU::ErrorAdd(error, VkU::CreateDebugReportCallbackEXT(gpu.instance, &debugReportCallbackCreateInfo, gpu.debugReportCallback));

		/// WINDOW
		gpu.windows.resize(1);
		gpu.windows[0].name = "Window Name";
		gpu.windows[0].hInstance = GetModuleHandle(NULL);

		WNDCLASSEX winClassEx;
		winClassEx.cbSize = sizeof(WNDCLASSEX);
		winClassEx.style = CS_HREDRAW | CS_VREDRAW;
		winClassEx.lpfnWndProc = VkU::MainWndProc;
		winClassEx.cbClsExtra = 0;
		winClassEx.cbWndExtra = 0;
		winClassEx.hInstance = gpu.windows[0].hInstance;
		winClassEx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		winClassEx.hCursor = LoadCursor(NULL, IDC_ARROW);
		winClassEx.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		winClassEx.lpszMenuName = NULL;
		winClassEx.lpszClassName = gpu.windows[0].name.c_str();
		winClassEx.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

		if (!RegisterClassEx(&winClassEx))
		{
			PUSH_ERROR("!RegisterClassEx");
		}

		RECT rect = { 0, 0, 800, 600 };

		DWORD dwStyle = WS_OVERLAPPEDWINDOW;

		AdjustWindowRect(&rect, dwStyle, FALSE);

		gpu.windows[0].hWnd = CreateWindowEx(0,
			gpu.windows[0].name.c_str(),
			"Title",
			dwStyle | WS_VISIBLE | WS_SYSMENU,
			0, 0,
			rect.right - rect.left,
			rect.bottom - rect.top,
			NULL,
			NULL,
			gpu.windows[0].hInstance,
			NULL);

		if (gpu.windows[0].hWnd == NULL)
		{
			UnregisterClass(gpu.windows[0].name.c_str(), gpu.windows[0].hInstance);
			PUSH_ERROR("_window.hWnd == NULL");
		}

		ShowWindow(gpu.windows[0].hWnd, SW_SHOW);
		SetForegroundWindow(gpu.windows[0].hWnd);
		SetFocus(gpu.windows[0].hWnd);

		/// SURFACE
		gpu.surfaces.resize(gpu.windows.size());
		for (size_t i = 0; i != gpu.surfaces.size(); ++i)
		{
			VkWin32SurfaceCreateInfoKHR win32SurfaceCreateInfoKHR;
			win32SurfaceCreateInfoKHR.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
			win32SurfaceCreateInfoKHR.pNext = nullptr;
			win32SurfaceCreateInfoKHR.flags = VK_RESERVED_FOR_FUTURE_USE;
			win32SurfaceCreateInfoKHR.hinstance = gpu.windows[i].hInstance;
			win32SurfaceCreateInfoKHR.hwnd = gpu.windows[i].hWnd;
			VkU::ErrorAdd(error, VkU::CreateWin32SurfaceKHR(gpu.instance, &win32SurfaceCreateInfoKHR, gpu.surfaces[i]));
		}

		/// PHYSICAL DEVICE
		std::vector<VkFormat> depthFormats = { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_X8_D24_UNORM_PACK32, VK_FORMAT_D16_UNORM, VK_FORMAT_D16_UNORM_S8_UINT };
		std::vector<VkPhysicalDevice> vkPhysicalDevices;
		VkU::ErrorAdd(error, VkU::EnumeratePhysicalDevices(gpu.instance, vkPhysicalDevices));

		gpu.physicalDevices.resize(vkPhysicalDevices.size());
		for (size_t i = 0; i != gpu.physicalDevices.size(); ++i)
		{
			gpu.physicalDevices[i].handle = vkPhysicalDevices[i];

			vkGetPhysicalDeviceFeatures(gpu.physicalDevices[i].handle, &gpu.physicalDevices[i].features);
			vkGetPhysicalDeviceMemoryProperties(gpu.physicalDevices[i].handle, &gpu.physicalDevices[i].memoryProperties);
			vkGetPhysicalDeviceProperties(gpu.physicalDevices[i].handle, &gpu.physicalDevices[i].properties);

			uint32_t propertyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(gpu.physicalDevices[i].handle, &propertyCount, nullptr);
			gpu.physicalDevices[i].queueFamilyProperties.resize(propertyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(gpu.physicalDevices[i].handle, &propertyCount, gpu.physicalDevices[i].queueFamilyProperties.data());

			gpu.physicalDevices[i].queueFamilyPresentability.resize(gpu.physicalDevices[i].queueFamilyProperties.size());
			for (uint32_t iQueueFamily = 0; iQueueFamily != gpu.physicalDevices[i].queueFamilyPresentability.size(); ++iQueueFamily)
			{
				gpu.physicalDevices[i].queueFamilyPresentability[iQueueFamily].win32Presentability = vkGetPhysicalDeviceWin32PresentationSupportKHR(gpu.physicalDevices[i].handle, iQueueFamily);

				gpu.physicalDevices[i].queueFamilyPresentability[iQueueFamily].perSurfacePresentability.resize(gpu.surfaces.size());
				for (size_t iSurface = 0; iSurface != gpu.physicalDevices[i].queueFamilyPresentability[iQueueFamily].perSurfacePresentability.size(); ++iSurface)
				{
					VkBool32* surfacePresentablePtr = &gpu.physicalDevices[i].queueFamilyPresentability[iQueueFamily].perSurfacePresentability[iSurface];
					ErrorAdd(error, VkU::GetPhysicalDeviceSurfaceSupportKHR(gpu.physicalDevices[i].handle, iQueueFamily, gpu.surfaces[i], surfacePresentablePtr));
				}
			}

			ErrorAdd(error, VkU::EnumerateDeviceLayerProperties(gpu.physicalDevices[i].handle, gpu.physicalDevices[i].layers));
			ErrorAdd(error, VkU::EnumerateDeviceExtensionProperties(gpu.physicalDevices[i].handle, gpu.physicalDevices[i].extensions));

			for (uint32_t iFormat = 0; iFormat != depthFormats.size(); ++iFormat)
			{
				VkFormatProperties formatProperties;
				vkGetPhysicalDeviceFormatProperties(gpu.physicalDevices[i].handle, depthFormats[iFormat], &formatProperties);

				if ((formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) == VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
				{
					gpu.physicalDevices[i].depthFormat = depthFormats[iFormat];
					break;
				}
			}

			gpu.physicalDevices[i].surfaceProperties.resize(gpu.surfaces.size());
			for (uint32_t iSurface = 0; iSurface != gpu.surfaces.size(); ++iSurface)
			{
				VkU::GetPhysicalDeviceSurfaceCapabilitiesKHR(gpu.physicalDevices[i].handle, gpu.surfaces[iSurface], &gpu.physicalDevices[i].surfaceProperties[iSurface].surfaceCapabilities);
				VkU::GetPhysicalDeviceSurfaceFormatsKHR(gpu.physicalDevices[i].handle, gpu.surfaces[iSurface], gpu.physicalDevices[i].surfaceProperties[iSurface].surfaceFormats);
				VkU::GetPhysicalDeviceSurfacePresentModesKHR(gpu.physicalDevices[i].handle, gpu.surfaces[iSurface], gpu.physicalDevices[i].surfaceProperties[iSurface].presentModes);
			}
		}

		controller.gpu = &gpu;

		controller.Init();
	}
	void Update()
	{
		controller.Run();
	}
	void ShutDown()
	{
		controller.ShutDown();

		// surface
		for (size_t i = 0; i != gpu.surfaces.size(); ++i)
			vkDestroySurfaceKHR(gpu.instance, gpu.surfaces[i], nullptr);

		for (size_t i = 0; i != gpu.windows.size(); ++i)
		{
			UnregisterClass(gpu.windows[i].name.c_str(), gpu.windows[i].hInstance);
			DestroyWindow(gpu.windows[i].hWnd);
		}

		// instance
		PFN_vkDestroyDebugReportCallbackEXT FP_vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(gpu.instance, "vkDestroyDebugReportCallbackEXT");
		if (FP_vkDestroyDebugReportCallbackEXT != nullptr)
			FP_vkDestroyDebugReportCallbackEXT(gpu.instance, gpu.debugReportCallback, nullptr);

		vkDestroyInstance(gpu.instance, nullptr);
	}
};

#endif