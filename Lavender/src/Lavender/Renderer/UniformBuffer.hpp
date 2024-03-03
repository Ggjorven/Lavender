#pragma once

#include "Lavender/Utils/Utils.hpp"

namespace Lavender
{

	struct UniformElement;
	class Pipeline;

	// Note(Jorben): Needs to be created after the pipeline
	class UniformBuffer
	{
	public:
		UniformBuffer() = default;
		virtual ~UniformBuffer() = default;

		virtual void SetData(void* data, size_t size) = 0;

		static Ref<UniformBuffer> Create(Ref<Pipeline> pipeline, UniformElement element, size_t dataSize);
	};

}