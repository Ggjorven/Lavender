#pragma once

#include <memory>

#include <glm/glm.hpp>

namespace Lavender
{

	struct APISpecifications
	{
	public:
		enum class MSAASamples
		{
			None = 0, MSAA2 = 2, MSAA4 = 4, MSAA8 = 8, MSAA16 = 16, MSAA32 = 32
		};

		enum class ClearValues
		{
			Colour = 0x00004000,
			Depth = 0x00000100,
			Stencil = 0x8224
		};

		enum class ColourSpace
		{
			Unorm = 44,
			sRGB = 50
		};

	public:
		glm::vec4 ClearColour = { 0.0f, 0.0f, 0.0f, 0.0f };
		MSAASamples MSAA = MSAASamples::None;
		ClearValues ClearValues = ClearValues::Colour;
		ColourSpace ColourSpace = ColourSpace::Unorm;
	};

}