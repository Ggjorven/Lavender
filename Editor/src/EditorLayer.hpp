#pragma once

#include <Lavender/Core/Layer.hpp>
#include <Lavender/Utils/Utils.hpp>
#include <Lavender/Renderer/RenderPass.hpp>

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
	bool OnResizeEvent(WindowResizeEvent& e);

private:
	Ref<RenderPass> m_RenderPass = nullptr;
};