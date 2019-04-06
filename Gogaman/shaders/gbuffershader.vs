#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;

out vec3 worldPositionFrag;
out vec4 clipPositionFrag;
out vec4 previousClipPositionFrag;
out vec2 texCoordsFrag;
out vec3 normalFrag;
out mat3 TBNFrag;

//Model matrix
uniform mat4 M;
//Previous model matrix
uniform mat4 previousM;
//View projection matrix
uniform mat4 VP;
//Previous view projection matrix
uniform mat4 previousVP;

void main()
{
	worldPositionFrag        = vec3(M * vec4(aPos, 1.0f));
	clipPositionFrag         = VP * vec4(worldPositionFrag, 1.0f);
	previousClipPositionFrag = previousVP * vec4(vec3(previousM * vec4(aPos, 1.0f)), 1.0f);
	texCoordsFrag            = aTexCoords;

	mat3 normalMatrix        = transpose(inverse(mat3(M)));
	normalFrag               = normalize(normalMatrix * aNormal);
	vec3 tangent             = normalize(normalMatrix * aTangent);
	tangent                  = normalize(tangent - dot(tangent, normalFrag) * normalFrag);
	vec3 bitangent           = cross(normalFrag, tangent);
	TBNFrag                  = mat3(tangent, bitangent, normalFrag);

    gl_Position              = clipPositionFrag;
}