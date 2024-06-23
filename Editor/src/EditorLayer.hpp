#pragma once

#include <Lavender/Core/Layer.hpp>

#include <Lavender/WorkSpace/Project.hpp>

#include <Lavender/UI/Style.hpp>

#include "Panels/UIEditor.hpp"
#include "Panels/Viewport.hpp"

using namespace Lavender;

class EditorLayer : public Layer
{
public:
	void OnAttach() override;
	void OnDetach() override;

	void OnUpdate(float deltaTime) override;
	void OnRender() override;
	void OnEvent(Event& e) override;
	void OnUIRender() override;

private:
	void LoadProject();
	void InitStyles();

	void CreateProject(const std::string& name, const std::filesystem::path& directory);

private:
	Ref<Project> m_Project = nullptr;

	UI::Editor m_Editor = {};

	UI::StyleList m_GlobalUIStyles = {};
	UI::StyleList m_GlobalUIColours = {};
	UI::Viewport m_Viewport = {};
};