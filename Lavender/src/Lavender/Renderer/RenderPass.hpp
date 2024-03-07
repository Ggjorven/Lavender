#pragma once

#include "Lavender/Utils/Utils.hpp"

namespace Lavender
{

	class RenderCommandBuffer;
	class Image2D;

	struct RenderPassSpecification // TODO: Add more
	{
	public:
		enum class ColourLoadOperation : uint8_t
		{
			Load = 0, Clear = 1
		};

		enum class ImageLayout : uint32_t
		{
			Undefined = 0, Colour = 2, ShaderRead = 5, Presentation = 1000001002
		};

		enum class Attachments : uint8_t // Note(Jorben): Colour is always included
		{
			None = 0, Depth = BIT(0)
		};

	public:
		Attachments UsedAttachments = Attachments::None;
		ColourLoadOperation ColourLoadOp = ColourLoadOperation::Clear;

		ImageLayout PreviousImageLayout = ImageLayout::Undefined;
		ImageLayout FinalImageLayout = ImageLayout::Presentation;
	};
	DEFINE_BITWISE_OPS(RenderPassSpecification::Attachments)

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