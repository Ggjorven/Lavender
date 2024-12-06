#include "lvpch.h"
#include "ContactListener.hpp"

#include "Lavender/Core/Logging.hpp"

namespace Lavender
{

	ContactListener::ContactListener(ContactCallbackFn callback)
		: m_CallBack(callback)
	{
	}

	ContactListener::~ContactListener()
	{
	}

	JPH::ValidateResult ContactListener::OnContactValidate(const JPH::Body& inBody1, const JPH::Body& inBody2, JPH::RVec3Arg inBaseOffset, const JPH::CollideShapeResult& inCollisionResult)
	{
		APP_LOG_TRACE("ContactListener::OnContactValidate()");

		// Allows you to ignore a contact before it is created (using layers to not make objects collide is cheaper!)
		return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
	}

	void ContactListener::OnContactAdded(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings)
	{
		APP_LOG_TRACE("ContactListener::OnContactAdded()");
	}

	void ContactListener::OnContactPersisted(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings)
	{
		APP_LOG_TRACE("ContactListener::OnContactPersisted()");
	}

	void ContactListener::OnContactRemoved(const JPH::SubShapeIDPair& inSubShapePair)
	{
		APP_LOG_TRACE("ContactListener::OnContactRemoved()");
	}

}