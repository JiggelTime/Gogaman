#pragma once

#include "Plane.h"
#include "Sphere.h"
#include "BoundingBox3D.h"

#include <glm.hpp>

namespace Gogaman
{
	struct RectangularFrustum
	{
		inline RectangularFrustum()
		{}

		inline RectangularFrustum(const glm::mat4 &matrix)
		{
			//Left
			planes[0].normal.x = matrix[0][3] + matrix[0][0];
			planes[0].normal.y = matrix[1][3] + matrix[1][0];
			planes[0].normal.z = matrix[2][3] + matrix[2][0];
			planes[0].distance = matrix[3][3] + matrix[3][0];
			//Right
			planes[1].normal.x = matrix[0][3] - matrix[0][0];
			planes[1].normal.y = matrix[1][3] - matrix[1][0];
			planes[1].normal.z = matrix[2][3] - matrix[2][0];
			planes[1].distance = matrix[3][3] - matrix[3][0];
			//Bottom
			planes[2].normal.x = matrix[0][3] + matrix[0][1];
			planes[2].normal.y = matrix[1][3] + matrix[1][1];
			planes[2].normal.z = matrix[2][3] + matrix[2][1];
			planes[2].distance = matrix[3][3] + matrix[3][1];
			//Top
			planes[3].normal.x = matrix[0][3] - matrix[0][1];
			planes[3].normal.y = matrix[1][3] - matrix[1][1];
			planes[3].normal.z = matrix[2][3] - matrix[2][1];
			planes[3].distance = matrix[3][3] - matrix[3][1];
			//Near
			planes[4].normal.x = matrix[0][3] + matrix[0][2];
			planes[4].normal.y = matrix[1][3] + matrix[1][2];
			planes[4].normal.z = matrix[2][3] + matrix[2][2];
			planes[4].distance = matrix[3][3] + matrix[3][2];
			//Far
			planes[5].normal.x = matrix[0][3] - matrix[0][2];
			planes[5].normal.y = matrix[1][3] - matrix[1][2];
			planes[5].normal.z = matrix[2][3] - matrix[2][2];
			planes[5].distance = matrix[3][3] - matrix[3][2];

			for(int i = 0; i < 6; i++)
				planes[i].Normalize();
		}

		inline bool Intersects(const glm::vec3 &point) const
		{
			for(int i = 0; i < 6; i++)
			{
				if(planes[i].Distance(point) < 0.0f)
					return false;
			}

			return true;
		}

		inline bool Intersects(const Sphere &sphere) const
		{
			for(int i = 0; i < 6; i++)
			{
				if(planes[i].Distance(sphere.position) < -sphere.radius)
					return false;
			}

			return true;
		}

		inline bool Intersects(const BoundingBox3D &axisAlignedBoundingBox) const
		{
			for(int i = 0; i < 6; i++)
			{
				glm::vec3 positivePoint;
				if(planes[i].normal.x > 0.0f)
					positivePoint.x = axisAlignedBoundingBox.maximum.x;
				else
					positivePoint.x = axisAlignedBoundingBox.minimum.x;
				if(planes[i].normal.y > 0.0f)
					positivePoint.x = axisAlignedBoundingBox.maximum.y;
				else
					positivePoint.y = axisAlignedBoundingBox.minimum.y;
				if(planes[i].normal.z > 0.0f)
					positivePoint.x = axisAlignedBoundingBox.maximum.z;
				else
					positivePoint.z = axisAlignedBoundingBox.minimum.z;

				if(planes[i].Distance(positivePoint) < 0.0f)
					return false;
			}

			return true;
		}

		Plane planes[6];
	};
}