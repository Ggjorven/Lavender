#include "lvpch.h"
#include "RegistryInterface.hpp"

#include "Lavender/Core/Logging.hpp"

namespace Lavender
{

	RegistryInterface* RegistryInterface::s_Instance = nullptr;

	RegistryInterface::RegistryInterface(Ref<RegistryCollection> collection, Ref<ScriptLoader> loader)
		#ifdef LV_PLATFORM_WINDOWS
		: m_Interface(RefHelper::Create<WindowsRegistryInterface>(collection, loader))
		#endif
	{
		s_Instance = this;

		m_Interface->Initialize();
	}

	RegistryInterface::~RegistryInterface()
	{
	}

	Ref<RegistryInterface> RegistryInterface::Create(Ref<RegistryCollection> collection, Ref<ScriptLoader> loader)
	{
		return RefHelper::Create<RegistryInterface>(collection, loader);
	}

}