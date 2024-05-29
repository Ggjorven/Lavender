#pragma once

#include <optional>
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

	struct ImageSpecification
	{
	public:
		// Empty is for creating your own one, File is for creating an image from a file
		enum class ImageUsage : uint8_t
		{
			None = 0, Size, File
		};
		enum class ImageUsageFlags : uint8_t
		{
			None = 0, Sampled = BIT(0), Storage = BIT(1), Colour = BIT(2), Depth = BIT(3), Transient = BIT(4), Input = BIT(5),
				NoMipMaps = BIT(6)
		};
		enum class ImageFormat : uint8_t
		{
			None = 0, RGBA, BGRA, sRGB
		};
	public:
		ImageUsage Usage = ImageUsage::None;
		ImageUsageFlags Flags = ImageUsageFlags::Sampled;
		ImageFormat Format = ImageFormat::RGBA;

		std::filesystem::path Path = {};

		uint32_t Width = 0;
		uint32_t Height = 0;

	public:
		ImageSpecification() = default;
		ImageSpecification(uint32_t width, uint32_t height, ImageUsageFlags flags = ImageUsageFlags::Sampled);
		ImageSpecification(const std::filesystem::path& path, ImageUsageFlags flags = ImageUsageFlags::Sampled);
		virtual ~ImageSpecification() = default;
	};
	DEFINE_BITWISE_OPS(ImageSpecification::ImageUsageFlags)

	class Image2D
	{
	public:
		Image2D() = default;
		virtual ~Image2D() = default;

		virtual void SetData(void* data, size_t size) = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;

		virtual void Upload() = 0;
		virtual void Upload(Ref<DescriptorSet> set, UniformElement element) = 0;

		virtual ImageSpecification& GetSpecification() = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		#ifndef LV_DISABLE_IMGUI
		virtual void* GetUIImage() = 0;
		#endif

		virtual Ref<Image2D> Copy() = 0;

		static Ref<Image2D> Create(const ImageSpecification& specs);
		static Ref<Image2D> Create(const ImageSpecification& specs, Ref<DescriptorSet> set, UniformElement element);
	};

}