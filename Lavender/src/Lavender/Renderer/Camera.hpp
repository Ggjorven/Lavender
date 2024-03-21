#pragma once

#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Renderer/Viewport.hpp"
#include "Lavender/Renderer/UniformBuffer.hpp"

#include <glm/glm.hpp>

namespace Lavender
{

	class Camera
	{
	public:
		glm::mat4 Model = {};
		glm::mat4 View = {};
		glm::mat4 Projection = {};

	};

	class EditorCamera : public Camera
	{
	public:
		EditorCamera(Ref<Viewport> viewport);
		virtual ~EditorCamera();

		void OnUpdate(float deltaTime);

		Camera& GetCamera() { return m_Camera; }
		inline Ref<UniformBuffer> GetBuffer() { return m_CameraUniform; }

		static Ref<EditorCamera> Create(Ref<Viewport> viewport);

	private:
		Ref<Viewport> m_Viewport = nullptr;
		Ref<UniformBuffer> m_CameraUniform = nullptr;

		Camera m_Camera = {};
	};

}