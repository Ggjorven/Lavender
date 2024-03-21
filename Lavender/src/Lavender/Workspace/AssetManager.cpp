#include "lvpch.h"
#include "AssetManager.hpp"

#include "Lavender/Core/Logging.hpp"

namespace Lavender
{

	AssetManager::AssetManager()
	{
	}

	AssetManager::~AssetManager()
	{
	}

	Ref<AssetManager> AssetManager::Create()
	{
		return RefHelper::Create<AssetManager>();
	}

}