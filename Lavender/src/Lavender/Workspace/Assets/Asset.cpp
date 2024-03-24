#include "lvpch.h"
#include "Asset.hpp"

#include "Lavender/Core/Logging.hpp"

namespace Lavender
{

	Asset::Asset()
		: m_Handle(UUID::Create())
	{
	}

}
