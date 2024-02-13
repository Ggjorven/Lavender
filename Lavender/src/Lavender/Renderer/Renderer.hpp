#pragma once

#include <memory>
#include <queue>

#include <glm/glm.hpp>

#include "Lavender/Renderer/RenderingAPI.hpp"
#include "Lavender/APIs/Vulkan/VulkanRenderPass.hpp"

namespace Lavender
{

	class Application;
	class VertexBuffer;
	class IndexBuffer;

	typedef std::function<void()> RenderFunction;
	typedef std::function<void()> UIFunction;

	class Renderer
	{
	public:
		static void Init(const APISpecifications& specs);
		static void Destroy();

		static void Wait();

		static void SetClearColour(const glm::vec4& colour);

		// Note(Jorben): Make sure you have a RenderController bound
		static void DrawIndexed(std::shared_ptr<IndexBuffer>& indexBuffer);

		static void AddToQueue(RenderFunction function);
		static void AddToUIQueue(UIFunction function);

		static APISpecifications& GetAPISpecs() { return s_APISpecs; }

		static void OnResize(uint32_t width, uint32_t height);

		static uint32_t GetCurrentFrame() { return s_Instance->m_CurrentFrame; }

	private:
		void QueuePresent();
		void RecordCommandBuffer(VkCommandBuffer& commandBuffer, uint32_t imageIndex);

		static void Display();

	private:
		uint32_t m_CurrentFrame = 0;

		VulkanRenderPass* m_DefaultRenderPass = nullptr;

		std::queue<RenderFunction> m_RenderQueue = { };
		std::queue<UIFunction> m_UIQueue = { };

	private:
		static Renderer* s_Instance;
		static APISpecifications s_APISpecs;

		friend class Application;
	};

}