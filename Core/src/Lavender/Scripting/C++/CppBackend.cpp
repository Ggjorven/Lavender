#include "lvpch.h"
#include "CppBackend.hpp"

#include "Lavender/Core/Logging.hpp"
#include "Lavender/Utils/UUID.hpp"

#include "Lavender/ECS/Components.hpp"

#include "Lavender/Scripting/C++/CppFunctions.hpp"

namespace Lavender
{

	ScriptEntityInfo::ScriptEntityInfo(const std::string& name, ScriptableEntity* instance)
		: Name(name), Instance(instance)
	{
	}



	CppBackend::CppBackend(const ScriptingSpecification& specs)
		: m_Specification(specs)
	{
		Reload();
		APP_LOG_TRACE("Initialized C++ script: {0}", specs.Path.string());
	}

	CppBackend::~CppBackend()
	{
		for (auto& [uuid, info] : m_Instances)
			m_Dll->DeleteClass(info.Name, info.Instance);
	}

	void CppBackend::Reload()
	{
		if (!m_Dll)
		{
			// TODO: Load a copied dll and move the PDB
			m_Dll = Insight::Dll::Create(m_Specification.Path);
		}
		else
			m_Dll->Reload();

		CppFunctions::OutSource(m_Dll);
	}

	void CppBackend::AddInstance(const std::string& classname, const UUID& entity)
	{
		/*
		if (m_Cache.Exists(classname))
		{
			m_Instances[entity] = ScriptEntityInfo(classname, (ScriptableEntity*)m_Dll->CreateClass(classname));
		}
		*/

		m_Instances[entity] = ScriptEntityInfo(classname, (ScriptableEntity*)m_Dll->CreateClass(classname));
	}

	void CppBackend::RemoveInstance(const std::string& classname, const UUID& entity)
	{
		m_Dll->DeleteClass(classname, m_Instances[entity].Instance);
	}

}