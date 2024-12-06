#pragma once

#include <Lavender/Core/Layer.hpp>

#include <Lavender/WorkSpace/Project.hpp>

#include <Lavender/UI/Style.hpp>

#include "Panels/UIEditor.hpp"

#include "Panels/Viewport.hpp"
#include "Panels/Entities.hpp"
#include "Panels/Components.hpp"
#include "Panels/Debug.hpp"

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
	void LoadProject(const std::filesystem::path& file = "Editor/Resources/Projects/Example/Example.lvproj");
	void InitStyles();
	void InitUI();

	void MenuBar();

	void CreateProject(const std::string& name, const std::filesystem::path& directory);

	bool OnKeyPress(KeyPressedEvent& e);

private:
	Ref<Project> m_Project = nullptr;

	UI::Editor m_Editor = {}; // TODO: Remove

	UI::StyleList m_GlobalUIStyles = {};
	UI::StyleList m_GlobalUIColours = {};

	Ref<UI::Viewport> m_Viewport = {};
	Ref<UI::Entities> m_Entities = {};
	Ref<UI::Components> m_Components = {};
	Ref<UI::Debug> m_Debug = {};
};