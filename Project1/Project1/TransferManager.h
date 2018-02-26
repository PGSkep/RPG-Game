#ifndef	TRANSFER_MANAGER_H
#define TRANSFER_MANAGER_H

#include "VkU.h"
#include "Device.h"
#include "BufferGroup.h"
#include <assert.h>

class TransferManager
{
	VkDeviceSize size;

public:
	VkQueue queue;
	VkCommandBuffer commandBuffer;
	VkFence fence;

	BufferGroup transferBufferGroup;

	std::vector<Error> Init(Device _device, VkCommandBuffer _commandBuffer, VkQueue _queue, VkDeviceSize _size)
	{
		std::vector<Error> error;

		size = _size;

		VkFenceCreateInfo fenceCreateInfo;
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.pNext = nullptr;
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		Error::Add(error, VkU::CreateFence(_device.handle, &fenceCreateInfo, &fence));

		commandBuffer = _commandBuffer;

		queue = _queue;

		VkBufferCreateInfo bufferCreateInfo;
		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.pNext = nullptr;
		bufferCreateInfo.flags = VK_FLAGS_NONE;
		bufferCreateInfo.size = size;
		bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		bufferCreateInfo.queueFamilyIndexCount = 0;
		bufferCreateInfo.pQueueFamilyIndices = nullptr;
		Error::Add(error, VkU::CreateBuffer(_device.handle, &bufferCreateInfo, &transferBufferGroup.handle));

		VkMemoryRequirements memoryRequirements;
		vkGetBufferMemoryRequirements(_device.handle, transferBufferGroup.handle, &memoryRequirements);

		VkMemoryAllocateInfo memoryAllocateInfo;
		memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocateInfo.pNext = nullptr;
		memoryAllocateInfo.allocationSize = memoryRequirements.size;
		memoryAllocateInfo.memoryTypeIndex = VkU::FindMemoryType(_device.physicalDevice->memoryProperties, memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		Error::Add(error, VkU::AllocateMemory(_device.handle, &memoryAllocateInfo, &transferBufferGroup.memory));

		Error::Add(error, VkU::BindBufferMemory(_device.handle, transferBufferGroup.handle, transferBufferGroup.memory, 0));

		return error;
	}
	
	std::vector<Error> Transfer(VkDevice _device, VkBuffer _target, void* _data, size_t _size)
	{
		if (_size > size)
			assert("Transfer is too large");

		std::vector<Error> error;

		Error::Add(error, VkU::WaitForFences(_device, 1, &fence, VK_TRUE, ~0U));
		Error::Add(error, VkU::ResetFences(_device, 1, &fence));

		void* data = nullptr;
		Error::Add(error, VkU::MapMemory(_device, transferBufferGroup.memory, 0, size, &data));

		memcpy(data, _data, _size);

		vkUnmapMemory(_device, transferBufferGroup.memory);

		VkCommandBufferBeginInfo commandBufferBeginInfo;
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.pNext = nullptr;
		commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		commandBufferBeginInfo.pInheritanceInfo = nullptr;
		Error::Add(error, VkU::BeginCommandBuffer(commandBuffer, &commandBufferBeginInfo));

		VkBufferCopy bufferCopy;
		bufferCopy.srcOffset = 0;
		bufferCopy.dstOffset = 0;
		bufferCopy.size = _size;
		vkCmdCopyBuffer(commandBuffer, transferBufferGroup.handle, _target, 1, &bufferCopy);

		Error::Add(error, VkU::EndCommandBuffer(commandBuffer));

		VkSubmitInfo submitInfo;
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pNext = nullptr;
		submitInfo.waitSemaphoreCount = 0;
		submitInfo.pWaitSemaphores = nullptr;
		submitInfo.pWaitDstStageMask = nullptr;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;
		submitInfo.signalSemaphoreCount = 0;
		submitInfo.pSignalSemaphores = nullptr;
		Error::Add(error, VkU::QueueSubmit(queue, 1, &submitInfo, fence));

		return error;
	}

	void ShutDown(VkDevice _device)
	{
		vkDestroyBuffer(_device, transferBufferGroup.handle, nullptr);
		vkFreeMemory(_device, transferBufferGroup.memory, nullptr);
		vkDestroyFence(_device, fence, nullptr);
	}
};

#endif