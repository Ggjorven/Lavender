#pragma once

#include <filesystem>

#include "Lavender/Utils/Utils.hpp"

// Also defined in lvpch.h
#if defined(LV_DIST)
	#define LV_DISABLE_IMGUI
#endif

namespace Lavender
{

	struct UniformElement;
	class Pipeline;
	class DescriptorSet;

	class Image2D
	{
	public:
		Image2D() = default;
		virtual ~Image2D() = default;

		virtual void SetData(void* data, size_t size) = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;

		virtual void Upload() = 0;
		virtual void Upload(Ref<DescriptorSet> set, UniformElement element) = 0;

		virtual std::filesystem::path GetPath() const = 0;

		#ifndef LV_DISABLE_IMGUI
		virtual void* GetUIImage() = 0;
		#endif

		virtual Ref<Image2D> Copy() = 0;

		static Ref<Image2D> Create(uint32_t width, uint32_t height);
		static Ref<Image2D> Create(const std::filesystem::path& path);
		static Ref<Image2D> Create(Ref<DescriptorSet> set, UniformElement element, uint32_t width, uint32_t height);
		static Ref<Image2D> Create(Ref<DescriptorSet> set, UniformElement element, const std::filesystem::path& path);
	};

}