#pragma once

#include "Lavender/Core/Core.hpp"
#include "Lavender/Utils/Utils.hpp"

#include <Jolt/Jolt.h>

#include <Jolt/Physics/Body/BodyActivationListener.h>

namespace Lavender
{

	enum class BodyType : uint8_t
	{
		None = 0, Static, Dynamic, Kinematic
	};

	class BodyActivationListener : public JPH::BodyActivationListener
	{
	public:
		// TODO: Add callbacks

		void OnBodyActivated(const JPH::BodyID& inBodyID, JPH::uint64 inBodyUserData) override;
		void OnBodyDeactivated(const JPH::BodyID& inBodyID, JPH::uint64 inBodyUserData) override;
	};

	class Body
	{
	public:

	};

}