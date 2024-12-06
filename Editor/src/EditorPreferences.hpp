#pragma once

#include "Lavender/Core/Core.hpp"
#include "Lavender/Utils/Utils.hpp"

#include "Lavender/WorkSpace/Project.hpp"

namespace Lavender::UI
{

	class PreferencesSerializer
	{
	public:
		PreferencesSerializer(const std::filesystem::path& path);
		virtual ~PreferencesSerializer() = default;

		void Serialize();
		void Deserialize();

	private:
		void Init(const std::string& iniData);
		std::string AsString();

	private:
		std::filesystem::path m_Path = {};
	};

}