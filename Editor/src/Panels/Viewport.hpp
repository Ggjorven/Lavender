#pragma once

#include <Lavender/Core/Core.hpp>
#include <Lavender/Utils/Utils.hpp>

#include <Lavender/Renderer/Image.hpp>

#include <Lavender/UI/UI.hpp>
#include <Lavender/UI/Style.hpp>

#include <ImGuizmo.h>

#include "Panels/Entities.hpp"

namespace Lavender::UI
{

	class Viewport
	{
	public:
		Viewport(Ref<Entities> entities);
		virtual ~Viewport();

		void RenderUI();

		void Resize(uint32_t width, uint32_t height);

		static Ref<Viewport> Create(Ref<Entities> entities);

	private:
		void InitStyles();
		void InitResources();

		void RenderTopUI();

	private:
		Ref<Entities> m_EntitiesRef = nullptr;

		UI::StyleList m_Styles = {};
		UI::StyleList m_Colours = { };

		glm::vec2 m_TopLeftCursorPos = {};

		// ImGuizmo state
		ImGuizmo::MODE m_Mode = ImGuizmo::MODE::LOCAL;
		ImGuizmo::OPERATION m_Operation = ImGuizmo::OPERATION::TRANSLATE;

		// Resources
		Ref<Image2D> m_PlayButton = nullptr;
		Ref<Image2D> m_StopButton = nullptr;

		Ref<Image2D> m_MoveButton = nullptr;
		Ref<Image2D> m_ScaleButton = nullptr;
		Ref<Image2D> m_RotateButton = nullptr;
	};

}