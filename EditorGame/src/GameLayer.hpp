#pragma once

#include <Lavender/Core/Layer.hpp>

#include <Lavender/WorkSpace/Project.hpp>

using namespace Lavender;

class GameLayer : public Layer
{
public:
	void OnAttach() override;
	void OnDetach() override;

	void OnUpdate(float deltaTime) override;
	void OnRender() override;
	void OnEvent(Event& e) override;
private:
	Ref<Project> m_Project = nullptr;
};