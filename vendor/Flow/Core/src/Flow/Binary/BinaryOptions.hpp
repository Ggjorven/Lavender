#pragma once

#include <string>
#include <vector>
#include <fstream>

namespace Flow::Binary
{

	template<typename T>
	struct Type
	{
	public:
		inline static void Write(std::ofstream& output, T data)
		{
			output.write(reinterpret_cast<const char*>(&data), sizeof(T));
		}

		inline static T Read(std::vector<char>& input, size_t offset = 0)
		{
			T result = {};
			memcpy(&result, &input[offset], sizeof(T));

			return result;
		}

		inline static size_t Size(T value = T())
		{
			return sizeof(T);
		}
	};

	template<>
	struct Type<std::string>
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

		inline static size_t Size(std::string value = std::string())
		{
			return sizeof(int32_t) + value.size();
		}
	};

}