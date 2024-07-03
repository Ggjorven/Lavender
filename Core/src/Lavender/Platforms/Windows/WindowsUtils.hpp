#pragma once

#include "Lavender/Utils/Utils.hpp"

namespace Lavender::Utils
{
	
	class WindowsToolKit : public ToolKit
	{
	public:
		WindowsToolKit();
		virtual ~WindowsToolKit() = default;

	private:
		std::filesystem::path OpenFileImpl(const std::string& filter, const std::filesystem::path& dir) const override;
		std::filesystem::path SaveFileImpl(const std::string& filter, const std::filesystem::path& dir) const override;

		std::filesystem::path OpenDirectoryImpl(const std::string& dir) const override;

		double GetTimeImpl() const override;
		size_t GetMemoryUsageImpl() const override;
		size_t GetHeapMemoryUsageImpl() const override;
	};

}