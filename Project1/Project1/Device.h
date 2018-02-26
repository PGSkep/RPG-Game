#ifndef	DEVICE_H
#define DEVICE_H

#include "VkU.h"

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
class Device
{
public:
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

	std::vector<Error> Init(std::vector<const char*> deviceExtensions, VkPhysicalDeviceFeatures _features, std::vector<Device::QueueFamilyRequirements> _queueFamilyRequirements, std::vector<VkSurfaceKHR> _surfaces, PhysicalDevice* _physicalDevice, uint32_t _validIndexPick)
	{
		std::vector<Error> error;

		std::vector<std::vector<uint32_t>> validIndices;
		bool found;

		physicalDevice = _physicalDevice;

		// validate extensions
		for (size_t i = 0; i != deviceExtensions.size(); ++i)
		{
			found = false;
			std::vector<VkExtensionProperties>* availableExtensions = &physicalDevice->extensions;
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
			if (((VkBool32*)&_features)[i] == VK_TRUE && ((VkBool32*)&physicalDevice->features)[i] == VK_FALSE)
			{
				PUSH_ERROR("feature " + std::to_string(i));
			}
		}

		// Get valid indices
		size_t queueCount = physicalDevice->queueFamilyProperties.size();

		// get number of iterations
		size_t numberOfIterations = (size_t)pow(queueCount, _queueFamilyRequirements.size());

		// iterate
		std::vector<uint32_t> indices(_queueFamilyRequirements.size());
		for (size_t iterationNum = 0; iterationNum != numberOfIterations; ++iterationNum)
		{
			// overflow to next when needed
			if (_queueFamilyRequirements.size() > 1)
			{
				for (size_t iReq = 0; iReq != _queueFamilyRequirements.size(); ++iReq)
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
			for (size_t iReq = 0; iReq != _queueFamilyRequirements.size(); ++iReq)
			{
				uint32_t familyIndex = indices[iReq];

				// count
				if (physicalDevice->queueFamilyProperties[familyIndex].queueCount < familyQueueUseCount[familyIndex] + _queueFamilyRequirements[iReq].priorities.size())
				{
					valid = false;
					break;
				}

				// flags
				if ((_queueFamilyRequirements[iReq].flags & physicalDevice->queueFamilyProperties[familyIndex].queueFlags) != _queueFamilyRequirements[iReq].flags)
				{
					valid = false;
					break;
				}

				// presentation
				if (_queueFamilyRequirements[iReq].presentability == true)
				{
					if (physicalDevice->queueFamilyPresentability[familyIndex].win32Presentability == VK_FALSE)
					{
						valid = false;
						break;
					}

					VkBool32 surfaceSupport = VK_TRUE;
					physicalDevice->queueFamilyPresentability[familyIndex].perSurfacePresentability.resize(_surfaces.size());
					for (size_t iSurface = 0; iSurface != _surfaces.size(); ++iSurface)
					{
						if (physicalDevice->queueFamilyPresentability[familyIndex].perSurfacePresentability[iSurface] == VK_FALSE)
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

		for (size_t iValid = 0; iValid != validIndices[_validIndexPick].size(); ++iValid)
		{
			bool found = false;
			for (size_t iInfo = 0; iInfo != deviceQueueCreateInfo.size(); ++iInfo)
			{
				if (validIndices[_validIndexPick][iValid] == deviceQueueCreateInfo[iInfo].queueFamilyIndex)
				{
					found = true;
					deviceQueueCreateInfo[iInfo].queueCount += (uint32_t)_queueFamilyRequirements[iValid].priorities.size();

					for (size_t iPriorities = 0; iPriorities != _queueFamilyRequirements[iValid].priorities.size(); ++iPriorities)
					{
						priorities[iInfo].push_back(_queueFamilyRequirements[iValid].priorities[iPriorities]);
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
				deviceQueueCreateInfo[deviceQueueCreateInfo.size() - 1].queueFamilyIndex = validIndices[_validIndexPick][iValid];
				deviceQueueCreateInfo[deviceQueueCreateInfo.size() - 1].queueCount = (uint32_t)_queueFamilyRequirements[iValid].priorities.size();

				priorities.push_back({});
				for (size_t iPriorities = 0; iPriorities != _queueFamilyRequirements[iValid].priorities.size(); ++iPriorities)
					priorities[priorities.size() - 1].push_back(_queueFamilyRequirements[iValid].priorities[iPriorities]);

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
		deviceCreateInfo.pEnabledFeatures = &_features;
		Error::Add(error, VkU::CreateDevice(physicalDevice->handle, &deviceCreateInfo, handle));

		/// QUEUE
		queueGroups.resize(deviceQueueCreateInfo.size());
		for (size_t iGroup = 0; iGroup != queueGroups.size(); ++iGroup)
		{
			queueGroups[iGroup].familyIndex = deviceQueueCreateInfo[iGroup].queueFamilyIndex;
			queueGroups[iGroup].flags = physicalDevice->queueFamilyProperties[queueGroups[iGroup].familyIndex].queueFlags;
			queueGroups[iGroup].presentability = physicalDevice->queueFamilyPresentability[queueGroups[iGroup].familyIndex];
			queueGroups[iGroup].priority = priorities[queueGroups[iGroup].familyIndex];
			queueGroups[iGroup].queues.resize(queueGroups[iGroup].priority.size());

			for (size_t iQueue = 0; iQueue != queueGroups[iGroup].queues.size(); ++iQueue)
				vkGetDeviceQueue(handle, queueGroups[iGroup].familyIndex, (uint32_t)iQueue, &queueGroups[iGroup].queues[iQueue]);
		}

		return error;
	}
};

#endif