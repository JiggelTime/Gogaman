#pragma once

#include "Gogaman/Geometry/Sphere.h"
#include "Gogaman/Geometry/BoundingBox3D.h"

namespace Gogaman
{
	struct BoundingVolumeComponent
	{
		Sphere        boundingSphere;
		Sphere        worldSpaceBoundingSphere;

		BoundingBox3D axisAlignedBoundingBox;
		BoundingBox3D worldSpaceAxisAlignedBoundingBox;
	};
}