#pragma once

#include "Lavender/Core/Core.hpp"
#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Physics/PhysicsLayers.hpp"
#include "Lavender/Physics/ContactListener.hpp"
#include "Lavender/Physics/PhysicsBody.hpp"

#include <Jolt/Jolt.h>

#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSystem.h>

namespace Lavender
{

	struct PhysicsSpecification
	{
	public:
		const uint32_t MaxBodies = 1024;
		const uint32_t NumBodyMutexes = 0;
		const uint32_t MaxBodyPairs = 1024;
		const uint32_t MaxContactConstraints = 1024;
	};

	class PhysicsSystem
	{
	public:
		PhysicsSystem(const PhysicsSpecification& specs = {});
		~PhysicsSystem();

		void OnUpdate(float deltaTime);

		static Ref<PhysicsSystem> Create(const PhysicsSpecification& specs = {});

	private:
		PhysicsSpecification m_Specification = {};

		Unique<JPH::PhysicsSystem> m_System = nullptr;

		Unique<BroadPhaseLayerInterface> m_BroadPhaseLayerInterface = nullptr;
		Unique<ObjectVsBroadPhaseLayerFilter> m_ObjectVsBroadPhaseFilter = nullptr;
		Unique<ObjectLayerPairFilter> m_ObjectVsObjectFilter = nullptr;
		Unique<ContactListener> m_ContactListener = nullptr;
		Unique<BodyActivationListener> m_BodyActivationListener = nullptr;

		Unique<JPH::TempAllocatorImpl> m_Allocation = nullptr;
		Unique<JPH::JobSystemThreadPool> m_ThreadPool = nullptr;
	};

}