#pragma once

#include "Lavender/Core/Core.hpp"
#include "Lavender/Utils/Utils.hpp"

#include "Lavender/WorkSpace/Scene.hpp"

#include <Flow/Flow.hpp>

namespace Lavender
{

	class SceneSerializer
	{
	public:
		SceneSerializer(Ref<Scene> scene);
		virtual ~SceneSerializer();

		void Serialize();
		void Deserialize(const std::filesystem::path& filepath);

	private:
		Ref<Scene> m_Scene = nullptr;

		friend class Scene;
	};

}