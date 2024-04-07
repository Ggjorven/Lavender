#pragma once

#include "Lavender/Utils/Utils.hpp"

namespace Lavender
{

	struct UniformElement;
	class Pipeline;
	class DescriptorSet;

	// Note(Jorben): Needs to be created after the pipeline
	class UniformBuffer
	{
	public:
		UniformBuffer() = default;
		virtual ~UniformBuffer() = default;

		virtual void SetData(void* data, size_t size) = 0;

		virtual void Upload() = 0;
		virtual void Upload(Ref<DescriptorSet> set, UniformElement element) = 0;

		static Ref<UniformBuffer> Create(size_t dataSize);
		static Ref<UniformBuffer> Create(Ref<DescriptorSet> set, UniformElement element, size_t dataSize);
	};

	class DynamicUniformBuffer
	{
	public:
		DynamicUniformBuffer() = default;
		virtual ~DynamicUniformBuffer() = default;

		virtual void SetData(void* data, size_t size) = 0;

		virtual void SetDataIndexed(uint32_t index, void* data, size_t size) = 0;
		virtual void UploadIndexedData() = 0;

		virtual size_t GetAlignment() const = 0;

		virtual void Upload() = 0;
		virtual void Upload(Ref<DescriptorSet> set, UniformElement element) = 0;
		virtual void Upload(Ref<DescriptorSet> set, UniformElement element, size_t offset) = 0;

		static Ref<DynamicUniformBuffer> Create(uint32_t elements, size_t sizeOfOneElement);
		static Ref<DynamicUniformBuffer> Create(Ref<DescriptorSet> set, UniformElement element, uint32_t elements, size_t sizeOfOneElement);
	};

}