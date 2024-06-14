#pragma once

#include "Lavender/Core/Core.hpp"
#include "Lavender/Utils/Utils.hpp"
#include "Lavender/Core/Input/Input.hpp"

#include "Lavender/ECS/Components.hpp"

#include "Lavender/Scripting/C++/Mutual/Functions.hpp"

#include "Lavender/WorkSpace/Project.hpp"

#include <Insight/Insight.hpp>

namespace Lavender
{

	// Note(Jorben): This class is far from perfect, we have to implement every component seperately
	class CppFunctions
	{
	public:
		static void OutSource(Ref<Insight::Dll> dll);

	public:
		/////////////////////////////////////////////////////
		// Components
		/////////////////////////////////////////////////////
		inline static void* Internal_AddComponent(ComponentType type, UUID uuid, void* data)
		{
			Entity entity = Project::Get()->GetScenes().GetActive()->GetRegistry(Project::Get()->GetState()).GetEntity(uuid);

			switch (type)
			{

			#define ADDCOMPONENT_INTERNAL_HELP(name) \
			case ComponentType::name: \
			{ \
				name##Component component = *((name##Component*)data); \
				entity.AddComponent<name##Component>(component); \
				return (void*)(&entity.GetComponent<name##Component>()); \
			}

			ADDCOMPONENT_INTERNAL_HELP(Tag)
			ADDCOMPONENT_INTERNAL_HELP(Transform)
			ADDCOMPONENT_INTERNAL_HELP(Mesh)
			ADDCOMPONENT_INTERNAL_HELP(PointLight)

			default:
				APP_LOG_ERROR("Invalid ComponentType passed in.");
				break;
			}

			return nullptr;
		}

		inline static void* Internal_AddOrReplaceComponent(ComponentType type, UUID uuid, void* data)
		{
			Entity entity = Project::Get()->GetScenes().GetActive()->GetRegistry(Project::Get()->GetState()).GetEntity(uuid);

			switch (type)
			{

			#define ADDORREPLACECOMPONENT_INTERNAL_HELP(name) \
			case ComponentType::name: \
			{ \
				name##Component component = *((name##Component*)data); \
				entity.AddOrReplaceComponent<name##Component>(component); \
				return (void*)(&entity.GetComponent<name##Component>()); \
			}

			ADDORREPLACECOMPONENT_INTERNAL_HELP(Tag)
			ADDORREPLACECOMPONENT_INTERNAL_HELP(Transform)
			ADDORREPLACECOMPONENT_INTERNAL_HELP(Mesh)
			ADDORREPLACECOMPONENT_INTERNAL_HELP(PointLight)

			default:
				APP_LOG_ERROR("Invalid ComponentType passed in.");
				break;
			}

			return nullptr;
		}

		inline static bool Internal_HasComponent(ComponentType type, UUID uuid)
		{
			Entity entity = Project::Get()->GetScenes().GetActive()->GetRegistry(Project::Get()->GetState()).GetEntity(uuid);

			switch (type)
			{

			#define HASCOMPONENT_INTERNAL_HELP(name) \
			case ComponentType::name: return entity.HasComponent<name##Component>();

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

		inline static void* Internal_GetComponent(ComponentType type, UUID uuid)
		{
			Entity entity = Project::Get()->GetScenes().GetActive()->GetRegistry(Project::Get()->GetState()).GetEntity(uuid);

			switch (type)
			{

			#define GETCOMPONENT_INTERNAL_HELP(name) \
			case ComponentType::name: return (void*)(&entity.GetComponent<name##Component>());

			GETCOMPONENT_INTERNAL_HELP(Tag)
			GETCOMPONENT_INTERNAL_HELP(Transform)
			GETCOMPONENT_INTERNAL_HELP(Mesh)
			GETCOMPONENT_INTERNAL_HELP(PointLight)

			default:
				APP_LOG_ERROR("Invalid ComponentType passed in.");
				break;
			}

			return nullptr;
		}

		inline static void Internal_RemoveComponent(ComponentType type, UUID uuid)
		{
			Entity entity = Project::Get()->GetScenes().GetActive()->GetRegistry(Project::Get()->GetState()).GetEntity(uuid);

			switch (type)
			{

			#define REMOVECOMPONENT_INTERNAL_HELP(name) \
			case ComponentType::name: \
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
		inline static bool Internal_IsKeyPressed(Key key)
		{
			return Input::IsKeyPressed(key);
		}

		inline static bool Internal_IsMousePressed(MouseButton button)
		{
			return Input::IsMousePressed(button);
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

		inline static void Internal_SetCursorMode(CursorMode mode)
		{
			Input::SetCursorMode(mode);
		}
	};

}