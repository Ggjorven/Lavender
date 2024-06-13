#pragma once

#include <memory>
#include <filesystem>
#include <unordered_map>

#include <Insight/Core/Class.hpp>
#include <Insight/Core/Fields.hpp>

#include <Insight/Utils/Variables.hpp>
#include <Insight/Utils/Functions.hpp>

#if defined(APP_PLATFORM_WINDOWS)
#include <Windows.h>
#elif defined(APP_PLATFORM_LINUX)
// TODO: ...
#endif

namespace Insight
{

	typedef Internal::Classes* (*GetClassesFn)();
	typedef void* (*CreateClassFn)();
	typedef void (*DeleteClassFn)(void* instance);

	struct ClassCache
	{
	public:
		struct ClassInfo
		{
		public:
			CreateClassFn Create = nullptr;
			DeleteClassFn Delete = nullptr;
		};
	public:
		std::unordered_map<std::string, ClassCache::ClassInfo> Info = { };

	public:
		ClassCache() = default;
		virtual ~ClassCache() = default;

		inline bool Exists(const std::string& name) { return (Info.find(name) != Info.end()); }
	};

#if defined(APP_PLATFORM_WINDOWS)
	class Dll
	{
	public:
		Dll() = default;
		Dll(const std::filesystem::path& path);
		virtual ~Dll();

		void Reload();
		void Reload(const std::filesystem::path& path);

		void* CreateClass(const std::string& className);
		void DeleteClass(const std::string& className, void* instance);

		inline bool Loaded() const { return m_Handle != nullptr; }
		inline std::filesystem::path GetPath() { return m_Path; }
		inline Internal::Classes& GetClasses() { return m_Classes; }

		std::vector<OpaqueVariable> GetVariables(const std::string& className, void* classInstance);
		
		inline HMODULE GetHandle() { return m_Handle; }

		template <typename TFunc>
		inline TFunc GetCustomFunction(const std::string& cFuncName)
		{
			return (TFunc)GetProcAddress(m_Handle, cFuncName.c_str());
		}

		static std::shared_ptr<Dll> Create(const std::filesystem::path& path);

	private:
		HMODULE m_Handle = {};

		std::filesystem::path m_Path = {};

		ClassCache m_Cache = {};
		Internal::Classes m_Classes = {};
		std::unordered_map<void*, std::string> m_ActiveClasses = { };
	};

#elif defined(APP_PLATFORM_LINUX)

// TODO: ...

#endif

}