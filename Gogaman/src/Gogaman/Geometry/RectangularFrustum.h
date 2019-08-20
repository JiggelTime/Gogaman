#pragma once

#include "Plane.h"
#include "Sphere.h"

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
			planes[0].normal.x = matrix[3][0] + matrix[0][0];
			planes[0].normal.y = matrix[3][1] + matrix[0][1];
			planes[0].normal.z = matrix[3][2] + matrix[0][2];
			planes[0].distance = matrix[3][3] + matrix[0][3];
			//Right
			planes[1].normal.x = matrix[3][0] - matrix[0][0];
			planes[1].normal.y = matrix[3][1] - matrix[0][1];
			planes[1].normal.z = matrix[3][2] - matrix[0][2];
			planes[1].distance = matrix[3][3] - matrix[0][3];
			//Bottom
			planes[2].normal.x = matrix[3][0] + matrix[1][0];
			planes[2].normal.y = matrix[3][1] + matrix[1][1];
			planes[2].normal.z = matrix[3][2] + matrix[1][2];
			planes[2].distance = matrix[3][3] + matrix[1][3];
			//Top
			planes[3].normal.x = matrix[3][0] - matrix[1][0];
			planes[3].normal.y = matrix[3][1] - matrix[1][1];
			planes[3].normal.z = matrix[3][2] - matrix[1][2];
			planes[3].distance = matrix[3][3] - matrix[1][3];
			//Near
			planes[4].normal.x = matrix[3][0] + matrix[2][0];
			planes[4].normal.y = matrix[3][1] + matrix[2][1];
			planes[4].normal.z = matrix[3][2] + matrix[2][2];
			planes[4].distance = matrix[3][3] + matrix[2][3];
			//Far
			planes[5].normal.x = matrix[3][0] - matrix[2][0];
			planes[5].normal.y = matrix[3][1] - matrix[2][1];
			planes[5].normal.z = matrix[3][2] - matrix[2][2];
			planes[5].distance = matrix[3][3] - matrix[2][3];

			auto NormalizePlane = [](Plane &plane)
			{
				float normalLength = glm::length(plane.normal);
				plane.normal   /= normalLength;
				plane.distance /= normalLength;
			};

			for(int i = 0; i < 6; i++)
				NormalizePlane(planes[i]);
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

		Plane planes[6];
	};
}