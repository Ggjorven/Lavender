#pragma once

#include "Lavender/Core/Core.hpp"
#include "Lavender/Utils/Utils.hpp"

#include "Lavender/WorkSpace/Rendering/FrameResources.hpp"

#include "Lavender/ECS/Camera.hpp"
#include "Lavender/ECS/Components.hpp"

#include <glm/glm.hpp>

namespace Lavender
{

	class Scene;

	class SceneRenderer
	{
	public:
		SceneRenderer(Scene* scene);
		virtual ~SceneRenderer();

		void Render(Ref<Camera> view);

		void OnEvent(Event& e);
		void Resize(uint32_t width, uint32_t height);

		inline FrameResources& GetResources() { return m_Resources; }
		
		static Ref<SceneRenderer> Create(Scene* scene);

	private:
		void UpdateCamera(Ref<Camera> view);
		void UpdateModels();
		void UpdateLights();
		void UpdateSceneData();

		void DepthPrePass();
		void LightCulling();
		void FinalShading();

		glm::uvec2 GetTileCount();
		glm::mat4 CalculateModelMatrix(const TransformComponent& component);

		bool OnResize(WindowResizeEvent& e);

	private:
		FrameResources m_Resources = {};
		Scene* m_Scene = nullptr;
	};

}