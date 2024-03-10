#pragma once

#include "Lavender/Utils/Utils.hpp"

namespace Lavender
{

	class RenderCommandBuffer;

	class VertexBuffer
	{
	public:
		VertexBuffer() = default;
		virtual ~VertexBuffer() = default;

		virtual void Bind(Ref<RenderCommandBuffer> commandBuffer) = 0;

		static Ref<VertexBuffer> Create(void* data, size_t size);
	};

}