#include "lvpch.h"
#include "CppBackend.hpp"

#include "Lavender/Core/Logging.hpp"
#include "Lavender/Utils/UUID.hpp"

#include "Lavender/ECS/Components.hpp"

#include "Lavender/Scripting/C++/CppFunctions.hpp"

namespace Lavender
{

	ScriptEntityInfo::ScriptEntityInfo(const std::string& name, Script::Entity* instance)
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
			CopyOver();
			m_Dll = Insight::Dll::Create(m_CopyPath / m_Specification.Path.filename());
		}
		else
			m_Dll->Reload();

		m_Cache.OnCreate = m_Dll->GetCustomFunction<Script::OnCreateFn>("Lavender_ScriptEntityOnCreate");
		m_Cache.OnUpdate = m_Dll->GetCustomFunction<Script::OnUpdateFn>("Lavender_ScriptEntityOnUpdate");
		m_Cache.GetUUID = m_Dll->GetCustomFunction<Script::GetUUIDFn>("Lavender_ScriptEntityGetUUID");

		CppFunctions::OutSource(m_Dll);
	}

	void CppBackend::OnCreateAll()
	{
		for (auto& [uuid, info] : m_Instances)
			m_Cache.OnCreate(info.Instance);
	}

	void CppBackend::OnUpdateAll(float deltaTime)
	{
		for (auto& [uuid, info] : m_Instances)
			m_Cache.OnUpdate(info.Instance, deltaTime);
	}

	void CppBackend::AddInstance(const std::string& classname, const UUID& entity)
	{
		m_Instances[entity] = ScriptEntityInfo(classname, (Script::Entity*)m_Dll->CreateClass(classname));
		
		UUID& uuid = *m_Cache.GetUUID(m_Instances[entity].Instance);
		uuid = entity;
	}

	void CppBackend::RemoveInstance(const std::string& classname, const UUID& entity)
	{
		m_Dll->DeleteClass(classname, m_Instances[entity].Instance);
	}

	std::vector<std::string> CppBackend::GetClasses()
	{
		std::vector<std::string> cls(m_Dll->GetClasses().GetClasses().size()); 

		size_t i = 0;
		for (auto& [name, info] : m_Dll->GetClasses().GetClasses())
		{
			cls[i] = name;
			i++;
		}

		return cls;
	}

	void CppBackend::CopyOver()
	{
		// Note(Jorben): We have so many copies of paths since most std::filesystem functions not only return the outcome but apply it to the parameter/variable
		std::filesystem::path oldPath = m_Specification.Path;
		oldPath = oldPath.remove_filename();

		std::filesystem::path newPath = m_Specification.Path;
		m_CopyPath = newPath.remove_filename();
		m_CopyPath = m_CopyPath / "Engine-Script";

		std::filesystem::create_directory(m_CopyPath);

		std::filesystem::path dllOld = m_Specification.Path;
		std::filesystem::path dllNew = m_CopyPath / m_Specification.Path.filename();

		// Note(Jorben): PDB is used for debugging.
		std::filesystem::path pdbOld = oldPath / m_Specification.Path.filename();
		pdbOld = pdbOld.replace_extension(".pdb");
		std::filesystem::path pdbNew = m_CopyPath / m_Specification.Path.filename();
		pdbNew = pdbNew.replace_extension(".pdb");

		std::filesystem::copy_file(m_Specification.Path, (m_CopyPath / m_Specification.Path.filename()), std::filesystem::copy_options::overwrite_existing);

		if (std::filesystem::exists(pdbOld))
			std::filesystem::copy_file(pdbOld, pdbNew, std::filesystem::copy_options::overwrite_existing);

		// Check if PDB file is present
		if (!std::filesystem::exists(pdbNew) && !std::filesystem::exists(pdbOld))
			APP_LOG_WARN("Failed to find PDB file for script ({0}), this is not critical. It only means debugging the script is not possible.", m_Specification.Path.string());

		// Note(Jorben): We delete the old one, since the debugger likes to use that one instead of the copied one and then we can't change the .pdb file while the app is running a.k.a we have to detach before we can compile our script.
		std::filesystem::remove(pdbOld);
	}

}