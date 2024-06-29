#pragma once

#include <Lavender/Core/Core.hpp>
#include <Lavender/Utils/Utils.hpp>

#include <Lavender/Renderer/Image.hpp>

#include <Lavender/UI/UI.hpp>
#include <Lavender/UI/Style.hpp>

namespace Lavender::UI
{

	class Viewport
	{
	public:
		Viewport();
		virtual ~Viewport();

		void RenderUI();

		void Resize(uint32_t width, uint32_t height);

		static Ref<Viewport> Create();

	private:
		void InitStyles();
		void InitResources();

		void RenderTopUI();

	private:
		UI::StyleList m_Styles = {};
		UI::StyleList m_Colours = { };

		glm::vec2 m_TopLeftCursorPos = {};

		// Resources
		Ref<Image2D> m_PlayButton = nullptr;
		Ref<Image2D> m_StopButton = nullptr;
	};

}