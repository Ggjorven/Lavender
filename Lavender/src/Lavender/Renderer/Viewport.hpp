#pragma once

#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Renderer/RenderCommandBuffer.hpp"

#include <imgui.h>

namespace Lavender
{

	class RenderPass;
	class ViewportRenderPass;
	struct RenderPassSpecification;

	class ViewportImage
	{
	public:
		ViewportImage() = default;
		virtual ~ViewportImage() = default;

		virtual void Resize(uint32_t width, uint32_t height) = 0;

		static Ref<ViewportImage> Create(uint32_t width, uint32_t height);
	};

	class ViewportRenderPass
	{
	public:
		ViewportRenderPass() = default;
		virtual ~ViewportRenderPass() = default;

		virtual void Begin() = 0;
		virtual void End() = 0;
		virtual void Submit() = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;

		virtual Ref<RenderPass> GetRenderPass() = 0;
		virtual Ref<RenderCommandBuffer> GetCommandBuffer() = 0;

		static Ref<ViewportRenderPass> Create(Ref<ViewportImage> image);
	};

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

		virtual void Resize(uint32_t width, uint32_t height) = 0;

		virtual Ref<ViewportRenderPass> GetRenderPass() = 0;
		virtual ImTextureID GetCurrentImGuiTexture() = 0;

		static Ref<Viewport> Create(uint32_t width, uint32_t height);
	};

}