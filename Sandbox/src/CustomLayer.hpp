#pragma once

#include <memory>

#include <Lavender/Core/Layer.hpp>
#include <Lavender/Renderer/VertexBuffer.hpp>
#include <Lavender/Renderer/IndexBuffer.hpp>
#include <Lavender/Renderer/RenderController.hpp>
#include <Lavender/Renderer/UniformBuffer.hpp>
#include <Lavender/Renderer/Image.hpp>
#include <Lavender/Utils/Mesh.hpp>

using namespace Lavender;

class CustomLayer : public Layer
{
public:
	void OnAttach() override;
	void OnDetach() override;

	void OnUpdate(float deltaTime) override;
	void OnRender();
	void OnImGuiRender() override;

	void OnEvent(Event& e);

private:
	float m_Timer = 0.0f; // For FPS updates

	std::shared_ptr<RenderController> m_Controller = nullptr;
	std::shared_ptr<Mesh> m_Mesh = nullptr;

	std::shared_ptr<Image> m_Texture = nullptr;
	std::shared_ptr<UniformBuffer> m_CameraBuffer = nullptr;

};