#pragma once

#include <filesystem>

#include "Lavender/Utils/Utils.hpp"
#include "Lavender/Utils/UUID.hpp"

#include "Lavender/FileSystem/YAMLUtils.hpp"

#include "Lavender/Workspace/Scene.hpp"

namespace Lavender
{

	class SceneSerializer
	{
	public:
		SceneSerializer(Ref<Scene> scene);
		virtual ~SceneSerializer();

		void Serialize();
		void Deserialize(const std::filesystem::path& path);

	private:
		void SerializeEntity(YAML::Emitter& emitter, const UUID& uuid);
		void DeserializeEntity(YAML::detail::iterator_value& node);

	private:
		Ref<Scene> m_Scene = nullptr;
	};

}