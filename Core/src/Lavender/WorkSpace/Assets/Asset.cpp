#include "lvpch.h"
#include "Asset.hpp"

#include "Lavender/Core/Logging.hpp"

namespace Lavender
{

	Asset::Asset(const AssetData& data)
		: m_Handle(data.Handle), m_Path(data.Path)
	{
	}

}