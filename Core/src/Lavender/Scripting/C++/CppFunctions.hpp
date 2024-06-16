#pragma once

#include "Lavender/Core/Core.hpp"
#include "Lavender/Utils/Utils.hpp"
#include "Lavender/Core/Input/Input.hpp"

#include "Lavender/ECS/Components.hpp"

#include "Lavender/Scripting/C++/Mutual/Core/Functions.hpp"
#include "Lavender/Scripting/C++/Mutual/Core/Logger.hpp"
#include "Lavender/Scripting/C++/Mutual/ECS/Components.hpp"
#include "Lavender/Scripting/C++/Mutual/Input/Input.hpp"

#include "Lavender/WorkSpace/Project.hpp"

#include <Insight/Insight.hpp>

namespace Lavender
{

	// Note(Jorben): This class is far from perfect, we have to implement every component manually
	class CppFunctions
	{
	public:
		static void OutSource(Ref<Insight::Dll> dll);

	public:
		/////////////////////////////////////////////////////
		// Components
		/////////////////////////////////////////////////////
		// TODO: Give Script::Component(s)
		inline static void* Internal_AddComponent(Script::ComponentType type, UUID uuid, void* data)
		{
			Entity entity = Project::Get()->GetScenes().GetActive()->GetRegistry(Project::Get()->GetState()).GetEntity(uuid);

			switch (type)
			{
			case Script::ComponentType::Tag:
			{
				Script::TagComponent& input = *((Script::TagComponent*)data);
				
				TagComponent& newComponent = entity.AddComponent<TagComponent>();
				newComponent.Tag = (std::string)input.Tag;

				return Internal_GetComponent(type, uuid);
			}
			case Script::ComponentType::Transform:
			{
				Script::TransformComponent& input = *((Script::TransformComponent*)data);

				TransformComponent& newComponent = entity.AddComponent<TransformComponent>();
				newComponent.Position = (glm::vec3)input.Position;
				newComponent.Size = (glm::vec3)input.Size;
				newComponent.Rotation = (glm::vec3)input.Rotation;

				return Internal_GetComponent(type, uuid);
			}
			case Script::ComponentType::PointLight:
			{
				Script::PointLightComponent & input = *((Script::PointLightComponent*)data);

				PointLightComponent& newComponent = entity.AddComponent<PointLightComponent>();
				newComponent.Colour = (glm::vec3)input.Colour;
				newComponent.Intensity = (float)input.Intensity;
				newComponent.Radius = (float)input.Radius;

				return Internal_GetComponent(type, uuid);
			}
			// TODO: ...

			default:
				APP_LOG_ERROR("Invalid ComponentType passed in.");
				break;
			}

			return nullptr;
		}

		inline static void* Internal_AddOrReplaceComponent(Script::ComponentType type, UUID uuid, void* data)
		{
			Entity entity = Project::Get()->GetScenes().GetActive()->GetRegistry(Project::Get()->GetState()).GetEntity(uuid);

			switch (type)
			{
			case Script::ComponentType::Tag:
			{
				Script::TagComponent& input = *((Script::TagComponent*)data);

				TagComponent& newComponent = entity.AddOrReplaceComponent<TagComponent>();
				newComponent.Tag = (std::string)input.Tag;

				return Internal_GetComponent(type, uuid);
			}
			case Script::ComponentType::Transform:
			{
				Script::TransformComponent& input = *((Script::TransformComponent*)data);

				TransformComponent& newComponent = entity.AddOrReplaceComponent<TransformComponent>();
				newComponent.Position = (glm::vec3)input.Position;
				newComponent.Size = (glm::vec3)input.Size;
				newComponent.Rotation = (glm::vec3)input.Rotation;

				return Internal_GetComponent(type, uuid);
			}
			case Script::ComponentType::PointLight:
			{
				Script::PointLightComponent& input = *((Script::PointLightComponent*)data);

				PointLightComponent& newComponent = entity.AddOrReplaceComponent<PointLightComponent>();
				newComponent.Colour = (glm::vec3)input.Colour;
				newComponent.Intensity = (float)input.Intensity;
				newComponent.Radius = (float)input.Radius;

				return Internal_GetComponent(type, uuid);
			}
			// TODO: ...

			default:
				APP_LOG_ERROR("Invalid ComponentType passed in.");
				break;
			}

			return nullptr;
		}

		inline static bool Internal_HasComponent(Script::ComponentType type, UUID uuid)
		{
			Entity entity = Project::Get()->GetScenes().GetActive()->GetRegistry(Project::Get()->GetState()).GetEntity(uuid);

			switch (type)
			{

			#define HASCOMPONENT_INTERNAL_HELP(name) \
			case Script::ComponentType::name: return entity.HasComponent<name##Component>();

			HASCOMPONENT_INTERNAL_HELP(Tag)
			HASCOMPONENT_INTERNAL_HELP(Transform)
			HASCOMPONENT_INTERNAL_HELP(Mesh)
			HASCOMPONENT_INTERNAL_HELP(PointLight)

			default:
				APP_LOG_ERROR("Invalid ComponentType passed in.");
				break;
			}

			return false;
		}

		inline static void* Internal_GetComponent(Script::ComponentType type, UUID uuid)
		{
			Entity entity = Project::Get()->GetScenes().GetActive()->GetRegistry(Project::Get()->GetState()).GetEntity(uuid);

			switch (type)
			{
			case Script::ComponentType::Tag:
			{
				static Script::TagComponent lastComponent = {};
				TagComponent& tag = entity.GetComponent<TagComponent>();

				lastComponent.Tag = Script::Handle<std::string>(&tag.Tag);
				
				return (void*)&lastComponent;
			}
			case Script::ComponentType::Transform:
			{
				static Script::TransformComponent lastComponent = {};
				TransformComponent& transform = entity.GetComponent<TransformComponent>();

				lastComponent.Position = Script::Handle<Script::Vec3>(&transform.Position);
				lastComponent.Size = Script::Handle<Script::Vec3>(&transform.Size);
				lastComponent.Rotation = Script::Handle<Script::Vec3>(&transform.Rotation);

				return (void*)&lastComponent;
			}
			case Script::ComponentType::PointLight:
			{
				static Script::PointLightComponent lastComponent = {};
				PointLightComponent& pointLight = entity.GetComponent<PointLightComponent>();

				lastComponent.Colour = Script::Handle<Script::Vec3>(&pointLight.Colour);
				lastComponent.Intensity = Script::Handle<float>(&pointLight.Intensity);
				lastComponent.Radius = Script::Handle<float>(&pointLight.Radius);

				return (void*)&lastComponent;
			}
			// TODO: ...

			default:
				APP_LOG_ERROR("Invalid ComponentType passed in.");
				break;
			}

			return nullptr;
		}

		inline static void Internal_RemoveComponent(Script::ComponentType type, UUID uuid)
		{
			Entity entity = Project::Get()->GetScenes().GetActive()->GetRegistry(Project::Get()->GetState()).GetEntity(uuid);

			switch (type)
			{

			#define REMOVECOMPONENT_INTERNAL_HELP(name) \
			case Script::ComponentType::name: \
				entity.RemoveComponent<name##Component>(); \
				break;

			REMOVECOMPONENT_INTERNAL_HELP(Tag)
			REMOVECOMPONENT_INTERNAL_HELP(Transform)
			REMOVECOMPONENT_INTERNAL_HELP(Mesh)
			REMOVECOMPONENT_INTERNAL_HELP(PointLight)

			default:
				APP_LOG_ERROR("Invalid ComponentType passed in.");
				break;
			}
		}

		/////////////////////////////////////////////////////
		// Logging
		/////////////////////////////////////////////////////
		inline static void Internal_LogMessage(uint8_t level, const char* str)
		{
			Log::LogMessage((Log::Level)level, str);
		}

		/////////////////////////////////////////////////////
		// Input
		/////////////////////////////////////////////////////
		inline static bool Internal_IsKeyPressed(Script::Key key)
		{
			return Input::IsKeyPressed((Key)key);
		}

		inline static bool Internal_IsMousePressed(Script::MouseButton button)
		{
			return Input::IsMousePressed((MouseButton)button);
		}

		inline static glm::vec2* Internal_GetCursorPosition()
		{
			static glm::vec2 lastPosition = {};
			lastPosition = Input::GetCursorPosition();

			return &lastPosition;
		}

		inline static void Internal_SetCursorPosition(glm::vec2* position)
		{
			Input::SetCursorPosition(*position);
		}

		inline static void Internal_SetCursorMode(Script::CursorMode mode)
		{
			Input::SetCursorMode((CursorMode)mode);
		}
	};

}