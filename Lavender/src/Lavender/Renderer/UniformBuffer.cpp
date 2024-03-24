#include "lvpch.h"
#include "UniformBuffer.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Renderer/Renderer.hpp"

#include "Lavender/APIs/Vulkan/VulkanUniformBuffer.hpp"

namespace Lavender
{

	Ref<UniformBuffer> UniformBuffer::Create(size_t dataSize)
	{
		switch (Renderer::GetAPI())
		{
		case RenderingAPI::Vulkan:
			return RefHelper::Create<VulkanUniformBuffer>(dataSize);

		default:
			LV_LOG_ERROR("Invalid API selected.");
			break;
		}

		return nullptr;
	}

	Ref<UniformBuffer> UniformBuffer::Create(Ref<DescriptorSet> set, UniformElement element, size_t dataSize)
	{
		switch (Renderer::GetAPI())
		{
		case RenderingAPI::Vulkan:
			return RefHelper::Create<VulkanUniformBuffer>(set, element, dataSize);

		default:
			LV_LOG_ERROR("Invalid API selected.");
			break;
		}

		return nullptr;
	}

}