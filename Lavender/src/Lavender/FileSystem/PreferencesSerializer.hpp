#pragma once

#include <memory>
#include <filesystem>

#include "Lavender/UI/UIPreferences.hpp"

#include "Lavender/Utils/Utils.hpp"

namespace Lavender
{

	class PreferencesSerializer
	{
	public:
		PreferencesSerializer(Ref<UIPreferences>& preferences);
		virtual ~PreferencesSerializer();

		void Serialize(const std::filesystem::path& file);
		void Deserialize(const std::filesystem::path& file);

	private:
		Ref<UIPreferences> m_Preferences = nullptr;
	};

}