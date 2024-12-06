#include "lvpch.h"
#include "SharedResources.hpp"

#include "Lavender/Core/Logging.hpp"

namespace Lavender
{

	void Shared::Init()
	{
		ImageSpecification white = ImageSpecification(1, 1, ImageUsageFlags::Colour | ImageUsageFlags::NoMipMaps | ImageUsageFlags::Sampled);
		WhiteTexture = Image2D::Create(white);
		uint32_t col = 0xFFFFFFFF;
		WhiteTexture->SetData((void*)&col, sizeof(uint32_t));
	}

	void Shared::Destroy()
	{
		WhiteTexture.reset();
		WhiteTexture = nullptr;
	}

}