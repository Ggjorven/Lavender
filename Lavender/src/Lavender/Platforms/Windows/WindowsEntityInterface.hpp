#pragma once

#include "Lavender/Utils/Utils.hpp"

#include "Lavender/ECS/Entity.hpp"
#include "Lavender/Scripting/EntityInterface.hpp"
#include "Lavender/Platforms/Windows/WindowsScriptLoader.hpp"

#include <Windows.h>

namespace Lavender
{

	class WindowsEntityInterface : public EntityInterface
	{
	public:
		WindowsEntityInterface(Entity& entity, Ref<ScriptLoader> loader, const std::string& classname);
		virtual ~WindowsEntityInterface();

		void InvokeOnCreate() override;
		void InvokeOnUpdate(float deltaTime) override;

	private:
		Ref<WindowsScriptLoader> m_Loader = nullptr;
		std::string m_ClassName = "Unset Class";

		Entity m_Entity = {};

		ScriptableEntity* m_Instance = nullptr;
		ScriptableEntityFunctions m_Functions = {};
	};

}