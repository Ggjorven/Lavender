#pragma once

#include <filesystem>

#include "Lavender/Utils/Utils.hpp"

namespace Lavender
{

	struct UniformElement;
	class Pipeline;

	class Image2D
	{
	public:
		Image2D() = default;
		virtual ~Image2D() = default;

		virtual void SetData(void* data, size_t size) = 0;

		// TODO: Add resizing

		static Ref<Image2D> Create(Ref<Pipeline> pipeline, UniformElement element, uint32_t width, uint32_t height);
		static Ref<Image2D> Create(Ref<Pipeline> pipeline, UniformElement element, const std::filesystem::path& path);
		static Ref<Image2D> Create(uint32_t width, uint32_t height);
	};

}