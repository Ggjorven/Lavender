#include "lvpch.h"
#include "SceneSerializer.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/ECS/Components.hpp"

#include "Lavender/FileSystem/Serialization.hpp"
#include "Lavender/FileSystem/SerializationUtils.hpp"

#include "Lavender/WorkSpace/Project.hpp"

using namespace Flow;
using namespace Flow::Yaml;

//////////////////////////////////////////////////
// Utils
//////////////////////////////////////////////////
namespace Lavender
{

	template<typename TComponent>
	void SerializeComponent(File<SerializationType::Yaml>& file, const UUID& uuid, Entity& entity)
	{
		if (entity.HasComponent<TComponent>())
			file.Serialize(entity.GetComponent<TComponent>());
	}

	template<typename... TComponents>
	void SerializeEntity(Utils::TypeGroup<TComponents...> group, File<SerializationType::Yaml>& file, const UUID& uuid, Entity& entity)
	{
		// Note(Jorben): Empty function for when there are no components
	}

	template<typename FirstComponent, typename ... RestComponents>
	void SerializeEntity(Utils::TypeGroup<FirstComponent, RestComponents...> group, File<SerializationType::Yaml>& file, const UUID& uuid, Entity& entity)
	{
		SerializeComponent<FirstComponent>(file, uuid, entity);
		SerializeEntity<RestComponents...>(Utils::TypeGroup<RestComponents...>(), file, uuid, entity);
	}

	template<typename TComponent>
	void DeserializeComponent(File<SerializationType::Yaml>& file, const UUID& uuid, Entity& entity)
	{
		if (file[ComponentToString<TComponent>()])
		{
			TComponent& component = entity.AddOrReplaceComponent<TComponent>();
			file.Deserialize(component);

			if constexpr (std::is_same_v<TComponent, ScriptComponent>)
			{
				if (Project::Get()->GetScript())
					Project::Get()->GetScript()->AddInstance(component.Class, uuid);
			}
		}
	}

	template<typename... TComponents>
	void DeserializeEntity(Utils::TypeGroup<TComponents...> group, File<SerializationType::Yaml>& file, const UUID& uuid, Entity& entity)
	{
		// Note(Jorben): Empty function for when there are no components
	}

	template<typename FirstComponent, typename ... RestComponents>
	void DeserializeEntity(Utils::TypeGroup<FirstComponent, RestComponents...> group, File<SerializationType::Yaml>& file, const UUID& uuid, Entity& entity)
	{
		DeserializeComponent<FirstComponent>(file, uuid, entity);
		DeserializeEntity<RestComponents...>(Utils::TypeGroup<RestComponents...>(), file, uuid, entity);
	}

}
//////////////////////////////////////////////////
// Core Functions
//////////////////////////////////////////////////
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
		auto& info = m_Scene->m_Info;

		File<SerializationType::Yaml> file = File<SerializationType::Yaml>(Project::Get()->GetInfo().Directory / Project::Get()->GetInfo().Scenes / info.Path, SerializationMode::Serialize);

		file << FileManip::Key << "Scene";
		file << FileManip::Value << (uint64_t)m_Scene->m_ID;

		file << FileManip::Key << "Data";
		file << FileManip::Value << FileManip::BeginMap;

		// Entities
		file << FileManip::Key << "Entities";
		file << FileManip::Value << FileManip::BeginSeq;

		for (auto& [uuid, entity] : m_Scene->m_Registry.GetDict())
		{
			file << FileManip::BeginMap;
			file << FileManip::Key << "Entity";
			file << FileManip::Value << (uint64_t)uuid;

			SerializeEntity(AllComponents(), file, uuid, entity);

			file << FileManip::EndMap;
		}

		file << FileManip::EndSeq;

		file << FileManip::EndMap;
	}

	void SceneSerializer::Deserialize(const std::filesystem::path& filepath)
	{
		auto& info = m_Scene->m_Info;
		info.Path = std::filesystem::relative(filepath, Project::Get()->GetInfo().Directory / Project::Get()->GetInfo().Scenes);

		File<SerializationType::Yaml> file = File<SerializationType::Yaml>(filepath, SerializationMode::Deserialize);

		auto scene = file["Scene"];
		if (scene)
			m_Scene->m_ID = UUID(scene.as<uint64_t>());

		auto sceneData = file["Data"];
		if (sceneData)
		{
			auto entities = sceneData["Entities"];
			if (entities)
			{
				for (auto iterator : entities)
				{
					UUID uuid = iterator["Entity"].as<uint64_t>();
					m_Scene->m_Registry.CreateEntity(uuid);
					Entity entity = m_Scene->m_Registry.GetDict()[uuid];

					file.SetDetailIterator(iterator);

					DeserializeEntity(AllComponents(), file, uuid, entity);
				}
			}
		}

		file.SetActive(File<SerializationType::Yaml>::Active::Node);
	}

}