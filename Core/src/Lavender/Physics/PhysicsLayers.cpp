#include "lvpch.h"
#include "PhysicsLayers.hpp"

#include "Lavender/Core/Logging.hpp"

namespace Lavender
{

	bool ObjectLayerPairFilter::ShouldCollide(JPH::ObjectLayer obj1, JPH::ObjectLayer obj2) const
	{
		switch (obj1)
		{
		case Physics::Layers::NotMoving:	return obj2 == Physics::Layers::Moving;		// Non moving only collides with moving
		case Physics::Layers::Moving:		return true;								// Moving collides with everything

		default:
			APP_ASSERT(false, "Passed in object with unrecognized layer.");
			return false;
		}

		return false;
	}

	BroadPhaseLayerInterface::BroadPhaseLayerInterface()
	{
		m_ObjectToBroadPhase[Physics::Layers::NotMoving] =		Physics::BroadPhaseLayers::NotMoving;
		m_ObjectToBroadPhase[Physics::Layers::Moving] =			Physics::BroadPhaseLayers::Moving;
	}

	JPH::uint BroadPhaseLayerInterface::GetNumBroadPhaseLayers() const
	{
		return Physics::BroadPhaseLayers::AmountOfLayers;
	}

	JPH::BroadPhaseLayer BroadPhaseLayerInterface::GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const
	{
		APP_ASSERT(inLayer < Physics::Layers::AmountOfLayers, "Layer passed in has index out of physics layers range.");
		return m_ObjectToBroadPhase[inLayer];
	}

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
	const char* BroadPhaseLayerInterface::GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const
	{
		switch ((JPH::BroadPhaseLayer::Type)inLayer)
		{
		case (JPH::BroadPhaseLayer::Type)Physics::BroadPhaseLayers::NotMoving:		return "NotMoving";
		case (JPH::BroadPhaseLayer::Type)Physics::BroadPhaseLayers::Moving:			return "Moving";

		default:
			APP_ASSERT(false, "Invalid layer type passed in.");
			break;
		}

		return "Invalid Layer";
	}
#endif

	bool ObjectVsBroadPhaseLayerFilter::ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const
	{
		switch (inLayer1)
		{
		case Physics::Layers::NotMoving:		return inLayer2 == Physics::BroadPhaseLayers::Moving;
		case Physics::Layers::Moving:			return true;
		
		default:
			APP_ASSERT(false, "Passed in object with unrecognized layer.");
			return false;
		}

		return false;
	}

}