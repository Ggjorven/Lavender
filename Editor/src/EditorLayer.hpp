#pragma once

#include <vector>

#include <Lavender/Core/Layer.hpp>
#include <Lavender/Utils/Utils.hpp>

#include <Lavender/Renderer/RenderPass.hpp>
#include <Lavender/Renderer/Pipeline.hpp>
#include <Lavender/Renderer/VertexBuffer.hpp>
#include <Lavender/Renderer/IndexBuffer.hpp>
#include <Lavender/Renderer/UniformBuffer.hpp>
#include <Lavender/Renderer/Image.hpp>

#include <imgui.h>

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
	Ref<Pipeline> m_Pipeline = nullptr;
	Ref<RenderPass> m_RenderPass = nullptr;

	Ref<VertexBuffer> m_VertexBuffer = nullptr;
	Ref<IndexBuffer> m_IndexBuffer = nullptr;

	Ref<Image2D> m_Image = nullptr;

	std::vector<ImTextureID> m_ImGuiTextures = { };
};