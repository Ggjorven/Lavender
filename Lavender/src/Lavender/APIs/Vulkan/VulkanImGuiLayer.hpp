#pragma once

#include "Lavender/ImGui/BaseImGuiLayer.hpp"
#include "Lavender/Utils/Utils.hpp"

#include "Lavender/APIs/Vulkan/VulkanRenderPass.hpp"

namespace Lavender
{

	class VulkanImGuiLayer : public BaseImGuiLayer
	{
	public:
		VulkanImGuiLayer() = default;
		virtual ~VulkanImGuiLayer() = default;

		void OnAttach() override;
		void OnDetach() override;

		void Begin() override;
		void End() override;

	private:
		void Resize(uint32_t width, uint32_t height) override;

	private:
		Ref<VulkanRenderPass> m_Renderpass = VK_NULL_HANDLE;
	};

}