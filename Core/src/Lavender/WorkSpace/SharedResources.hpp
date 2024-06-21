#pragma once

#include "Lavender/Core/Core.hpp"
#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Renderer/Image.hpp"

namespace Lavender
{

	class Shared
	{
	public:
		static void Init();
		static void Destroy();

	public:
		inline static Ref<Image2D> WhiteTexture = nullptr;
	};

}