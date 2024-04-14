#pragma once

#include <vector>

#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Renderer/Viewport.hpp"
#include "Lavender/Renderer/RenderPass.hpp"

#include "Lavender/APIs/Vulkan/VulkanImage.hpp"
#include "Lavender/APIs/Vulkan/VulkanPipeline.hpp"
#include "Lavender/APIs/Vulkan/VulkanRenderPass.hpp"
#include "Lavender/APIs/Vulkan/VulkanRenderCommandBuffer.hpp"

#include "Lavender/UI/Style.hpp"

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace Lavender
{

	class VulkanViewport : public Viewport
	{
	public:
		VulkanViewport(uint32_t width, uint32_t height);
		virtual ~VulkanViewport();

		void BeginFrame() override;
		void EndFrame() override;

		void BeginRender() override;
		void EndRender() override;

		inline uint32_t GetWidth() const override { return m_Width; }
		inline uint32_t GetHeight() const override { return m_Height; }
		inline glm::vec2 GetPosition() const override { return { m_XPos, m_YPos }; }

		bool InView(const glm::vec2& mainWindowPosition) const override;

		void Resize() override;
		void Resize(uint32_t width, uint32_t height) override;
		glm::vec2 ConvertMousePosition(const glm::vec2& mainWindowPosition) const override;

		inline Ref<RenderPass> GetRenderPass() override { return m_Renderpass; }
		ImTextureID GetImGuiTexture() override;

	private:
		Ref<Image2D> m_Image = VK_NULL_HANDLE;
		Ref<RenderPass> m_Renderpass = VK_NULL_HANDLE;

		UI::StyleList m_WindowStyle = {};
		UI::StyleColourList m_WindowColours = {};

		ImTextureID m_ImGuiImage = {};
		uint32_t m_Width = 0, m_Height = 0;
		uint32_t m_XPos = 0, m_YPos = 0;
	};

}