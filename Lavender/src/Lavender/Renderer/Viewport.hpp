#pragma once

#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Renderer/RenderPass.hpp"
#include "Lavender/Renderer/RenderCommandBuffer.hpp"

#include <glm/glm.hpp>

#include <imgui.h>

namespace Lavender
{

	class Pipeline;
	class UniformBuffer;

	// TODO: Fix for runtime with removing ImGui
	class Viewport
	{
	public:
		Viewport() = default;
		virtual ~Viewport() = default;

		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;

		virtual void BeginRender() = 0;
		virtual void EndRender() = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual glm::vec2 GetPosition() const = 0;

		virtual bool InView(const glm::vec2& mainWindowPosition) const = 0;
		virtual glm::vec2 ConvertMousePosition(const glm::vec2& mainWindowPosition) const = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;

		virtual Ref<RenderPass> GetRenderPass() = 0;
		virtual ImTextureID GetImGuiTexture() = 0;

		static Ref<Viewport> Create(uint32_t width, uint32_t height);
	};

}