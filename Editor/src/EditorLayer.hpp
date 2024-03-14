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

#include <Lavender/ECS/Registry.hpp>
#include <Lavender/ECS/Entity.hpp>
#include <Lavender/ECS/Components.hpp>

#include <Lavender/Scripting/ScriptLoader.hpp>
#include <Lavender/Scripting/EntityInterface.hpp>
#include <Lavender/Scripting/RegistryInterface.hpp>

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

private:
	Ref<UIPreferences> m_Preferences = nullptr;

	Ref<Viewport> m_Viewport = nullptr;

	Ref<Pipeline> m_Pipeline = nullptr;

	Ref<VertexBuffer> m_VertexBuffer = nullptr;
	Ref<IndexBuffer> m_IndexBuffer = nullptr;

	Ref<Image2D> m_Image = nullptr;
	Ref<UniformBuffer> m_CameraBuffer = nullptr;

	// Scripting
	Ref<RegistryCollection> m_Collection = nullptr;
	Entity m_Entity = {};

	Ref<ScriptLoader> m_Loader = nullptr;
	Ref<EntityInterface> m_EntityInterface = nullptr;
	Ref<RegistryInterface> m_RegistryInterface = nullptr;
};