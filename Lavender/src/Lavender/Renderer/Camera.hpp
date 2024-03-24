#pragma once

#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Renderer/Viewport.hpp"
#include "Lavender/Renderer/UniformBuffer.hpp"
#include "Lavender/Renderer/Pipeline.hpp"
#include "Lavender/Renderer/RenderCommandBuffer.hpp"

#include <glm/glm.hpp>

namespace Lavender
{

	class Camera
	{
	public:
		glm::mat4 View = {};
		glm::mat4 Projection = {};

	};

	class EditorCamera
	{
	public:
		EditorCamera(Ref<Viewport> viewport);
		virtual ~EditorCamera();

		void OnUpdate(float deltaTime);

		void UpdateAndUpload();
		void BindDescriptorSet(Ref<Pipeline> pipeline, Ref<RenderCommandBuffer> cmdBuffer);

		Camera& GetCamera() { return m_Camera; }
		inline Ref<UniformBuffer> GetBuffer() { return m_CameraUniform; }

		static Ref<EditorCamera> Create(Ref<Viewport> viewport);

	private:
		Ref<Viewport> m_Viewport = nullptr;
		Ref<UniformBuffer> m_CameraUniform = nullptr;

		Camera m_Camera = {};
	};

}