#pragma once

#include "Lavender/Core/Core.hpp"
#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Renderer/RenderPass.hpp"

namespace Lavender::UI
{

	class Viewport
	{
	public:
		Viewport();
		virtual ~Viewport();

		void StartRecording();
		void EndRecording();

		void RenderUI();

		void Resize(uint32_t width, uint32_t height);

	private:
		Ref<Image2D> m_Attachment = nullptr;
		Ref<RenderPass> m_RenderPass = nullptr;
	};
}