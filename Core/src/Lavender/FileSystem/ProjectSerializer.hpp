#pragma once

#include "Lavender/Core/Core.hpp"
#include "Lavender/Utils/Utils.hpp"

#include "Lavender/WorkSpace/Project.hpp"

namespace Lavender
{

	class ProjectSerializer
	{
	public:
		ProjectSerializer(Ref<Project> project);
		virtual ~ProjectSerializer();

		void Serialize();
		void Deserialize(const std::filesystem::path& filepath);

	private:
		Ref<Project> m_Project = nullptr;

		friend class Project;
	};

}