#pragma once

#include "Lavender/Core/Core.hpp"
#include "Lavender/Utils/Utils.hpp"
#include "Lavender/Utils/UUID.hpp"

#include <Jolt/Jolt.h>

#include <Jolt/Physics/PhysicsSystem.h>

namespace Lavender
{

	enum class ContactType : uint8_t
	{
		None = 0, CollisionBegin, CollisionPersisted, CollisionEnd
	};

	using ContactCallbackFn = std::function<void(ContactType, const UUID&, const UUID&)>;
	
	class ContactListener : public JPH::ContactListener
	{
	public:
		ContactListener(ContactCallbackFn callback);
		~ContactListener();

		JPH::ValidateResult OnContactValidate(const JPH::Body& inBody1, const JPH::Body& inBody2, JPH::RVec3Arg inBaseOffset, const JPH::CollideShapeResult& inCollisionResult) override;

		void OnContactAdded(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings) override;
		void OnContactPersisted(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings) override;
		void OnContactRemoved(const JPH::SubShapeIDPair& inSubShapePair) override;

	private:
		ContactCallbackFn m_CallBack = nullptr;
	};

}