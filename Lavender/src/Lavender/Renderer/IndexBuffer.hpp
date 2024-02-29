#pragma once

#include "Lavender/Utils/Utils.hpp"

namespace Lavender
{

	class RenderCommandBuffer;

	class IndexBuffer
	{
	public:
		IndexBuffer() = default;
		virtual ~IndexBuffer() = default;

		virtual void Bind(Ref<RenderCommandBuffer> commandBuffer) const = 0;

		virtual uint32_t GetCount() const = 0;

		static Ref<IndexBuffer> Create(uint32_t* indices, uint32_t count);
	};

}