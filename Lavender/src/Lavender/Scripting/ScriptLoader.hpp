#pragma once

#include <memory>
#include <filesystem>

#include "Lavender/Utils/Utils.hpp"

namespace Lavender
{

	class Input;

	typedef void (*SetInputInstanceFn)(Input*);
	typedef void (*RemoveInputInstanceFn)();

	typedef void (*InitLoggerFn)(std::shared_ptr<spdlog::logger>*);

	class ScriptLoader
	{
	public:
		// Inside bin/%Config%/
		inline static const std::filesystem::path DefaultPath = "Script\\script.dll";
	public:
		ScriptLoader() = default;
		virtual ~ScriptLoader() = default;

		virtual void Reload() = 0;

		virtual void Detach() = 0;
		virtual bool IsDetached() = 0;

		virtual std::filesystem::path GetPath() = 0;

		static Ref<ScriptLoader> Create(const std::filesystem::path& filepath);
	};

}