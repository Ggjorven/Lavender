#pragma once

#include <vector>

#include <Lavender/Core/Layer.hpp>
#include <Lavender/UI/UIPreferences.hpp>
#include <Lavender/Utils/Utils.hpp>

#include <Lavender/Renderer/RenderPass.hpp>
#include <Lavender/Renderer/Pipeline.hpp>
#include <Lavender/Renderer/VertexBuffer.hpp>
#include <Lavender/Renderer/IndexBuffer.hpp>
#include <Lavender/Renderer/UniformBuffer.hpp>
#include <Lavender/Renderer/Image.hpp>
#include <Lavender/Renderer/Viewport.hpp>
#include <Lavender/Renderer/Mesh.hpp>

#include <Lavender/Workspace/Project.hpp>

#include "Panels/ContentBrowserPanel.hpp"
#include "Panels/EntitiesPanel.hpp"
#include "Panels/MaterialPanel.hpp"
#include "Panels/DebugPanel.hpp"

#include "Panels/SettingsWindow.hpp"

using namespace Lavender;

class EditorLayer : public Layer
{
public:
	void OnAttach() override;
	void OnDetach() override;

	void OnUpdate(float deltaTime) override;
	void OnRender();
	void OnImGuiRender() override;

	void OnEvent(Event& e);

private:
	bool OnKeyPressEvent(KeyPressedEvent& e);
	bool OnResizeEvent(WindowResizeEvent& e);

	void RenderMenuBar();

private:
	Ref<UIPreferences> m_Preferences = nullptr;

	Ref<Project> m_Project = nullptr;

	Ref<ContentBrowserPanel> m_ContentBrowserPanel = nullptr;
	Ref<EntitiesPanel> m_EntityPanel = nullptr;
	Ref<MaterialPanel> m_MaterialPanel = nullptr;
	Ref<DebugPanel> m_DebugPanel = nullptr;

	Ref<SettingsWindow> m_SettingsWindow = nullptr;

	Ref<Image2D> m_Image = nullptr;

	Entity m_Entity = {};
};