#pragma once

#include "Lavender/Utils/Utils.hpp"

namespace Lavender
{

	class RenderCommandBuffer;

	struct RenderPassSpecification // TODO: Add more
	{
	public:
		enum class ColourLoadOperation
		{
			Load = 0, Clear = 1
		};

		enum class ImageLayout
		{
			Undefined = 0, Presentation = 1000001002
		};

	public:
		ColourLoadOperation ColourLoadOp = ColourLoadOperation::Clear;

		ImageLayout PreviousImageLayout = ImageLayout::Undefined;
		ImageLayout FinalImageLayout = ImageLayout::Presentation;
	};

	class RenderPass
	{
	public:
		RenderPass() = default;
		virtual ~RenderPass() = default;

		virtual void Begin() = 0;
		virtual void End() = 0;
		virtual void Submit() = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;

		virtual Ref<RenderCommandBuffer> GetCommandBuffer() = 0;

		static Ref<RenderPass> Create(RenderPassSpecification specs = RenderPassSpecification());
		static Ref<RenderPass> CreateFromCommandBuffer(RenderPassSpecification specs, Ref<RenderCommandBuffer> commandBuffer);
	};

}