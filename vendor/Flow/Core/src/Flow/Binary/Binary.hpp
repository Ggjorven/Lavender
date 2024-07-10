#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <filesystem>

#include "Flow/Core/Core.hpp"
#include "Flow/Binary/BinaryOptions.hpp"

namespace Flow::Binary
{

	class File
	{
	public:
		File(const std::filesystem::path& file, FileMode mode);
		virtual ~File();

		// Writing
		template<typename T>
		inline void Write(T data)
		{
			Type<T>::Write(m_Write, data);
		}

		template<typename T>
		File& operator << (T data)
		{
			Write<T>(data);
			return *this;
		}

		// Reading
		template<typename T>
		inline T Read(size_t offset = 0)
		{
			return Type<T>::Read(m_Read, offset);
		}

	private:
		void StartRead();
		void StartWrite();

		void EndRead();
		void EndWrite();

	private:
		std::filesystem::path m_Path = {};
		FileMode m_Mode = FileMode::None;

		std::vector<char> m_Read = { };
		std::ofstream m_Write = {};
	};

}