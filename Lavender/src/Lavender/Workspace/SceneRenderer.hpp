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

		// TODO: Add some kind of custom/runtime camera as well
		void Render(Ref<EditorCamera>& camera, Ref<RenderCommandBuffer> cmdBuffer);

	private:
		Scene* m_Scene;
	};

}