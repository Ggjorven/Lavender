#include "lvpch.h"
#include "SceneSerializer.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/ECS/Components.hpp"

#include "Lavender/WorkSpace/Project.hpp"

//////////////////////////////////////////////////
// GLM Utilities
//////////////////////////////////////////////////
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace YAML
{

	template<>
	struct convert<glm::vec2>
	{
		static Node encode(const glm::vec2& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			return node;
		}

		static bool decode(const Node& node, glm::vec2& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};

}

namespace Lavender
{

	inline YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return out;
	}

	inline YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	inline YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}

}


//////////////////////////////////////////////////
// Core Functions
//////////////////////////////////////////////////
using namespace Flow;

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

		Yaml::File file = Yaml::File(Project::Get()->GetInfo().Directory / Project::Get()->GetInfo().Scenes / info.Path, FileMode::Write);
		file << Yaml::FileManip::BeginMap;
		
		file << Yaml::FileManip::Key << "Scene";
		file << Yaml::FileManip::Value << (uint64_t)m_Scene->m_ID;

		file << Yaml::FileManip::Key << "Data";
		file << Yaml::FileManip::Value << YAML::BeginMap;

		// Entities
		file << Yaml::FileManip::Key << "Entities";
		file << Yaml::FileManip::Value << YAML::BeginSeq;

		for (auto& entity : m_Scene->m_Registries[WorkSpace::State::Editor].GetDict())
			SerializeEntity(file.GetEmitter(), entity.first);

		file << Yaml::FileManip::EndSeq;

		file << Yaml::FileManip::EndMap;
		file << Yaml::FileManip::EndMap;
	}

	void SceneSerializer::Deserialize(const std::filesystem::path& filepath)
	{
		auto& info = m_Scene->m_Info;
		info.Path = std::filesystem::relative(filepath, Project::Get()->GetInfo().Directory / Project::Get()->GetInfo().Scenes);

		Yaml::File file = Yaml::File(filepath, FileMode::Read);

		auto scene = file["Scene"];
		if (scene)
			m_Scene->m_ID = UUID(scene.as<uint64_t>());

		auto sceneData = file["Data"];
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

	// Entity Functions
	void SceneSerializer::SerializeEntity(YAML::Emitter& emitter, const UUID& uuid)
	{
		Entity entity = m_Scene->GetRegistry(WorkSpace::State::Editor).GetDict()[uuid];

		emitter << YAML::BeginMap;
		emitter << YAML::Key << "Entity";
		emitter << YAML::Value << (uint64_t)uuid;

		// TagComponent
		if (entity.HasComponent<TagComponent>())
		{
			TagComponent& tagComponent = entity.GetComponent<TagComponent>();
			emitter << YAML::Key << "TagComponent";
			emitter << YAML::BeginMap;

			emitter << YAML::Key << "Tag" << YAML::Value << tagComponent.Tag;

			emitter << YAML::EndMap;
		}

		// TransformComponent
		if (entity.HasComponent<TransformComponent>())
		{
			TransformComponent& transform = entity.GetComponent<TransformComponent>();
			emitter << YAML::Key << "TransformComponent";
			emitter << YAML::BeginMap;

			emitter << YAML::Key << "Position" << YAML::Value << transform.Position;
			emitter << YAML::Key << "Size" << YAML::Value << transform.Size;
			emitter << YAML::Key << "Rotation" << YAML::Value << transform.Rotation;

			emitter << YAML::EndMap;
		}

		// MeshComponent
		if (entity.HasComponent<MeshComponent>())
		{
			MeshComponent& mesh = entity.GetComponent<MeshComponent>();
			emitter << YAML::Key << "MeshComponent";
			emitter << YAML::BeginMap;

			emitter << YAML::Key << "Mesh" << YAML::Value << (mesh.Mesh ? (uint64_t)mesh.Mesh->GetHandle() : 0ul);
			emitter << YAML::Key << "Material" << YAML::Value << (mesh.Material ? (uint64_t)mesh.Material->GetHandle() : 0ul);

			emitter << YAML::EndMap;
		}

		// PointLightComponent
		if (entity.HasComponent<PointLightComponent>())
		{
			PointLightComponent& pointLight = entity.GetComponent<PointLightComponent>();
			emitter << YAML::Key << "PointLightComponent";
			emitter << YAML::BeginMap;

			emitter << YAML::Key << "Colour" << YAML::Value << pointLight.Colour;
			emitter << YAML::Key << "Intensity" << YAML::Value << pointLight.Intensity;
			emitter << YAML::Key << "Radius" << YAML::Value << pointLight.Radius;

			emitter << YAML::EndMap;
		}

		// ScriptComponent
		if (entity.HasComponent<ScriptComponent>())
		{
			ScriptComponent& script = entity.GetComponent<ScriptComponent>();
			emitter << YAML::Key << "ScriptComponent";
			emitter << YAML::BeginMap;

			emitter << YAML::Key << "Class" << YAML::Value << script.Class;

			emitter << YAML::EndMap;
		}

		// CameraComponent
		if (entity.HasComponent<CameraComponent>())
		{
			CameraComponent& camera = entity.GetComponent<CameraComponent>();
			emitter << YAML::Key << "CameraComponent";
			emitter << YAML::BeginMap;

			emitter << YAML::Key << "Yaw" << YAML::Value << camera.Yaw;
			emitter << YAML::Key << "Pitch" << YAML::Value << camera.Pitch;
			emitter << YAML::Key << "FOV" << YAML::Value << camera.FOV;
			emitter << YAML::Key << "Active" << YAML::Value << camera.Active;
			emitter << YAML::Key << "Near" << YAML::Value << camera.Near;
			emitter << YAML::Key << "Far" << YAML::Value << camera.Far;

			emitter << YAML::EndMap;
		}

		emitter << YAML::EndMap;
	}

	void SceneSerializer::DeserializeEntity(YAML::detail::iterator_value& node)
	{
		UUID uuid = node["Entity"].as<uint64_t>();
		m_Scene->m_Registries[Project::Get()->GetState()].CreateEntity(uuid);
		Entity entity = m_Scene->m_Registries[Project::Get()->GetState()].GetDict()[uuid];

		// TagComponent
		auto tagComponent = node["TagComponent"];
		if (tagComponent)
		{
			TagComponent& tag = entity.AddOrReplaceComponent<TagComponent>();
			tag.Tag = tagComponent["Tag"].as<std::string>();
		}

		// TransformComponent
		auto transformComponent = node["TransformComponent"];
		if (transformComponent)
		{
			TransformComponent& transform = entity.AddOrReplaceComponent<TransformComponent>();
			transform.Position = transformComponent["Position"].as<glm::vec3>();
			transform.Size = transformComponent["Size"].as<glm::vec3>();
			transform.Rotation = transformComponent["Rotation"].as<glm::vec3>();
		}

		// MeshComponent
		auto meshComponent = node["MeshComponent"];
		if (meshComponent)
		{
			MeshComponent& mesh = entity.AddOrReplaceComponent<MeshComponent>();

			Ref<Asset> tempAsset = Project::Get()->GetAssets()->GetAsset(meshComponent["Mesh"].as<uint64_t>());
			if (tempAsset)
				mesh.Mesh = RefHelper::RefAs<MeshAsset>(tempAsset);

			tempAsset = Project::Get()->GetAssets()->GetAsset(meshComponent["Material"].as<uint64_t>());
			if (tempAsset)
				mesh.Material = RefHelper::RefAs<MaterialAsset>(tempAsset);
		}

		// PointLightComponent
		auto pointLightComponent = node["PointLightComponent"];
		if (pointLightComponent)
		{
			PointLightComponent& pointLight = entity.AddOrReplaceComponent<PointLightComponent>();
			pointLight.Colour = pointLightComponent["Colour"].as<glm::vec3>();
			pointLight.Intensity = pointLightComponent["Intensity"].as<float>();
			pointLight.Radius = pointLightComponent["Radius"].as<float>();
		}

		// ScriptComponent
		auto scriptComponent = node["ScriptComponent"];
		if (scriptComponent)
		{
			ScriptComponent& script = entity.AddOrReplaceComponent<ScriptComponent>();

			if (Project::Get()->GetScript())
				Project::Get()->GetScript()->AddInstance(scriptComponent["Class"].as<std::string>(), uuid);

			script.Class = scriptComponent["Class"].as<std::string>();
		}

		// CameraComponent
		auto cameraComponent = node["CameraComponent"];
		if (cameraComponent)
		{
			CameraComponent& camera = entity.AddOrReplaceComponent<CameraComponent>();

			camera.Yaw = cameraComponent["Yaw"].as<float>();
			camera.Pitch = cameraComponent["Pitch"].as<float>();
			camera.FOV = cameraComponent["FOV"].as<float>();
			camera.Active = cameraComponent["Active"].as<bool>();
			camera.Near = cameraComponent["Near"].as<float>();
			camera.Far = cameraComponent["Far"].as<float>();
		}
	}

}