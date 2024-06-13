#pragma once

#include "Lavender/Core/Core.hpp"
#include "Lavender/Utils/Utils.hpp"

#include "Lavender/WorkSpace/WorkSpace.hpp"

#include <glm/glm.hpp>

// A namespace for keeping track of engine side stats
namespace Lavender::Track
{

	struct Frame
	{
	public:
		inline static float DeltaTime = 0.0f;

		inline static constexpr const float UpdateInterval = 1.0f;
		// Stats below get updated every 'UpdateInterval'
		inline static uint32_t FPS = 0u;
		inline static float FrameTime = 0.0f;
	}; 

	struct Viewport
	{
	public:
		inline static uint32_t Width = 0u;
		inline static uint32_t Height = 0u;

		inline static glm::uvec2 Position = { 0.0f, 0.0f };
	};

	struct Lavender
	{
	public:
		inline static std::filesystem::path Directory = {};

		#if defined(APP_DEBUG)
		inline static constexpr const WorkSpace::Configuration Config = WorkSpace::Configuration::Debug;
		#elif defined(APP_RELEASE)
		inline static constexpr const WorkSpace::Configuration Config = WorkSpace::Configuration::Release;
		#elif defined(APP_DIST)
		inline static constexpr const WorkSpace::Configuration Config = WorkSpace::Configuration::Dist;
		#else
		inline static constexpr const WorkSpace::Configuration Config = WorkSpace::Configuration::None;
		#endif

		#if defined(APP_PLATFORM_WINDOWS)
		inline static constexpr const WorkSpace::Platform Platform = WorkSpace::Platform::Windows;
		#elif defined(APP_PLATFORM_MACOS)
		inline static constexpr const WorkSpace::Platform Platform = WorkSpace::Platform::MacOS;
		#elif defined(APP_PLATFORM_LINUX)
		inline static constexpr const WorkSpace::Platform Platform = WorkSpace::Platform::Linux;
		#elif defined(APP_PLATFORM_ANDROID)
		inline static constexpr const WorkSpace::Platform Platform = WorkSpace::Platform::Android;
		#else
		inline static constexpr const WorkSpace::Platform Platform = WorkSpace::Platform::None;
		#endif

		// To be changed by developers
		inline static constexpr const char* VersionTitle = "Lavender Engine: Alpha 0.01";
	};

}