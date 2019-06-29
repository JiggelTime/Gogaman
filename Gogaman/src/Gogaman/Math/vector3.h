#pragma once

#include <iostream>

namespace gogaman
{
	namespace maths
	{
		struct vec3
		{
			float x, y, z;

			vec3();
			vec3(float scalar);
			vec3(float x, float y, float z);

			vec3 &add     (const vec3 &other);
			vec3 &subtract(const vec3 &other);
			vec3 &multiply(const vec3 &other);
			vec3 &divide  (const vec3 &other);

			friend vec3 operator+(vec3 left, const vec3 &right);
			friend vec3 operator-(vec3 left, const vec3 &right);
			friend vec3 operator*(vec3 left, const vec3 &right);
			friend vec3 operator/(vec3 left, const vec3 &right);

			bool operator==(const vec3 &other) const;
			bool operator!=(const vec3 &other) const;
			
			vec3 &operator+=(const vec3 &other);
			vec3 &operator-=(const vec3 &other);
			vec3 &operator*=(const vec3 &other);
			vec3 &operator/=(const vec3 &other);

			vec3  cross(const vec3 &other)    const;
			float dot(const   vec3 &other)    const;
			float magnitude()                 const;
			float distance(const vec3 &other) const;
			vec3  normalize()                 const;

			friend std::ostream &operator<<(std::ostream &stream, const vec3 &vector);
		};
	}
}