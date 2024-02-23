#pragma once

#include "Lavender/ImGui/BaseImGuiLayer.hpp"

namespace Lavender
{

	class VulkanImGuiLayer : public BaseImGuiLayer
	{
	public:
		VulkanImGuiLayer() = default;
		virtual ~VulkanImGuiLayer() = default;

		void OnAttach() override;
		void OnDetach() override;

		void Begin() override;
		void End() override;
	};

}