#include "lvpch.h"
#include "PhysicsSystem.hpp"

#include "Lavender/Core/Logging.hpp"

#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>

namespace Lavender
{

	////////////////////////////////////////////////////////////////////
	// Util Functions
	////////////////////////////////////////////////////////////////////
	static void TraceImpl(const char* inFMT, ...)
	{
		va_list list = {};
		va_start(list, inFMT);
		char buffer[1024] = {};
		vsnprintf(buffer, sizeof(buffer), inFMT, list);
		va_end(list);

		APP_LOG_TRACE("{0}", buffer);
	}

#ifdef JPH_ENABLE_ASSERTS
	static bool AssertFailedImpl(const char* inExpression, const char* inMessage, const char* inFile, JPH::uint inLine)
	{
		APP_ASSERT(false, "{0}: {1}: ({2}) -> {3}", inFile, inLine, inExpression, (inMessage != nullptr ? inMessage : ""));
		return true;
	};
#endif // JPH_ENABLE_ASSERTS

	////////////////////////////////////////////////////////////////////
	// Core Functions
	////////////////////////////////////////////////////////////////////
	static uint8_t s_PhysicsInstances = 0;

	PhysicsSystem::PhysicsSystem(const PhysicsSpecification& specs)
		: m_Specification(specs)
	{
		// Initialize Jolt
		if (s_PhysicsInstances++ == 0)
		{
			JPH::RegisterDefaultAllocator();

			JPH::Trace = TraceImpl;
#ifdef JPH_ENABLE_ASSERTS
			JPH::AssertFailed = AssertFailedImpl;
#endif

			JPH::Factory::sInstance = new JPH::Factory();
			
			JPH::RegisterTypes();
		}

		// Initialize variables
		{
			m_Allocation = UniqueHelper::Create<JPH::TempAllocatorImpl>(10 * 1024 * 1024/*10 MB*/);
			m_ThreadPool = UniqueHelper::Create<JPH::JobSystemThreadPool>(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, std::thread::hardware_concurrency() - 1);
			
			m_BroadPhaseLayerInterface = UniqueHelper::Create<BroadPhaseLayerInterface>();
			m_ObjectVsBroadPhaseFilter = UniqueHelper::Create<ObjectVsBroadPhaseLayerFilter>();
			m_ObjectVsObjectFilter = UniqueHelper::Create<ObjectLayerPairFilter>();

			m_System = UniqueHelper::Create<JPH::PhysicsSystem>();
			m_System->Init(m_Specification.MaxBodies, m_Specification.NumBodyMutexes, m_Specification.MaxBodyPairs, m_Specification.MaxContactConstraints, *m_BroadPhaseLayerInterface, *m_ObjectVsBroadPhaseFilter, *m_ObjectVsObjectFilter);
		
			m_ContactListener = UniqueHelper::Create<ContactListener>(nullptr);
			m_System->SetContactListener(m_ContactListener.get());

			m_BodyActivationListener = UniqueHelper::Create<BodyActivationListener>();
			m_System->SetBodyActivationListener(m_BodyActivationListener.get());
		}
	}

	PhysicsSystem::~PhysicsSystem()
	{
		// Destroy Jolt
		if (--s_PhysicsInstances == 0)
		{
			JPH::UnregisterTypes();

			delete JPH::Factory::sInstance;
			JPH::Factory::sInstance = nullptr;
		}
	}

	void PhysicsSystem::OnUpdate(float deltaTime)
	{
	}

	Ref<PhysicsSystem> PhysicsSystem::Create(const PhysicsSpecification& specs)
	{
		return RefHelper::Create<PhysicsSystem>(specs);
	}

}