#pragma once

#include <Lavender/Core/Layer.hpp>

#include <Lavender/WorkSpace/Project.hpp>

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
	Ref<Project> m_Project = nullptr;
};