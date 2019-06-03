#include "vector3.h"

namespace gogaman
{
	namespace maths
	{
		vec3::vec3()
		{
			x = 0.0f;
			y = 0.0f;
			z = 0.0f;
		}

		vec3::vec3(float scalar)
		{
			this->x = scalar;
			this->y = scalar;
			this->z = scalar;
		}

		vec3::vec3(float x, float y, float z)
		{
			this->x = x;
			this->y = y;
			this->z = z;
		}

		vec3 &vec3::add(const vec3 &other)
		{
			x += other.x;
			y += other.y;
			z += other.z;
			return *this;
		}

		vec3 &vec3::subtract(const vec3 &other)
		{
			x -= other.x;
			y -= other.y;
			z -= other.z;
			return *this;
		}

		vec3 &vec3::multiply(const vec3 &other)
		{
			x *= other.x;
			y *= other.y;
			z *= other.z;
			return *this;
		}

		vec3 &vec3::divide(const vec3 &other)
		{
			x /= other.x;
			y /= other.y;
			z /= other.z;
			return *this;
		}

		vec3 operator+(vec3 left, const vec3 &right)
		{
			return left.add(right);
		}

		vec3 operator-(vec3 left, const vec3 &right)
		{
			return left.subtract(right);
		}

		vec3 operator*(vec3 left, const vec3 &right)
		{
			return left.multiply(right);
		}

		vec3 operator/(vec3 left, const vec3 &right)
		{
			return left.divide(right);
		}

		bool vec3::operator==(const vec3 &other) const
		{
			return x == other.x && y == other.y && z == other.z;
		}

		bool vec3::operator!=(const vec3 &other) const
		{
			return x != other.x || y != other.y || z != other.z;
		}

		vec3 &vec3::operator+=(const vec3 &other)
		{
			return add(other);
		}

		vec3 &vec3::operator-=(const vec3 &other)
		{
			return subtract(other);
		}

		vec3 &vec3::operator*=(const vec3 &other)
		{
			return multiply(other);
		}

		vec3 &vec3::operator/=(const vec3 &other)
		{
			return divide(other);
		}

		vec3 vec3::cross(const vec3 &other) const
		{
			return vec3(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
		}

		float vec3::dot(const vec3 &other) const
		{
			return x * other.x + y * other.y + z * other.z;
		}

		float vec3::magnitude() const
		{
			return sqrt(x * x + y * y + z * z);
		}

		float vec3::distance(const vec3 &other) const
		{
			float a = x - other.x;
			float b = y - other.y;
			float c = z - other.z;
			return sqrt(a * a + b * b + c * c);
		}

		vec3 vec3::normalize() const
		{
			float length = magnitude();
			return vec3(x / length, y / length, z / length);
		}

		std::ostream &operator<<(std::ostream &stream, const vec3 &vector)
		{
			stream << "vec3(" << vector.x << ", " << vector.y << ", " << vector.z << ")";
			return stream;
		}
	}
}