#include "lvpch.h"
#include "SceneSerializer.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/ECS/Components.hpp"

#include <fstream>

namespace Lavender
{
	SceneSerializer::SceneSerializer(Ref<Scene> scene)
		: m_Scene(scene)
	{
	}

	SceneSerializer::~SceneSerializer()
	{
	}

	void SceneSerializer::Serialize()
	{
		YAML::Emitter data = {};

		data << YAML::BeginMap;
		data << YAML::Key << "Scene";
		data << YAML::Value << m_Scene->m_UUID.Get();

		data << YAML::Key << "SceneData";
		data << YAML::Value << YAML::BeginMap;

		{
			data << YAML::Key << "Entities";
			data << YAML::Value << YAML::BeginSeq;

			for (auto& entity : m_Scene->m_Collection->GetMainRegistry()->GetEntityMap())
				SerializeEntity(data, entity.first);

			data << YAML::EndSeq;
		}

		data << YAML::EndMap;

		data << YAML::EndMap;

		std::ofstream fileStream(m_Scene->m_Path);
		if (!fileStream.good() || !fileStream.is_open())
		{
			LV_LOG_ERROR("Failed to open file {0}", m_Scene->m_Path);
			return;
		}

		fileStream << data.c_str();
		fileStream.close();
	}

	void SceneSerializer::Deserialize(const std::filesystem::path& path)
	{
		m_Scene->m_Path = path;

		YAML::Node data = {};
		try
		{
			data = YAML::LoadFile(path.string());
		}
		catch (YAML::BadFile e)
		{
			LV_LOG_WARN("Failed to load {0} (Code: {1})\n\tLoading empty preferences.", path.string(), e.what());
			return;
		}

		auto scene = data["Scene"];
		if (scene)
			m_Scene->m_UUID = UUID(scene.as<uint64_t>());

		auto sceneData = data["SceneData"];
		if (sceneData)
		{
			auto entities = sceneData["Entities"];
			if (entities)
			{
				for (auto entity : entities)
					DeserializeEntity(entity);
			}
		}
	}

	void SceneSerializer::SerializeEntity(YAML::Emitter& emitter, const UUID& uuid)
	{
		Entity entity = Entity(m_Scene->GetCollection(), uuid);

		emitter << YAML::BeginMap;
		emitter << YAML::Key << "Entity";
		emitter << YAML::Value << uuid.Get();

		if (entity.HasComponent<TagComponent>())
		{
			TagComponent& tagComponent = entity.GetComponent<TagComponent>();
			emitter << YAML::Key << "TagComponent";
			emitter << YAML::BeginMap;

			emitter << YAML::Key << "Tag" << YAML::Value << tagComponent.Tag;

			emitter << YAML::EndMap;
		}

		if (entity.HasComponent<TransformComponent>())
		{
			TransformComponent& transform = entity.GetComponent<TransformComponent>();
			emitter << YAML::Key << "TransformComponent";
			emitter << YAML::BeginMap;

			emitter << YAML::Key << "Position" << transform.Position;
			emitter << YAML::Key << "Size" << YAML::Value << transform.Size;
			emitter << YAML::Key << "Rotation" << YAML::Value << transform.Rotation;

			emitter << YAML::EndMap;
		}

		if (entity.HasComponent<MeshComponent>())
		{
			MeshComponent& mesh = entity.GetComponent<MeshComponent>();
			emitter << YAML::Key << "MeshComponent";
			emitter << YAML::BeginMap;

			if (mesh.MeshObject) emitter << YAML::Key << "AssetHandle" << mesh.MeshObject->GetHandle().Get();
			if (mesh.Material) emitter << YAML::Key << "Material" << YAML::Value << mesh.Material->GetHandle().Get();

			emitter << YAML::EndMap;
		}

		emitter << YAML::EndMap;
	}

	void SceneSerializer::DeserializeEntity(YAML::detail::iterator_value& node)
	{
		UUID uuid = node["Entity"].as<uint64_t>();
		Entity entity = m_Scene->CreateEntityWithUUID(uuid);

		//TagComponent
		auto tagComponent = node["TagComponent"];
		if (tagComponent)
		{
			TagComponent& tag = entity.AddOrReplaceComponent<TagComponent>();
			tag.Tag = tagComponent["Tag"].as<std::string>();
		}

		//TransformComponent
		auto transformComponent = node["TransformComponent"];
		if (transformComponent)
		{
			TransformComponent& transform = entity.AddOrReplaceComponent<TransformComponent>();
			transform.Position = transformComponent["Position"].as<glm::vec3>();
			transform.Size = transformComponent["Size"].as<glm::vec3>();
			transform.Rotation = transformComponent["Rotation"].as<glm::vec3>();
		}

		//MeshComponent
		auto meshComponent = node["MeshComponent"];
		if (meshComponent)
		{
			MeshComponent& mesh = entity.AddOrReplaceComponent<MeshComponent>();
			auto meshHandle = meshComponent["AssetHandle"];
			if (meshHandle) mesh.MeshObject = RefHelper::RefAs<MeshAsset>(m_Scene->GetAssetManager()->GetAsset(meshHandle.as<uint64_t>()));
			auto materialHandle = meshComponent["Material"];
			if (materialHandle) mesh.Material = RefHelper::RefAs<MaterialAsset>(m_Scene->GetAssetManager()->GetAsset(materialHandle.as<uint64_t>()));
		}
	}

}
