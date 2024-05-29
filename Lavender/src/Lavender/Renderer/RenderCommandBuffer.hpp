#pragma once

#include "Lavender/Utils/Utils.hpp"

namespace Lavender
{

	struct CommandBufferSpecification
	{
	public:
		enum class UsageFlags : uint8_t
		{
			None = 0, WaitForLatest = BIT(0), NoWaiting = BIT(1)
		};
	public:
		UsageFlags Flags = UsageFlags::WaitForLatest;
	};
	DEFINE_BITWISE_OPS(CommandBufferSpecification::UsageFlags)

	class RenderCommandBuffer
	{
	public:
		RenderCommandBuffer() = default;
		virtual ~RenderCommandBuffer() = default;

		virtual void Begin() = 0;
		virtual void End() = 0;
		virtual void Submit() = 0;

		static Ref<RenderCommandBuffer> Create(CommandBufferSpecification specs = {});
	};

}