#pragma once

#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Renderer/RenderPass.hpp"
#include "Lavender/Renderer/RenderCommandBuffer.hpp"

#include <imgui.h>

namespace Lavender
{

	// Note(Jorben): This class should only be used in a non-Dist build aka only in non shipped games
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

		virtual void SetShouldResize(bool shouldResize) = 0;
		virtual void Resize(uint32_t width, uint32_t height) = 0;

		virtual Ref<RenderPass> GetRenderPass() = 0;
		virtual ImTextureID GetCurrentImGuiTexture() = 0;

		static Ref<Viewport> Create(uint32_t width, uint32_t height);
	};

}