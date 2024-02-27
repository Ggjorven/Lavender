#include "lvpch.h"
#include "VulkanImGuiLayer.hpp"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

#include "Lavender/Core/Application.hpp"
#include "Lavender/Core/Logging.hpp"
#include "Lavender/Renderer/Renderer.hpp"

#include "Lavender/APIs/Vulkan/VulkanContext.hpp"

//#include "Lavender/Renderer/GraphicsContext.hpp"
//#include "Lavender/APIs/Vulkan/VulkanManager.hpp"
//#include "Lavender/APIs/Vulkan/VulkanActions.hpp"
//#include "Lavender/APIs/Vulkan/Setup/VulkanHelper.hpp"
//#include "Lavender/APIs/Vulkan/Setup/VulkanResources.hpp"

#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

namespace Lavender
{

	static VkDescriptorPool s_ImGuiPool = VK_NULL_HANDLE;

	void VulkanImGuiLayer::OnAttach()
	{
		auto context = RefHelper::RefAs<VulkanContext>(Renderer::GetContext());
		{
			std::vector<VkDescriptorPoolSize> poolSizes =
			{
				{ VK_DESCRIPTOR_TYPE_SAMPLER, 100 },
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 100 }, // Important for ImGui
				{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 100 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 100 },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 100 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 100 },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 100 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 100 },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 100 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 100 },
				{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 100 }
			};

			VkDescriptorPoolCreateInfo poolInfo = {};
			poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
			poolInfo.pPoolSizes = poolSizes.data();
			poolInfo.maxSets = static_cast<uint32_t>(Renderer::GetSpecification().FramesInFlight);

			if (vkCreateDescriptorPool(context->GetLogicalDevice()->GetVulkanDevice(), &poolInfo, nullptr, &s_ImGuiPool) != VK_SUCCESS)
				LV_LOG_ERROR("Failed to create descriptor pool!");
		}

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO(); (void)io;

		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

		io.IniFilename = NULL;

		ImGui::StyleColorsDark();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		ImGui_ImplGlfw_InitForVulkan(window, true);

		ImGui_ImplVulkan_InitInfo  initInfo = {};
		initInfo.Instance = context->GetVulkanInstance();
		initInfo.PhysicalDevice = context->GetPhysicalDevice()->GetVulkanPhysicalDevice();
		initInfo.Device = context->GetLogicalDevice()->GetVulkanDevice();
		initInfo.QueueFamily = QueueFamilyIndices::Find(context->GetPhysicalDevice()->GetVulkanPhysicalDevice()).GraphicsFamily.value();
		initInfo.Queue = context->GetLogicalDevice()->GetGraphicsQueue();
		//initInfo.PipelineCache = vkPipelineCache;
		initInfo.DescriptorPool = s_ImGuiPool;
		initInfo.Allocator = nullptr; // Optional, use nullptr to use the default allocator
		initInfo.MinImageCount = static_cast<uint32_t>(context->GetSwapChain()->GetImageViews().size());
		initInfo.ImageCount = static_cast<uint32_t>(context->GetSwapChain()->GetImageViews().size());
		initInfo.CheckVkResultFn = nullptr; // Optional, a callback function for Vulkan errors
		//init_info.MSAASamples = vkMSAASamples; // The number of samples per pixel in case of MSAA
		//init_info.Subpass = 0; // The index of the subpass where ImGui will be drawn


		// TODO: Implement
		/*
		// TODO(Jorben): Create some sort of renderpass for ImGui
		ImGui_ImplVulkan_Init(&initInfo, GraphicsContext::Get()->GetResources()->GetRenderPasses()[0]->m_RenderPass);

		// Create fonts
		io.Fonts->AddFontDefault();
		{
			VkCommandBuffer commandBuffer = VulkanActions::BeginSingleTimeCommands();
			ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
			VulkanActions::EndSingleTimeCommands(commandBuffer);

			ImGui_ImplVulkan_DestroyFontUploadObjects();
		}
		*/
	}

	void VulkanImGuiLayer::OnDetach()
	{
		auto device = RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetLogicalDevice()->GetVulkanDevice();

		vkDeviceWaitIdle(device);

		vkDestroyDescriptorPool(device, s_ImGuiPool, nullptr);

		// TODO: Implement ^^^
		/*
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		*/
	}

	void VulkanImGuiLayer::Begin()
	{
		/*
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		*/
	}

	void VulkanImGuiLayer::End()
	{
		/*
		ImGuiIO& io = ImGui::GetIO();

		io.DisplaySize = ImVec2((float)Application::Get().GetWindow().GetWidth(), (float)Application::Get().GetWindow().GetHeight());

		auto& resourceInfo = VulkanManager::GetResourceInfo();

		// Rendering
		ImGui::Render();
		Renderer::AddToUIQueue([this]() {
			auto& resourceInfo = VulkanManager::GetResourceInfo();

			ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), resourceInfo.CommandBuffers[Renderer::GetCurrentFrame()]);
			});

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
		*/
	}

}