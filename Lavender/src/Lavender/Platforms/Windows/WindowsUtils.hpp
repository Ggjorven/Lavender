#pragma once

#include "Lavender/Utils/Utils.hpp"

namespace Lavender::Utils
{
	
	class WindowsToolKit : public ToolKit
	{
	public:
		WindowsToolKit() = default;

	private:
		float GetTimeImpl() const override;
	};

}