#pragma once

#include "Lavender/Core/Core.hpp"
#include "Lavender/Utils/Utils.hpp"

#include <Flow/Yaml/Yaml.hpp>
#include <Flow/Binary/Binary.hpp>

namespace Lavender
{

	//////////////////////////////////////////////////////////////////////
	// Serialize helpers
	//////////////////////////////////////////////////////////////////////
	enum class SerializationType : uint8_t	{ None = 0, Yaml, Binary };
	enum class SerializationMode : uint8_t	{ None = 0, Deserialize, Serialize }; // Corresponds with Flow::FileMode

	template<SerializationType type = SerializationType::None>
	class File {};

	template<typename T>
	struct SerializableType
	{
	public:
		static void Serialize(File<SerializationType::Yaml>& file, const T& value) {}
		static void Serialize(File<SerializationType::Binary>& file, const T& value) {}
		static void Deserialize(File<SerializationType::Yaml>& file, T& result) {}
		static void Deserialize(File<SerializationType::Binary>& file, size_t& offset, T& result) {}
	};

	//////////////////////////////////////////////////////////////////////
	// File classes
	//////////////////////////////////////////////////////////////////////
	template<>
	class File<SerializationType::Yaml>
	{
	public:
		File(const std::filesystem::path& file, SerializationMode mode);

		template<typename T>
		inline void Serialize(const T& value) 
		{ 
			if (m_File.GetFileMode() != Flow::FileMode::Write)
			{
				APP_LOG_ERROR("Tried to serialize on a file which wasn't created to be serialized.");
				return;
			}

			SerializableType<T>::Serialize(*this, value);
		}
		template<typename T>
		inline void Deserialize(T& result) 
		{ 
			if (m_File.GetFileMode() != Flow::FileMode::Read)
			{
				APP_LOG_ERROR("Tried to deserialize on a file which wasn't created to be deserialized.");
				return;
			}

			SerializableType<T>::Deserialize(*this, result);
		}

		void Close();

		// Flow::Yaml Utils
		template<typename T>
		inline File& operator << (const T& value)
		{
			m_File << value;
			return *this;
		}
		inline File& operator << (Flow::Yaml::FileManip value)
		{
			m_File << value;
			return *this;
		}

		inline YAML::Node operator [] (const std::string& key) 
		{ 
			switch (m_Active)
			{
			case Active::Node:
				return m_CurrentNode[key];
			case Active::Iterator:
				return (*m_Iterator)[key];
			}

			return {};
		}

		enum class Active : uint8_t { Node = 0, Iterator };
		inline void SetActive(Active type) { m_Active = type; }

		inline void SetNode(const YAML::Node& node) { m_CurrentNode = node; m_Active = Active::Node; }
		inline void SetDetailIterator(YAML::detail::iterator_value& iterator) { m_Iterator = &iterator; m_Active = Active::Iterator; }

		inline YAML::Node& GetNode() { return m_CurrentNode; }
		inline YAML::detail::iterator_value& GetDetailIterator() { return *m_Iterator; }
		
		inline YAML::Emitter& GetEmitter() { return m_File.GetEmitter(); }

	private:
		Flow::Yaml::File m_File;

		Active m_Active = Active::Node;

		YAML::Node m_CurrentNode;
		YAML::detail::iterator_value* m_Iterator;
	};

	template<>
	class File<SerializationType::Binary> // TODO: Implement
	{
	public:
		File(const std::filesystem::path& file, SerializationMode mode);

	private:
		Flow::Binary::File m_File;
	};

}