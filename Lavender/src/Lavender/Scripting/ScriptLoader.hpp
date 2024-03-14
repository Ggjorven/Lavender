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
		ScriptLoader() = default;
		virtual ~ScriptLoader() = default;

		virtual void Reload() = 0;

		virtual void Detach() = 0;
		virtual bool IsDetached() = 0;

		static Ref<ScriptLoader> Create(const std::filesystem::path& filepath);
	};

}