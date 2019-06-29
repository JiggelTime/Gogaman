#version 450 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec3 worldPositionGeom;
out vec3 normalGeom;
out vec2 texCoordsGeom;

uniform mat4 M;

void main()
{
	gl_Position = M * vec4(aPos, 1.0f);
	worldPositionGeom = gl_Position.xyz;
	mat3 normalMatrix = transpose(inverse(mat3(M)));
	normalGeom = normalize(normalMatrix * aNormal);
	texCoordsGeom = aTexCoords;
}