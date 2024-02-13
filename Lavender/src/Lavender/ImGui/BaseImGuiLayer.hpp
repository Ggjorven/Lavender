#pragma once

#include "Lavender/Core/Layer.hpp"

#include <vulkan/vulkan.h>

namespace Lavender
{

	class BaseImGuiLayer : public Layer
	{
	public:
		BaseImGuiLayer() = default;
		virtual ~BaseImGuiLayer() = default;

		void OnAttach() override;
		void OnDetach() override;

		void Begin();
		void End();

	private:
		void CreateDescriptorPool();

	private:
		VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;
	};

}