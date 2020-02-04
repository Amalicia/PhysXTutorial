#pragma once
#include "PhysicsEngine.h"
#include <iostream>
#include <iomanip>

namespace PhysicsEngine {

	class CompoundObject : public DynamicActor
	{
	public:
		CompoundObject(const PxTransform& pose = PxTransform(PxIdentity), PxVec3 dimensions = PxVec3(.5f, .5f, .5f), PxReal density = 1.f)
			: DynamicActor(pose)
		{
			CreateShape(PxBoxGeometry(dimensions), density);
		}
	};
};

