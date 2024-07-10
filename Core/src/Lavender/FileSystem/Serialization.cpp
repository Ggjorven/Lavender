#include "lvpch.h"
#include "Serialization.hpp"

#include "Lavender/Core/Logging.hpp"

namespace Lavender
{

	///////////////////////////////////////////////////
	// Yaml
	///////////////////////////////////////////////////
	File<SerializationType::Yaml>::File(const std::filesystem::path& file, SerializationMode mode)
		: m_File(file, (Flow::FileMode)mode), m_CurrentNode(m_File.GetNode())
	{
		m_File << Flow::Yaml::FileManip::BeginMap;
	}

	void File<SerializationType::Yaml>::Close()
	{
		m_File << Flow::Yaml::FileManip::EndMap; 
		m_File.Close();
	}

	///////////////////////////////////////////////////
	// Binary
	///////////////////////////////////////////////////
	File<SerializationType::Binary>::File(const std::filesystem::path& file, SerializationMode mode)
		: m_File(file, (Flow::FileMode)mode)
	{
	}

}