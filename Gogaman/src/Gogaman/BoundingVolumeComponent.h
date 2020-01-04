#pragma once

#include "Gogaman/Utilities/Geometry/Sphere.h"
#include "Gogaman/Utilities/Geometry/BoundingBox3D.h"

namespace Gogaman
{
	struct BoundingVolumeComponent
	{
		Sphere boundingSphere;
		Sphere worldSpaceBoundingSphere;

		BoundingBox3D axisAlignedBoundingBox;
		BoundingBox3D worldSpaceAxisAlignedBoundingBox;
	};
}