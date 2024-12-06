#pragma once

#include "Lavender/Core/Core.hpp"
#include "Lavender/Utils/Utils.hpp"

#include <Jolt/Jolt.h>

#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhase.h>

namespace Lavender
{

	namespace Physics::Layers
	{
		static constexpr JPH::ObjectLayer		NotMoving = 0;
		static constexpr JPH::ObjectLayer		Moving = 1;
		static constexpr JPH::ObjectLayer		AmountOfLayers = 2;
	};

	namespace Physics::BroadPhaseLayers
	{
		static constexpr JPH::BroadPhaseLayer	NotMoving(0);
		static constexpr JPH::BroadPhaseLayer	Moving(1);
		static constexpr JPH::uint				AmountOfLayers(2);
	};



	class ObjectLayerPairFilter : public JPH::ObjectLayerPairFilter
	{
	public:
		bool ShouldCollide(JPH::ObjectLayer obj1, JPH::ObjectLayer obj2) const override;
	};



	class BroadPhaseLayerInterface final : public JPH::BroadPhaseLayerInterface
	{
	public:
		BroadPhaseLayerInterface();

		JPH::uint GetNumBroadPhaseLayers() const override;
		JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override;

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
		const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override;
#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

	private:
		JPH::BroadPhaseLayer m_ObjectToBroadPhase[Physics::Layers::AmountOfLayers] = { };
	};



	class ObjectVsBroadPhaseLayerFilter : public JPH::ObjectVsBroadPhaseLayerFilter
	{
	public:
		bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const override;
	};

}