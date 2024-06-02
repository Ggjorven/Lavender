#pragma once
 
#include "Lavender/Core/Core.hpp"
#include "Lavender/Utils/Utils.hpp"

#include "Lavender/WorkSpace/Scene.hpp"
#include "Lavender/WorkSpace/WorkSpace.hpp"

namespace Lavender
{

	class ProjectSerializer;

	class Project
	{
	public:
		Project(const WorkSpace::ProjectInfo& info);
		virtual ~Project();

		inline const WorkSpace::ProjectInfo& GetInfo() { return m_Info; }
		inline WorkSpace::State& GetState() { return m_State; }
		inline SceneCollection& GetScenes() { return m_Scenes; }

		static Ref<Project> Create(const WorkSpace::ProjectInfo& info = WorkSpace::ProjectInfo());
		static Ref<Project> Get();

	private:
		void Init();
		void Destroy();

	private:
		WorkSpace::ProjectInfo m_Info = {};
		WorkSpace::State m_State = WorkSpace::State::None;
		SceneCollection m_Scenes = { };

		friend class ProjectSerializer;
	};

}