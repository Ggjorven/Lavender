#include "flpch.h"
#include "Binary.hpp"

namespace Flow::Binary
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
		std::ifstream fileStream = std::ifstream(m_Path, std::ios::in | std::ios::binary);

		if (!fileStream.is_open() || !fileStream.good())
		{
			// TODO: Logging
			return;
		}

		fileStream.seekg(0, std::ios::end);
		size_t fileSize = fileStream.tellg();
		fileStream.seekg(0, std::ios::beg);

		m_Read = std::vector<char>(fileSize);
		fileStream.read(m_Read.data(), fileSize);

		if (fileStream.bad())
		{
			// TODO: Logging
			fileStream.close();
			return;
		}

		fileStream.close();
	}

	void File::StartWrite()
	{
		// Empty ...
		// Open the file in binary mode for writing
		m_Write = std::ofstream(m_Path, std::ios::out | std::ios::binary);

		if (!m_Write.is_open() || m_Write.good())
		{
			// TODO: Logging
			return;
		}
	}

	void File::EndRead()
	{
		// Empty ...
	}

	void File::EndWrite()
	{
		m_Write.close();
	}

}