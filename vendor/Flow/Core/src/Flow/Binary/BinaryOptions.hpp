#pragma once

#include <string>
#include <vector>
#include <fstream>

namespace Flow::Binary
{

	template<typename T>
	struct Convert
	{
	public:
		static void Write(std::ofstream& output, T data)
		{
			output.write(reinterpret_cast<const char*>(&data), sizeof(T));
		}

		static T Read(std::vector<char>& input, size_t offset = 0)
		{
			T result = {};
			memcpy(&result, &input[offset], sizeof(T));

			return result;
		}
	};

	template<>
	struct Convert<std::string>
	{
	public:
		static void Write(std::ofstream& output, std::string data)
		{
			int32_t size = (int32_t)data.size();
			output.write(reinterpret_cast<const char*>(&size), sizeof(int32_t));

			output.write(data.c_str(), size);
		}

		static std::string Read(std::vector<char>& input, size_t offset = 0)
		{
			if (offset + sizeof(int32_t) > input.size())
			{
				return {};
			}

			int32_t size = 0;
			memcpy(&size, &input[offset], sizeof(int32_t));
			offset += sizeof(int32_t);

			if (offset + size > input.size())
			{
				return {};
			}

			return std::string(input.begin() + offset, input.begin() + offset + size);
		}
	};

	// Used for getting the size of an object in binary files
	template<typename T>
	inline size_t SizeOf(T value = T())
	{
		return sizeof(T);
	}

	template<>
	inline size_t SizeOf<std::string>(std::string value)
	{
		return sizeof(int32_t) + value.size();
	}

}