#pragma once

#include <filesystem>

#include "Flow/Core/Core.hpp"
#include "Flow/Yaml/YamlOptions.hpp"

namespace Flow::Yaml
{

	class File
	{
	public:
		File(const std::filesystem::path& file, FileMode mode);
		virtual ~File();

		template<typename T>
		File& operator << (const T& value);
		File& operator << (FileManip value);

		YAML::Node operator [] (const std::string& key);

		inline YAML::Emitter& GetEmitter() { return m_Write; }

	private:
		void StartRead();
		void StartWrite();

		void EndRead();
		void EndWrite();

	private:
		std::filesystem::path m_Path = {};
		FileMode m_Mode = FileMode::None;

		YAML::Emitter m_Write = {};
		YAML::Node m_Read = {};
	};



	template<typename T>
	inline File& File::operator << (const T& value)
	{
		m_Write << value;
		return *this;
	}

	inline File& File::operator << (FileManip value)
	{
		m_Write << (YAML::EMITTER_MANIP)value;
		return *this;
	}

	inline YAML::Node File::operator [] (const std::string& key)
	{
		return m_Read[key];
	}

}