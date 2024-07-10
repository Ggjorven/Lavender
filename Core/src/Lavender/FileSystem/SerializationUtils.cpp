#include "lvpch.h"
#include "SerializationUtils.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/WorkSpace/Project.hpp"

//////////////////////////////////////////////////////////////////////
// YAML utils
//////////////////////////////////////////////////////////////////////
namespace YAML
{

	Node convert<glm::vec2>::encode(const glm::vec2& rhs)
	{
		Node node;
		node.push_back(rhs.x);
		node.push_back(rhs.y);
		return node;
	}

	bool convert<glm::vec2>::decode(const Node& node, glm::vec2& rhs)
	{
		if (!node.IsSequence() || node.size() != 2)
			return false;

		rhs.x = node[0].as<float>();
		rhs.y = node[1].as<float>();
		return true;
	}

	Node convert<glm::vec3>::encode(const glm::vec3& rhs)
	{
		Node node;
		node.push_back(rhs.x);
		node.push_back(rhs.y);
		node.push_back(rhs.z);
		return node;
	}

	bool convert<glm::vec3>::decode(const Node& node, glm::vec3& rhs)
	{
		if (!node.IsSequence() || node.size() != 3)
			return false;

		rhs.x = node[0].as<float>();
		rhs.y = node[1].as<float>();
		rhs.z = node[2].as<float>();
		return true;
	}

	Node convert<glm::vec4>::encode(const glm::vec4& rhs)
	{
		Node node;
		node.push_back(rhs.x);
		node.push_back(rhs.y);
		node.push_back(rhs.z);
		node.push_back(rhs.w);
		return node;
	}

	bool convert<glm::vec4>::decode(const Node& node, glm::vec4& rhs)
	{
		if (!node.IsSequence() || node.size() != 4)
			return false;

		rhs.x = node[0].as<float>();
		rhs.y = node[1].as<float>();
		rhs.z = node[2].as<float>();
		rhs.w = node[3].as<float>();
		return true;
	}

	YAML::Emitter& operator << (YAML::Emitter& out, const glm::vec2& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator << (YAML::Emitter& out, const glm::vec3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator << (YAML::Emitter& out, const glm::vec4& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}

}

//////////////////////////////////////////////////////////////////////
// Binary utils
//////////////////////////////////////////////////////////////////////
namespace Flow::Binary
{

	void Type<glm::vec2>::Write(std::ofstream& output, glm::vec2 data)
	{
		output.write(reinterpret_cast<const char*>(&data.x), sizeof(float));
		output.write(reinterpret_cast<const char*>(&data.y), sizeof(float));
	}

	glm::vec2 Type<glm::vec2>::Read(std::vector<char>& input, size_t offset)
	{
		glm::vec2 result = {};
		memcpy(&result, &input[offset], sizeof(glm::vec2));

		return result;
	}

	size_t Type<glm::vec2>::Size(glm::vec2 value)
	{
		return sizeof(glm::vec2);
	}

	void Type<glm::vec3>::Write(std::ofstream& output, glm::vec3 data)
	{
		output.write(reinterpret_cast<const char*>(&data.x), sizeof(float));
		output.write(reinterpret_cast<const char*>(&data.y), sizeof(float));
		output.write(reinterpret_cast<const char*>(&data.z), sizeof(float));
	}

	glm::vec3 Type<glm::vec3>::Read(std::vector<char>& input, size_t offset)
	{
		glm::vec3 result = {};
		memcpy(&result, &input[offset], sizeof(glm::vec3));

		return result;
	}

	size_t Type<glm::vec3>::Size(glm::vec3 value)
	{
		return sizeof(glm::vec3);
	}

	void Type<glm::vec4>::Write(std::ofstream& output, glm::vec4 data)
	{
		output.write(reinterpret_cast<const char*>(&data.x), sizeof(float));
		output.write(reinterpret_cast<const char*>(&data.y), sizeof(float));
		output.write(reinterpret_cast<const char*>(&data.z), sizeof(float));
		output.write(reinterpret_cast<const char*>(&data.w), sizeof(float));
	}

	glm::vec4 Type<glm::vec4>::Read(std::vector<char>& input, size_t offset)
	{
		glm::vec4 result = {};
		memcpy(&result, &input[offset], sizeof(glm::vec4));

		return result;
	}

	size_t Type<glm::vec4>::Size(glm::vec4 value)
	{
		return sizeof(glm::vec4);
	}

}

//////////////////////////////////////////////////////////////////////
// Components
//////////////////////////////////////////////////////////////////////
using namespace Flow;
using namespace Flow::Yaml;
using namespace Flow::Binary;

namespace Lavender
{

	// TagComponent
	void SerializableType<TagComponent>::Serialize(File<SerializationType::Yaml>& file, const TagComponent& value)
	{
		file << FileManip::Key << "TagComponent";
		file << FileManip::BeginMap;

		file << FileManip::Key << "Tag" << FileManip::Value << value.Tag;

		file << FileManip::EndMap;
	}

	void SerializableType<TagComponent>::Serialize(File<SerializationType::Binary>& file, const TagComponent& value)
	{
	}

	void SerializableType<TagComponent>::Deserialize(File<SerializationType::Yaml>& file, TagComponent& result)
	{
		auto tagComponent = file["TagComponent"];
		if (tagComponent)
		{
			result.Tag = tagComponent["Tag"].as<std::string>();
		}
	}

	void SerializableType<TagComponent>::Deserialize(File<SerializationType::Binary>& file, size_t& offset, TagComponent& result)
	{
	}

	// TransformComponent
	void SerializableType<TransformComponent>::Serialize(File<SerializationType::Yaml>& file, const TransformComponent& value)
	{
		file << FileManip::Key << "TransformComponent";
		file << FileManip::BeginMap;

		file << FileManip::Key << "Position" << FileManip::Value << value.Position;
		file << FileManip::Key << "Size" << FileManip::Value << value.Size;
		file << FileManip::Key << "Rotation" << FileManip::Value << value.Rotation;

		file << FileManip::EndMap;
	}

	void SerializableType<TransformComponent>::Serialize(File<SerializationType::Binary>& file, const TransformComponent& value)
	{
	}

	void SerializableType<TransformComponent>::Deserialize(File<SerializationType::Yaml>& file, TransformComponent& result)
	{
		auto transformComponent = file["TransformComponent"];
		if (transformComponent)
		{
			result.Position = transformComponent["Position"].as<glm::vec3>();
			result.Size = transformComponent["Size"].as<glm::vec3>();
			result.Rotation = transformComponent["Rotation"].as<glm::vec3>();
		}
	}

	void SerializableType<TransformComponent>::Deserialize(File<SerializationType::Binary>& file, size_t& offset, TransformComponent& result)
	{
	}

	// MeshComponent
	void SerializableType<MeshComponent>::Serialize(File<SerializationType::Yaml>& file, const MeshComponent& value)
	{
		file << FileManip::Key << "MeshComponent";
		file << FileManip::BeginMap;

		file << FileManip::Key << "Mesh" << FileManip::Value << (value.Mesh ? (uint64_t)value.Mesh->GetHandle() : 0ull);
		file << FileManip::Key << "Material" << FileManip::Value << (value.Material ? (uint64_t)value.Material->GetHandle() : 0ull);

		file << FileManip::EndMap;
	}

	void SerializableType<MeshComponent>::Serialize(File<SerializationType::Binary>& file, const MeshComponent& value)
	{
	}

	void SerializableType<MeshComponent>::Deserialize(File<SerializationType::Yaml>& file, MeshComponent& result)
	{
		auto meshComponent = file["MeshComponent"];
		if (meshComponent)
		{
			Ref<Asset> tempAsset = Project::Get()->GetAssets()->GetAsset(meshComponent["Mesh"].as<uint64_t>());
			if (tempAsset)
				result.Mesh = RefHelper::RefAs<MeshAsset>(tempAsset);

			tempAsset = Project::Get()->GetAssets()->GetAsset(meshComponent["Material"].as<uint64_t>());
			if (tempAsset)
				result.Material = RefHelper::RefAs<MaterialAsset>(tempAsset);
		}
	}

	void SerializableType<MeshComponent>::Deserialize(File<SerializationType::Binary>& file, size_t& offset, MeshComponent& result)
	{
	}

	// PointLightComponent
	void SerializableType<PointLightComponent>::Serialize(File<SerializationType::Yaml>& file, const PointLightComponent& value)
	{
		file << FileManip::Key << "PointLightComponent";
		file << FileManip::BeginMap;

		file << FileManip::Key << "Colour" << FileManip::Value << value.Colour;
		file << FileManip::Key << "Intensity" << FileManip::Value << value.Intensity;
		file << FileManip::Key << "Radius" << FileManip::Value << value.Radius;

		file << FileManip::EndMap;
	}

	void SerializableType<PointLightComponent>::Serialize(File<SerializationType::Binary>& file, const PointLightComponent& value)
	{
	}

	void SerializableType<PointLightComponent>::Deserialize(File<SerializationType::Yaml>& file, PointLightComponent& result)
	{
		auto pointLightComponent = file["PointLightComponent"];
		if (pointLightComponent)
		{
			result.Colour = pointLightComponent["Colour"].as<glm::vec3>();
			result.Intensity = pointLightComponent["Intensity"].as<float>();
			result.Radius = pointLightComponent["Radius"].as<float>();
		}
	}

	void SerializableType<PointLightComponent>::Deserialize(File<SerializationType::Binary>& file, size_t& offset, PointLightComponent& result)
	{
	}

	// ScriptComponent
	void SerializableType<ScriptComponent>::Serialize(File<SerializationType::Yaml>& file, const ScriptComponent& value)
	{
		file << FileManip::Key << "ScriptComponent";
		file << FileManip::BeginMap;

		file << FileManip::Key << "Class" << FileManip::Value << value.Class;

		file << FileManip::EndMap;
	}

	void SerializableType<ScriptComponent>::Serialize(File<SerializationType::Binary>& file, const ScriptComponent& value)
	{
	}

	void SerializableType<ScriptComponent>::Deserialize(File<SerializationType::Yaml>& file, ScriptComponent& result)
	{
		auto scriptComponent = file["ScriptComponent"];
		if (scriptComponent)
		{
			result.Class = scriptComponent["Class"].as<std::string>();
		}
	}

	void SerializableType<ScriptComponent>::Deserialize(File<SerializationType::Binary>& file, size_t& offset, ScriptComponent& result)
	{
	}

	// CameraComponent
	void SerializableType<CameraComponent>::Serialize(File<SerializationType::Yaml>& file, const CameraComponent& value)
	{
		file << FileManip::Key << "CameraComponent";
		file << FileManip::BeginMap;

		file << FileManip::Key << "Yaw" << FileManip::Value << value.Yaw;
		file << FileManip::Key << "Pitch" << FileManip::Value << value.Pitch;
		file << FileManip::Key << "FOV" << FileManip::Value << value.FOV;
		file << FileManip::Key << "Active" << FileManip::Value << value.Active;
		file << FileManip::Key << "Near" << FileManip::Value << value.Near;
		file << FileManip::Key << "Far" << FileManip::Value << value.Far;

		file << FileManip::EndMap;
	}

	void SerializableType<CameraComponent>::Serialize(File<SerializationType::Binary>& file, const CameraComponent& value)
	{
	}

	void SerializableType<CameraComponent>::Deserialize(File<SerializationType::Yaml>& file, CameraComponent& result)
	{
		auto cameraComponent = file["CameraComponent"];
		if (cameraComponent)
		{
			result.Yaw = cameraComponent["Yaw"].as<float>();
			result.Pitch = cameraComponent["Pitch"].as<float>();
			result.FOV = cameraComponent["FOV"].as<float>();
			result.Active = cameraComponent["Active"].as<bool>();
			result.Near = cameraComponent["Near"].as<float>();
			result.Far = cameraComponent["Far"].as<float>();
		}
	}

	void SerializableType<CameraComponent>::Deserialize(File<SerializationType::Binary>& file, size_t& offset, CameraComponent& result)
	{
	}

}