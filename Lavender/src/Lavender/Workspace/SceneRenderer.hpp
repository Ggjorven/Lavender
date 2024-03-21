#pragma once

#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Renderer/Camera.hpp"
#include "Lavender/Renderer/RenderCommandBuffer.hpp"

#include "Lavender/Workspace/Scene.hpp"

namespace Lavender
{

	class SceneRenderer
	{
	public:
		SceneRenderer() = default;
		SceneRenderer(Scene* scene);
		SceneRenderer(Ref<Scene> scene);
		virtual ~SceneRenderer();

		void Render(const Camera& camera, Ref<RenderCommandBuffer> cmdBuffer);

	private:
		Scene* m_Scene;
	};

}