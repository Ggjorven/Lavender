#pragma once

#include "Lavender/Utils/Utils.hpp"

namespace Lavender
{

	class AssetManager // TODO
	{
	public:
		AssetManager();
		virtual ~AssetManager();

		static Ref<AssetManager> Create();
	};

}