#include "lvpch.h"
#include "PhysicsBody.hpp"

#include "Lavender/Core/Logging.hpp"

namespace Lavender
{

	void BodyActivationListener::OnBodyActivated(const JPH::BodyID& inBodyID, JPH::uint64 inBodyUserData)
	{
		APP_LOG_TRACE("BodyActivationListener::OnBodyActivated()");
	}

	void BodyActivationListener::OnBodyDeactivated(const JPH::BodyID& inBodyID, JPH::uint64 inBodyUserData)
	{
		APP_LOG_TRACE("BodyActivationListener::OnBodyDeactivated()");
	}

}