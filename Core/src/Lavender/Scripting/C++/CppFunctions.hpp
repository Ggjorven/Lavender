#pragma once

#include "Lavender/Core/Core.hpp"
#include "Lavender/Utils/Utils.hpp"
#include "Lavender/Core/Application.hpp"
#include "Lavender/Core/Input/Input.hpp"

#include "Lavender/ECS/Components.hpp"

#include "Lavender/Scripting/C++/Mutual/Core/Functions.hpp"
#include "Lavender/Scripting/C++/Mutual/Core/Logger.hpp"
#include "Lavender/Scripting/C++/Mutual/ECS/Components.hpp"
#include "Lavender/Scripting/C++/Mutual/Input/Input.hpp"

#include "Lavender/WorkSpace/Project.hpp"
#include "Lavender/WorkSpace/EngineTracker.hpp"

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
		// Window
		/////////////////////////////////////////////////////
		inline static uint32_t Internal_GetWidth()
		{
			return Track::Viewport::Width;
		}

		inline static uint32_t Internal_GetHeight()
		{
			return Track::Viewport::Height;
		}

		inline static uint32_t Internal_GetXPosition()
		{
			return Track::Viewport::Position.x;
		}

		inline static uint32_t Internal_GetYPosition()
		{
			return Track::Viewport::Position.y;
		}

		inline static uint32_t Internal_GetMonitorWidth()
		{
			return Application::Get().GetWindow().GetMonitorWidth();
		}

		inline static uint32_t Internal_GetMonitorHeight()
		{
			return Application::Get().GetWindow().GetMonitorHeight();
		}

		inline static void Internal_SetVSync(bool enabled)
		{
			Application::Get().GetWindow().SetVSync(enabled);
		}

		inline static bool Internal_IsVSync()
		{
			return Application::Get().GetWindow().IsVSync();
		}

		inline static void Internal_SetTitle(const char* title)
		{
			Application::Get().GetWindow().SetTitle(std::string(title));
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

		/////////////////////////////////////////////////////
		// Components
		/////////////////////////////////////////////////////
		inline static void* Internal_AddComponent(Script::ComponentType type, Script::UUID uuid, void* data)
		{
			Entity entity = Scene::Get()->GetRegistry(Project::Get()->GetState()).GetEntity((uint64_t)uuid);

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
			// TODO: MeshComponent
			case Script::ComponentType::PointLight:
			{
				Script::PointLightComponent & input = *((Script::PointLightComponent*)data);

				PointLightComponent& newComponent = entity.AddComponent<PointLightComponent>();
				newComponent.Colour = (glm::vec3)input.Colour;
				newComponent.Intensity = (float)input.Intensity;
				newComponent.Radius = (float)input.Radius;

				return Internal_GetComponent(type, uuid);
			}
			// TODO: ScriptComponent
			case Script::ComponentType::Camera:
			{
				Script::CameraComponent& input = *((Script::CameraComponent*)data);

				CameraComponent& newComponent = entity.AddComponent<CameraComponent>();
				newComponent.Yaw = (float)newComponent.Yaw;
				newComponent.Pitch = (float)newComponent.Pitch;
				newComponent.FOV = (float)newComponent.FOV;
				newComponent.Far = (float)newComponent.Far;
				newComponent.Near = (float)newComponent.Near;
				newComponent.Active = (bool)newComponent.Active;

				return Internal_GetComponent(type, uuid);
			}

			default:
				APP_LOG_ERROR("Invalid ComponentType passed in.");
				break;
			}

			return nullptr;
		}

		inline static void* Internal_AddOrReplaceComponent(Script::ComponentType type, Script::UUID uuid, void* data)
		{
			Entity entity = Scene::Get()->GetRegistry(Project::Get()->GetState()).GetEntity((uint64_t)uuid);

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
			// TODO: MeshComponent
			case Script::ComponentType::PointLight:
			{
				Script::PointLightComponent& input = *((Script::PointLightComponent*)data);

				PointLightComponent& newComponent = entity.AddOrReplaceComponent<PointLightComponent>();
				newComponent.Colour = (glm::vec3)input.Colour;
				newComponent.Intensity = (float)input.Intensity;
				newComponent.Radius = (float)input.Radius;

				return Internal_GetComponent(type, uuid);
			}
			// TODO: ScriptComponent
			case Script::ComponentType::Camera:
			{
				Script::CameraComponent& input = *((Script::CameraComponent*)data);

				CameraComponent& newComponent = entity.AddComponent<CameraComponent>();
				newComponent.Yaw = (float)newComponent.Yaw;
				newComponent.Pitch = (float)newComponent.Pitch;
				newComponent.FOV = (float)newComponent.FOV;
				newComponent.Far = (float)newComponent.Far;
				newComponent.Near = (float)newComponent.Near;
				newComponent.Active = (bool)newComponent.Active;

				return Internal_GetComponent(type, uuid);
			}

			default:
				APP_LOG_ERROR("Invalid ComponentType passed in.");
				break;
			}

			return nullptr;
		}

		inline static bool Internal_HasComponent(Script::ComponentType type, Script::UUID uuid)
		{
			Entity entity = Scene::Get()->GetRegistry(Project::Get()->GetState()).GetEntity((uint64_t)uuid);

			switch (type)
			{

			#define HASCOMPONENT_INTERNAL_HELP(name) \
			case Script::ComponentType::name: return entity.HasComponent<name##Component>();

			HASCOMPONENT_INTERNAL_HELP(Tag)
			HASCOMPONENT_INTERNAL_HELP(Transform)
			HASCOMPONENT_INTERNAL_HELP(Mesh)
			HASCOMPONENT_INTERNAL_HELP(PointLight)
			HASCOMPONENT_INTERNAL_HELP(Script)
			HASCOMPONENT_INTERNAL_HELP(Camera)

			default:
				APP_LOG_ERROR("Invalid ComponentType passed in.");
				break;
			}

			return false;
		}

		inline static void* Internal_GetComponent(Script::ComponentType type, Script::UUID uuid)
		{
			Entity entity = Scene::Get()->GetRegistry(Project::Get()->GetState()).GetEntity((uint64_t)uuid);

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
			// TODO: MeshComponent
			case Script::ComponentType::PointLight:
			{
				static Script::PointLightComponent lastComponent = {};
				PointLightComponent& pointLight = entity.GetComponent<PointLightComponent>();

				lastComponent.Colour = Script::Handle<Script::Vec3>(&pointLight.Colour);
				lastComponent.Intensity = Script::Handle<float>(&pointLight.Intensity);
				lastComponent.Radius = Script::Handle<float>(&pointLight.Radius);

				return (void*)&lastComponent;
			}
			// TODO: ScriptComponent
			case Script::ComponentType::Camera:
			{
				static Script::CameraComponent lastComponent = {};
				CameraComponent& camera = entity.GetComponent<CameraComponent>();

				lastComponent.Yaw = Script::Handle<float>(&camera.Yaw);
				lastComponent.Pitch = Script::Handle<float>(&camera.Pitch);
				lastComponent.FOV = Script::Handle<float>(&camera.FOV);
				lastComponent.Near = Script::Handle<float>(&camera.Near);
				lastComponent.Far = Script::Handle<float>(&camera.Far);
				lastComponent.Active = Script::Handle<bool>(&camera.Active);

				return (void*)&lastComponent;
			}

			default:
				APP_LOG_ERROR("Invalid ComponentType passed in.");
				break;
			}

			return nullptr;
		}

		inline static void Internal_RemoveComponent(Script::ComponentType type, Script::UUID uuid)
		{
			Entity entity = Scene::Get()->GetRegistry(Project::Get()->GetState()).GetEntity((uint64_t)uuid);

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
			REMOVECOMPONENT_INTERNAL_HELP(Script)
			REMOVECOMPONENT_INTERNAL_HELP(Camera)

			default:
				APP_LOG_ERROR("Invalid ComponentType passed in.");
				break;
			}
		}
	};

}