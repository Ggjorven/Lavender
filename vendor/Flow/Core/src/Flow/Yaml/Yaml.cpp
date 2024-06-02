#include "flpch.h"
#include "Yaml.hpp"

namespace Flow::Yaml
{

	File::File(const std::filesystem::path& file, FileMode mode)
		: m_Path(file), m_Mode(mode)
	{
		switch (m_Mode)
		{
		case FileMode::Read:
			StartRead();
			break;
		case FileMode::Write:
			StartWrite();
			break;

		default:
			// TODO: Logging
			break;
		}
	}

	File::~File()
	{
		switch (m_Mode)
		{
		case FileMode::Read:
			EndRead();
			break;
		case FileMode::Write:
			EndWrite();
			break;

		default:
			// TODO: Logging
			break;
		}
	}

	void File::StartRead()
	{
		try { m_Read = YAML::LoadFile(m_Path.string()); }
		catch (YAML::BadFile e)
		{
			// TODO: Logging
			return;
		}
	}

	void File::StartWrite()
	{
		// Empty ...
	}

	void File::EndRead()
	{
		// Empty ...
	}

	void File::EndWrite()
	{
		std::ofstream fileStream = std::ofstream(m_Path);

		if (!fileStream.good() || !fileStream.is_open())
		{
			// TODO: Logging
			return;
		}

		fileStream << m_Write.c_str();
		fileStream.close();
	}

}