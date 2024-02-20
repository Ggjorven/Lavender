#pragma once

#include <memory>
#include <filesystem>

#include "Lavender/UI/UIPreferences.hpp"

namespace Lavender
{

	class PreferencesSerializer
	{
	public:
		PreferencesSerializer(std::shared_ptr<UIPreferences>& preferences);
		virtual ~PreferencesSerializer();

		void Serialize(const std::filesystem::path& file);
		void Deserialize(const std::filesystem::path& file);

	private:
		std::shared_ptr<UIPreferences> m_Preferences = nullptr;
	};

}