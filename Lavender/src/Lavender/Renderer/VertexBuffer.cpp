#include "lvpch.h"
#include "VertexBuffer.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Renderer/Renderer.hpp"

#include "Lavender/APIs/Vulkan/VulkanVertexBuffer.hpp"

namespace Lavender
{

	Ref<VertexBuffer> VertexBuffer::Create(void* data, size_t size)
	{
		switch (Renderer::GetAPI())
		{
		case RenderingAPI::Vulkan:
			return RefHelper::Create<VulkanVertexBuffer>(data, size);

		default:
			LV_LOG_ERROR("Invalid API selected.");
			break;
		}

		return nullptr;
	}

}