#pragma once

#include <vector>

#include <Lavender/Core/Layer.hpp>

using namespace Lavender;

class TestLayer : public Layer
{
public:
	void OnAttach() override;
	void OnDetach() override;

	void OnUpdate(float deltaTime) override;
	void OnRender();
	void OnImGuiRender() override;

	void OnEvent(Event& e);
};