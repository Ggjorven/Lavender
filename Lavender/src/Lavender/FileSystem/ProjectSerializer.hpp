#pragma once

#include <filesystem>

#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Workspace/Project.hpp"

namespace Lavender
{

	class ProjectSerializer
	{
	public:
		ProjectSerializer(Ref<Project> project);
		virtual ~ProjectSerializer();

		void Serialize();
		void Deserialize(const std::filesystem::path& path);

	private:
		Ref<Project> m_Project = nullptr;
	};

}