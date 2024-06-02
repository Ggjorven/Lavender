#include "ispch.h"
#include "Dll.hpp"

namespace Insight
{

#if defined(APP_PLATFORM_WINDOWS)
	Dll::Dll(const std::filesystem::path& path)
		: m_Path(path)
	{
		Reload(m_Path);
	}

	Dll::~Dll()
	{
		if (!m_ActiveClasses.empty())
		{
			// TODO: Implement logging

			// Create a copy of the map to avoid iterator invalidation
			auto activeClassesCopy = m_ActiveClasses;
			for (auto& [instance, name] : activeClassesCopy)
				DeleteClass(name, instance);
		}

		FreeLibrary(m_Handle);
	}

	void Dll::Reload()
	{
		Reload(m_Path);
	}

	void Dll::Reload(const std::filesystem::path& path)
	{
		if (!std::filesystem::exists(path))
		{
			// TODO: Implement logging
			return;
		}
		if (m_Handle)
		{
			FreeLibrary(m_Handle);
		}

		m_Handle = LoadLibraryA(m_Path.string().c_str());
		if (!m_Handle)
		{
			// TODO: Implement logging
			return;
		}

		// Load functions and set pointers
		std::string fnName = std::string("Insight_GetClasses");
		GetClassesFn getClassesFP = (GetClassesFn)GetProcAddress(m_Handle, fnName.c_str());
		m_Classes = *getClassesFP();
	}

	void* Dll::CreateClass(const std::string& className)
	{
		std::string fnName = std::string("Insight_Create") + className;
		CreateClassFn createClassFP = (CreateClassFn)GetProcAddress(m_Handle, fnName.c_str());

		void* cls = createClassFP();
		m_ActiveClasses[cls] = className;

		return cls;
	}

	void Dll::DeleteClass(const std::string& className, void* instance)
	{
		std::string fnName = std::string("Insight_Delete") + className;
		DeleteClassFn deleteClassFP = (DeleteClassFn)GetProcAddress(m_Handle, fnName.c_str());

		m_ActiveClasses.erase(instance);

		deleteClassFP(instance);
	}

	std::vector<OpaqueVariable> Dll::GetVariables(const std::string& className, void* classInstance)
	{
		std::vector<OpaqueVariable> vars = { };

		std::string fnName = std::string("Empty");
		for (auto& var : m_Classes.GetClasses()[className].Variables)
		{
			OpaqueVariable& newVar = vars.emplace_back();
			newVar.GetInstance() = classInstance;
			newVar.GetVariableInfo() = var;

			fnName = std::string("Insight_Get") + var.Name + className;
			newVar.GetGetter() = (GetDefaultFn)GetProcAddress(m_Handle, fnName.c_str());

			fnName = std::string("Insight_Set") + var.Name + className;
			newVar.GetSetter() = (SetDefaultFn)GetProcAddress(m_Handle, fnName.c_str());
		}

		return vars;
	}

#elif defined(APP_PLATFORM_LINUX)

	// TODO: ...

#endif
}