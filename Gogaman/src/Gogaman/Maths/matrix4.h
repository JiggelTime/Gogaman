#pragma once

#include "maths_func.h"
#include "vector3.h"
#include "vector4.h"

namespace gogaman
{
	namespace maths
	{
		struct mat4
		{
			union
			{
				float elements[4 * 4];
				vec4  rows[4];
			};

			mat4();
			mat4(float diagonal);
			mat4(const vec4 &row0, const vec4 &row1, const vec4 &row2, const vec4 &row3);

			static mat4 identity();

			mat4 &multiply       (const mat4 &other);
			friend mat4 operator*(mat4 left, const mat4 &right);
			mat4 &operator*=     (const mat4 &other);

			mat4& invert();

			static mat4 orthographic(float left, float right, float bottom, float top, float near, float far);
			static mat4 perspective (float fov, float aspectRatio, float near, float far);
			static mat4 lookAt      (const vec3 &position, const vec3 &target, const vec3 &up);

			static mat4 translate(const vec3 &translation);
			static mat4 rotate   (float degrees, const vec3 &axis);
			static mat4 scale    (const vec3 &scale);
			static mat4 invert   (const mat4 &matrix);
			static mat4 transpose(const mat4 &matrix);
		};
	}
}