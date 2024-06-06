#pragma once

#include "Lavender/Core/Core.hpp"
#include "Lavender/Utils/Utils.hpp"

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
		inline static void* Internal_AddComponent(ComponentType type, UUID uuid, void* data)
		{
			Entity entity = Project::Get()->GetScenes().GetActive()->GetRegistry(Project::Get()->GetState()).GetEntity(uuid);

			switch (type)
			{
			case ComponentType::Tag:
			{
				TagComponent component = *((TagComponent*)data);
				entity.AddComponent<TagComponent>(component);

				return (void*)(&entity.GetComponent<TagComponent>());
			}
			case ComponentType::Transform:
			{
				TransformComponent component = *((TransformComponent*)data);
				entity.AddComponent<TransformComponent>(component);

				return (void*)(&entity.GetComponent<TransformComponent>());
			}

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
			case ComponentType::Tag:
			{
				TagComponent component = *((TagComponent*)data);
				entity.AddOrReplaceComponent<TagComponent>(component);

				return (void*)(&entity.GetComponent<TagComponent>());
			}
			case ComponentType::Transform:
			{
				TransformComponent component = *((TransformComponent*)data);
				entity.AddOrReplaceComponent<TransformComponent>(component);

				return (void*)(&entity.GetComponent<TransformComponent>());
			}

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
			case ComponentType::Tag:		return entity.HasComponent<TagComponent>();
			case ComponentType::Transform:	return entity.HasComponent<TransformComponent>();

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
			case ComponentType::Tag:		return (void*)(&entity.GetComponent<TagComponent>());
			case ComponentType::Transform:	return (void*)(&entity.GetComponent<TransformComponent>());

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
			case ComponentType::Tag:	
				entity.RemoveComponent<TagComponent>();
				break;
			case ComponentType::Transform:
				entity.RemoveComponent<TransformComponent>();
				break;

			default:
				APP_LOG_ERROR("Invalid ComponentType passed in.");
				break;
			}
		}
	};

}