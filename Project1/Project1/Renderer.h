#ifndef	RENDERER_H
#define RENDERER_H

#include <math.h>
#include <vector>

#include "Console.h"

#include "Error.h"
#include "VkU.h"
#include "Device.h"
#include "Window.h"
#include "TransferManager.h"

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
		
		uint32_t graphicsGroupID;
		uint32_t graphicsQueueID;

		TransferManager transferManager;

		BufferGroup vertexBufferGroup;
		BufferGroup indexBufferGroup;
		
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
			std::vector<Error> error;

			/// DEVICE
			uint32_t physicalDevicePick = 0;
			uint32_t validIndexPick = 0;
			std::vector<const char*> deviceExtensions = { "VK_KHR_swapchain" };
			VkPhysicalDeviceFeatures features = VkU::PhysicalDevivceGetFeaturesFromStrArr({ "fillModeNonSolid", "multiViewport", "tessellationShader", "geometryShader" });
			queueFamilyRequirements = {
				Device::QueueFamilyRequirements::Get(VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT,	true,{ 1.0f }),
			};

			std::vector<Error> deviceInitErrors = device.Init(deviceExtensions, features, queueFamilyRequirements, gpu->surfaces, &gpu->physicalDevices[physicalDevicePick], validIndexPick);
			Error::Add(error, deviceInitErrors);

			graphicsGroupID = 0;
			graphicsQueueID = 0;

			/// COMMAND POOL
			VkCommandPoolCreateInfo commandPoolCreateInfo;
			commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			commandPoolCreateInfo.pNext = nullptr;
			commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			for (size_t i = 0; i != device.queueGroups.size(); ++i)
			{
				commandPoolCreateInfo.queueFamilyIndex = device.queueGroups[i].familyIndex;
				Error::Add(error, VkU::CreateCommandPool(device.handle, &commandPoolCreateInfo, &device.queueGroups[i].commandPool));
			}
		}
		void SetupTransferManager()
		{
			std::vector<Error> error;

			uint32_t groupID = 0;
			uint32_t queueID = 0;

			// Command buffer
			VkCommandBuffer commandBuffer;
			VkCommandBufferAllocateInfo commandBufferAllocateInfo;
			commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			commandBufferAllocateInfo.pNext = nullptr;
			commandBufferAllocateInfo.commandPool = device.queueGroups[groupID].commandPool;
			commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			commandBufferAllocateInfo.commandBufferCount = 1;
			Error::Add(error, VkU::AllocateCommandBuffers(device.handle, &commandBufferAllocateInfo, &commandBuffer));

			transferManager.Init(device, commandBuffer, device.queueGroups[groupID].queues[queueID], sizeof(float) * 1024);
		}
		void SetupMemory()
		{
			std::vector<Error> error;

			// data
			float vertData[] = {
				//x    y            r     g     b
				-0.5f/2, +0.86603f/2,	0.0f, 0.0f, 1.0f,
				-1.0f/2, +0.00000f/2,	0.0f, 1.0f, 0.0f,
				-0.5f/2, -0.86603f/2,	0.0f, 1.0f, 1.0f,

				+0.5f/2, -0.86603f/2,	1.0f, 0.0f, 0.0f,
				+1.0f/2, +0.00000f/2,	1.0f, 0.0f, 1.0f,
				+0.5f/2, +0.86603f/2,	1.0f, 1.0f, 0.0f,

				-0.375f, -0.433015f,
				+0.375f, +0.0f,
				-5.0f, +0.0f,
				-5.0f, -0.0f,
			};

			uint16_t indxData[] = {
				0, 1, 2,
				0, 2, 3,
				0, 3, 5,
				3, 4, 5,
			};

			// vertex
			Error::Add(error, vertexBufferGroup.Init(device, sizeof(vertData), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT));

			// index
			Error::Add(error, indexBufferGroup.Init(device, sizeof(vertData), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT));

			// cpy
			Error::Add(error, transferManager.Transfer(device.handle, vertexBufferGroup.handle, vertData, sizeof(vertData)));
			Error::Add(error, transferManager.Transfer(device.handle, indexBufferGroup.handle, indxData, sizeof(indxData)));
		}
		void SetupSwapchain()
		{
			std::vector<Error> error;

			Error::Add(error, VkU::WaitForFences(device.handle, 1, &transferManager.fence, VK_TRUE, ~0U));
			Error::Add(error, VkU::ResetFences(device.handle, 1, &transferManager.fence));

			VkCommandBufferBeginInfo commandBufferBeginInfo;
			commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			commandBufferBeginInfo.pNext = nullptr;
			commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			commandBufferBeginInfo.pInheritanceInfo = nullptr;
			Error::Add(error, VkU::BeginCommandBuffer(transferManager.commandBuffer, &commandBufferBeginInfo));

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

				Error::Add(error, VkU::CreateSwapchain(device.handle, &swapchainCreateInfos[iSwapchain], &swapchains[iSwapchain].handle));

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
					Error::Add(error, VkU::CreateRenderPass(device.handle, &renderPassCreateInfo, &renderPasses[renderPasses.size() - 1]));

					//swapchains[iSwapchain].renderPass = (VkRenderPass*)(renderPasses.size() - 1);
					swapchains[iSwapchain].renderPass = &renderPasses[renderPasses.size() - 1];
				}

				/// SWAPCHAIN OTHER
				// images
				Error::Add(error, VkU::GetSwapchainImagesKHR(device.handle, swapchains[iSwapchain].handle, swapchains[iSwapchain].images));

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
					Error::Add(error, VkU::CreateImageView(device.handle, &imageViewCreateInfo, &swapchains[iSwapchain].views[iView]));
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
				Error::Add(error, VkU::CreateImage(device.handle, &imageCreateInfo, &swapchains[iSwapchain].depthImage.handle));

				// Depth memory
				VkMemoryRequirements memoryRequirements;
				vkGetImageMemoryRequirements(device.handle, swapchains[iSwapchain].depthImage.handle, &memoryRequirements);

				VkMemoryAllocateInfo memoryAllocateInfo;
				memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
				memoryAllocateInfo.pNext = nullptr;
				memoryAllocateInfo.allocationSize = memoryRequirements.size;
				memoryAllocateInfo.memoryTypeIndex = VkU::FindMemoryType(device.physicalDevice->memoryProperties, memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
				Error::Add(error, VkU::AllocateMemory(device.handle, &memoryAllocateInfo, &swapchains[iSwapchain].depthImage.memory));

				Error::Add(error, VkU::BindImageMemory(device.handle, swapchains[iSwapchain].depthImage.handle, swapchains[iSwapchain].depthImage.memory, 0));

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
				Error::Add(error, VkU::CreateImageView(device.handle, &imageViewCreateInfo, &swapchains[iSwapchain].depthImage.view));

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

					Error::Add(error, VkU::CreateFramebuffer(device.handle, &framebufferCreateInfo, &swapchains[iSwapchain].framebuffers[iFramebuffer]));
				}

				// Fences
				VkFenceCreateInfo fenceCreateInfo;
				fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
				fenceCreateInfo.pNext = nullptr;
				fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
				swapchains[iSwapchain].fences.resize(swapchains[iSwapchain].images.size());
				for (size_t iFence = 0; iFence != swapchains[iSwapchain].views.size(); ++iFence)
					Error::Add(error, VkU::CreateFence(device.handle, &fenceCreateInfo, &swapchains[iSwapchain].fences[iFence]));

				// Semaphores
				VkSemaphoreCreateInfo semaphoreCreateInfo;
				semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
				semaphoreCreateInfo.pNext = nullptr;
				semaphoreCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;
				Error::Add(error, VkU::CreateSemaphore_(device.handle, &semaphoreCreateInfo, &swapchains[iSwapchain].imageAvailableSemaphore));
				Error::Add(error, VkU::CreateSemaphore_(device.handle, &semaphoreCreateInfo, &swapchains[iSwapchain].renderDoneSemaphore));

				// Command Buffer
				swapchains[iSwapchain].commandBuffers.resize(swapchains[iSwapchain].images.size());
				VkCommandBufferAllocateInfo commandBufferAllocateInfo;
				commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
				commandBufferAllocateInfo.pNext = nullptr;
				commandBufferAllocateInfo.commandPool = device.queueGroups[graphicsGroupID].commandPool;
				commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
				commandBufferAllocateInfo.commandBufferCount = (uint32_t)swapchains[iSwapchain].commandBuffers.size();
				Error::Add(error, VkU::AllocateCommandBuffers(device.handle, &commandBufferAllocateInfo, swapchains[iSwapchain].commandBuffers.data()));
			}

			Error::Add(error, VkU::EndCommandBuffer(transferManager.commandBuffer));

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
			Error::Add(error, VkU::QueueSubmit(transferManager.queue, 1, &submitInfo, transferManager.fence));
		}
		void SetupRender()
		{
			std::vector<Error> error;

			VkFenceCreateInfo fenceCreateInfo;
			fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fenceCreateInfo.pNext = nullptr;
			fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
			Error::Add(error, VkU::CreateFence(device.handle, &fenceCreateInfo, &imageAvailable));
			Error::Add(error, VkU::CreateFence(device.handle, &fenceCreateInfo, &renderDone));

			VkCommandBufferAllocateInfo commandBufferAllocateInfo;
			commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			commandBufferAllocateInfo.pNext = nullptr;
			commandBufferAllocateInfo.commandPool = device.queueGroups[0].commandPool;
			commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			commandBufferAllocateInfo.commandBufferCount = 1;
			Error::Add(error, VkU::AllocateCommandBuffers(device.handle, &commandBufferAllocateInfo, &renderCommandBuffer));

			renderQueue = device.queueGroups[graphicsGroupID].queues[graphicsQueueID];
		}
		void SetupLayouts()
		{
			std::vector<Error> error;

			/// DESCRIPTOR SET LAYOUT
			VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo;
			descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			descriptorSetLayoutCreateInfo.pNext = nullptr;
			descriptorSetLayoutCreateInfo.flags = VK_FLAGS_NONE;
			descriptorSetLayoutCreateInfo.bindingCount = 0;
			descriptorSetLayoutCreateInfo.pBindings = nullptr;
			Error::Add(error, VkU::CreateDescriptorSetLayout(device.handle, &descriptorSetLayoutCreateInfo, &descriptorSetLayout));

			/// PIPELINE LAYOUT
			VkPushConstantRange pushConstantRange;
			pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
			pushConstantRange.offset = 0;
			pushConstantRange.size = sizeof(float) * 4 + sizeof(uint32_t);

			VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo;
			pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			pipelineLayoutCreateInfo.pNext = nullptr;
			pipelineLayoutCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;
			pipelineLayoutCreateInfo.setLayoutCount = 0;
			pipelineLayoutCreateInfo.pSetLayouts = nullptr;
			pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
			pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;
			Error::Add(error, VkU::CreatePipelineLayout(device.handle, &pipelineLayoutCreateInfo, &pipelineLayout));

			/// SHADER MODULE
			std::vector<char> shaderCode;
			VkShaderModuleCreateInfo shaderModuleCreateInfo;
			shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			shaderModuleCreateInfo.pNext = nullptr;
			shaderModuleCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;

			VkU::GetShaderCode("../../Data/Shaders/vert.spv", shaderCode);
			shaderModuleCreateInfo.codeSize = shaderCode.size();
			shaderModuleCreateInfo.pCode = (uint32_t*)shaderCode.data();
			Error::Add(error, VkU::CreateShaderModule(device.handle, &shaderModuleCreateInfo, &shaderVert));

			VkU::GetShaderCode("../../Data/Shaders/frag.spv", shaderCode);
			shaderModuleCreateInfo.codeSize = shaderCode.size();
			shaderModuleCreateInfo.pCode = (uint32_t*)shaderCode.data();
			Error::Add(error, VkU::CreateShaderModule(device.handle, &shaderModuleCreateInfo, &shaderFrag));
		}
		void SetupPipeline()
		{
			std::vector<Error> error;

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

			std::vector<VkVertexInputBindingDescription> vertexInputBindingDescription(1);
			vertexInputBindingDescription[0].binding = 0;
			vertexInputBindingDescription[0].stride = sizeof(float) * 5;
			vertexInputBindingDescription[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescription(2);
			vertexInputAttributeDescription[0].location = 0;
			vertexInputAttributeDescription[0].binding = 0;
			vertexInputAttributeDescription[0].format = VK_FORMAT_R32G32_SFLOAT;
			vertexInputAttributeDescription[0].offset = 0;
			vertexInputAttributeDescription[1].location = 1;
			vertexInputAttributeDescription[1].binding = 0;
			vertexInputAttributeDescription[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			vertexInputAttributeDescription[1].offset = sizeof(float) * 2;

			VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo;
			pipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			pipelineVertexInputStateCreateInfo.pNext = nullptr;
			pipelineVertexInputStateCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;
			pipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = (uint32_t)vertexInputBindingDescription.size();
			pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = vertexInputBindingDescription.data();
			pipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = (uint32_t)vertexInputAttributeDescription.size();
			pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexInputAttributeDescription.data();

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

			Error::Add(error, VkU::CreateGraphicsPipelines(device.handle, 1, &graphicsPipelineCreateInfo, &pipeline));
		}
		void SetupDescriptor()
		{
			std::vector<Error> error;


		}

		void PrepareForFirstRun()
		{
			std::vector<Error> error;

			// wait for any transfers being done
			Error::Add(error, VkU::WaitForFences(device.handle, 1, &transferManager.fence, VK_TRUE, ~0U));
		}

		void Run(float _deltaTime)
		{
			std::vector<Error> error;

			uint32_t imageIndex = ~0U;
			Swapchain* swapchain = &swapchains[0];

			// Draw
			Error::Add(error, VkU::WaitForFences(device.handle, 1, &imageAvailable, VK_TRUE, ~0U));
			Error::Add(error, VkU::ResetFences(device.handle, 1, &imageAvailable));
			Error::Add(error, VkU::AcquireNextImageKHR(device.handle, swapchains[0].handle, ~0U, swapchain->imageAvailableSemaphore, imageAvailable, &imageIndex));

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
			Error::Add(error, VkU::BeginCommandBuffer(swapchain->commandBuffers[imageIndex], &commandBufferBeginInfo));

			vkCmdBindPipeline(swapchain->commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

			VkDeviceSize offsets[] = { 0 };
			VkBuffer vertexBuffers[] = { vertexBufferGroup.handle };

			struct PushConstant
			{
				float posX;
				float posY;
				float aspect;
				float zoom;
				uint32_t hexPerLine;
			};
			static PushConstant pushConstant = {
				0.0f,
				0.0f,
				(float)swapchains[0].extent.width / (float)swapchains[0].extent.height,
				0.5f,
				25,
			};
			if (Input::IsDown(Input::A))
				pushConstant.posX += _deltaTime / pushConstant.zoom;
			if (Input::IsDown(Input::D))
				pushConstant.posX -= _deltaTime / pushConstant.zoom;

			if (Input::IsDown(Input::W))
				pushConstant.posY += _deltaTime / pushConstant.zoom;
			if (Input::IsDown(Input::S))
				pushConstant.posY -= _deltaTime / pushConstant.zoom;

			if (Input::IsDown(Input::Q))
				pushConstant.zoom += pushConstant.zoom * _deltaTime / 2.0f;
			if (Input::IsDown(Input::E))
				pushConstant.zoom -= pushConstant.zoom * _deltaTime / 2.0f;

			if (Input::IsPressed(Input::ARROW_LEFT))
				--pushConstant.hexPerLine;
			if (Input::IsPressed(Input::ARROW_RIGHT))
				++pushConstant.hexPerLine;

			vkCmdPushConstants(swapchain->commandBuffers[imageIndex], pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstant), &pushConstant);

			vkCmdBindVertexBuffers(swapchain->commandBuffers[imageIndex], 0, sizeof(vertexBuffers) / sizeof(VkBuffer), vertexBuffers, offsets);
			vkCmdBindIndexBuffer(swapchain->commandBuffers[imageIndex], indexBufferGroup.handle, 0, VK_INDEX_TYPE_UINT16);
			
			vkCmdDrawIndexed(swapchain->commandBuffers[imageIndex], 12, 625, 0, 0, 0);

			Error::Add(error, VkU::EndCommandBuffer(swapchain->commandBuffers[imageIndex]));

			// Update Buffers
			Error::Add(error, VkU::WaitForFences(device.handle, 1, &renderDone, VK_TRUE, ~0U));
			Error::Add(error, VkU::ResetFences(device.handle, 1, &renderDone));

			// Render
			commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			commandBufferBeginInfo.pNext = nullptr;
			commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			commandBufferBeginInfo.pInheritanceInfo = nullptr;
			Error::Add(error, VkU::BeginCommandBuffer(renderCommandBuffer, &commandBufferBeginInfo));

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

			Error::Add(error, VkU::EndCommandBuffer(renderCommandBuffer));

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
			Error::Add(error, VkU::QueueSubmit(renderQueue, 1, &submitInfo, renderDone));

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
			Error::Add(error, VkU::QueuePresentKHR(renderQueue, &presentInfo));
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
			indexBufferGroup.ShutDown(device.handle);
			vertexBufferGroup.ShutDown(device.handle);
			transferManager.ShutDown(device.handle);

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
		std::vector<Error> error;

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
		Error::Add(error, VkU::CreateInstance(&instanceCreateInfo, gpu.instance));

		/// DEBUG
		debugReportCallbackCreateInfo.pNext = nullptr;
		Error::Add(error, VkU::CreateDebugReportCallbackEXT(gpu.instance, &debugReportCallbackCreateInfo, gpu.debugReportCallback));

		/// WINDOW
		gpu.windows.resize(1);
		gpu.windows[0].Init("Window Name", "Title", VkU::MainWndProc, 800, 600);

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
			Error::Add(error, VkU::CreateWin32SurfaceKHR(gpu.instance, &win32SurfaceCreateInfoKHR, gpu.surfaces[i]));
		}

		/// PHYSICAL DEVICE
		std::vector<VkFormat> depthFormats = { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_X8_D24_UNORM_PACK32, VK_FORMAT_D16_UNORM, VK_FORMAT_D16_UNORM_S8_UINT };
		std::vector<VkPhysicalDevice> vkPhysicalDevices;
		Error::Add(error, VkU::EnumeratePhysicalDevices(gpu.instance, vkPhysicalDevices));

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
					Error::Add(error, VkU::GetPhysicalDeviceSurfaceSupportKHR(gpu.physicalDevices[i].handle, iQueueFamily, gpu.surfaces[i], surfacePresentablePtr));
				}
			}

			Error::Add(error, VkU::EnumerateDeviceLayerProperties(gpu.physicalDevices[i].handle, gpu.physicalDevices[i].layers));
			Error::Add(error, VkU::EnumerateDeviceExtensionProperties(gpu.physicalDevices[i].handle, gpu.physicalDevices[i].extensions));

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
	void Update(float _deltaTime)
	{
		controller.Run(_deltaTime);
	}
	void ShutDown()
	{
		controller.ShutDown();

		// surface
		for (size_t i = 0; i != gpu.surfaces.size(); ++i)
			vkDestroySurfaceKHR(gpu.instance, gpu.surfaces[i], nullptr);

		for (size_t i = 0; i != gpu.windows.size(); ++i)
			gpu.windows[i].ShutDown();

		// instance
		PFN_vkDestroyDebugReportCallbackEXT FP_vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(gpu.instance, "vkDestroyDebugReportCallbackEXT");
		if (FP_vkDestroyDebugReportCallbackEXT != nullptr)
			FP_vkDestroyDebugReportCallbackEXT(gpu.instance, gpu.debugReportCallback, nullptr);

		vkDestroyInstance(gpu.instance, nullptr);
	}
};

#endif