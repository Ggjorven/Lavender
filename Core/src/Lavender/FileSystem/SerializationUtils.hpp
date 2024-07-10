#pragma once

#include <glm/glm.hpp>

#include <yaml-cpp/yaml.h>

#include <Flow/Binary/BinaryOptions.hpp>

#include "Lavender/ECS/Components.hpp"

#include "Lavender/FileSystem/Serialization.hpp"

//////////////////////////////////////////////////////////////////////
// YAML utils
//////////////////////////////////////////////////////////////////////
namespace YAML
{

	template<>
	struct convert<glm::vec2>
	{
		static Node encode(const glm::vec2& rhs);
		static bool decode(const Node& node, glm::vec2& rhs);
	};

	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs);
		static bool decode(const Node& node, glm::vec3& rhs);
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs);
		static bool decode(const Node& node, glm::vec4& rhs);
	};

	Emitter& operator << (Emitter& out, const glm::vec2& v);
	Emitter& operator << (Emitter& out, const glm::vec3& v);
	Emitter& operator << (Emitter& out, const glm::vec4& v);

}

//////////////////////////////////////////////////////////////////////
// Binary utils
//////////////////////////////////////////////////////////////////////
namespace Flow::Binary
{

	// std::string is already defined in Flow/Binary/BinaryOptions.hpp

	template<>
	struct Type<glm::vec2>
	{
	public:
		static void Write(std::ofstream& output, glm::vec2 data);
		static glm::vec2 Read(std::vector<char>& input, size_t offset = 0);

		static size_t Size(glm::vec2 value = glm::vec2());
	};

	template<>
	struct Type<glm::vec3>
	{
	public:
		static void Write(std::ofstream& output, glm::vec3 data);
		static glm::vec3 Read(std::vector<char>& input, size_t offset = 0);

		static size_t Size(glm::vec3 value = glm::vec3());
	};

	template<>
	struct Type<glm::vec4>
	{
	public:
		static void Write(std::ofstream& output, glm::vec4 data);
		static glm::vec4 Read(std::vector<char>& input, size_t offset = 0);

		static size_t Size(glm::vec4 value = glm::vec4());
	};

}

//////////////////////////////////////////////////////////////////////
// Components // TODO: Binary
//////////////////////////////////////////////////////////////////////
namespace Lavender
{

	template<>
	struct SerializableType<TagComponent>
	{
	public:
		static void Serialize(File<SerializationType::Yaml>& file, const TagComponent& value);
		static void Serialize(File<SerializationType::Binary>& file, const TagComponent& value);
		static void Deserialize(File<SerializationType::Yaml>& file, TagComponent& result);
		static void Deserialize(File<SerializationType::Binary>& file, size_t& offset, TagComponent& result);
	};

	template<>
	struct SerializableType<TransformComponent>
	{
	public:
		static void Serialize(File<SerializationType::Yaml>& file, const TransformComponent& value);
		static void Serialize(File<SerializationType::Binary>& file, const TransformComponent& value);
		static void Deserialize(File<SerializationType::Yaml>& file, TransformComponent& result);
		static void Deserialize(File<SerializationType::Binary>& file, size_t& offset, TransformComponent& result);
	};

	template<>
	struct SerializableType<MeshComponent>
	{
	public:
		static void Serialize(File<SerializationType::Yaml>& file, const MeshComponent& value);
		static void Serialize(File<SerializationType::Binary>& file, const MeshComponent& value);
		static void Deserialize(File<SerializationType::Yaml>& file, MeshComponent& result);
		static void Deserialize(File<SerializationType::Binary>& file, size_t& offset, MeshComponent& result);
	};

	template<>
	struct SerializableType<PointLightComponent>
	{
	public:
		static void Serialize(File<SerializationType::Yaml>& file, const PointLightComponent& value);
		static void Serialize(File<SerializationType::Binary>& file, const PointLightComponent& value);
		static void Deserialize(File<SerializationType::Yaml>& file, PointLightComponent& result);
		static void Deserialize(File<SerializationType::Binary>& file, size_t& offset, PointLightComponent& result);
	};

	template<>
	struct SerializableType<ScriptComponent>
	{
	public:
		static void Serialize(File<SerializationType::Yaml>& file, const ScriptComponent& value);
		static void Serialize(File<SerializationType::Binary>& file, const ScriptComponent& value);
		static void Deserialize(File<SerializationType::Yaml>& file, ScriptComponent& result);
		static void Deserialize(File<SerializationType::Binary>& file, size_t& offset, ScriptComponent& result);
	};

	template<>
	struct SerializableType<CameraComponent>
	{
	public:
		static void Serialize(File<SerializationType::Yaml>& file, const CameraComponent& value);
		static void Serialize(File<SerializationType::Binary>& file, const CameraComponent& value);
		static void Deserialize(File<SerializationType::Yaml>& file, CameraComponent& result);
		static void Deserialize(File<SerializationType::Binary>& file, size_t& offset, CameraComponent& result);
	};

}