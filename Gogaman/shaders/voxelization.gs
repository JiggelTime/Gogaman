#version 450 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 worldPositionGeom[];
in vec3 normalGeom[];
in vec2 texCoordsGeom[];

out vec3 voxelPositionFrag;
out vec3 normalFrag;
out vec2 texCoordsFrag;

uniform int   voxelResolution;
uniform float voxelGridSizeInverse;
uniform vec3  voxelGridPos;
uniform mat4  VP;

vec3 worldToVoxelSpace(vec3 worldPos);
int computeProjectionAxis();

void main()
{
	int projectionAxis = computeProjectionAxis();

	for(uint i = 0; i < 3; ++i)
	{
		vec3 worldPosition = worldPositionGeom[i];

		voxelPositionFrag = worldToVoxelSpace(worldPosition);
		normalFrag = normalGeom[i];
		texCoordsFrag = texCoordsGeom[i];

		if(projectionAxis == 1)
			gl_Position = VP * vec4(worldPosition.y, worldPosition.z, 0.0f, 1.0f);
		else if(projectionAxis == 2)
			gl_Position = VP * vec4(worldPosition.x, worldPosition.z, 0.0f, 1.0f);
		else
			gl_Position = VP * vec4(worldPosition.x, worldPosition.y, 0.0f, 1.0f);

		EmitVertex();
	}

	EndPrimitive();
}

vec3 worldToVoxelSpace(vec3 worldPos)
{
	//World to voxel space
	vec3 voxelPos = vec3((worldPos - voxelGridPos) * voxelGridSizeInverse);
	//[-1, 1] to [0, 1]
	voxelPos = (voxelPos * 0.5f) + vec3(0.5f);
	voxelPos *= voxelResolution;
	return voxelPos;
}

int computeProjectionAxis()
{
	vec3 p1 = worldPositionGeom[1] - worldPositionGeom[0];
	vec3 p2 = worldPositionGeom[2] - worldPositionGeom[0];
	vec3 faceNormal = cross(p1, p2);
	float nDotX = abs(faceNormal.x);
	float nDotY = abs(faceNormal.y);
	float nDotZ = abs(faceNormal.z);

	if(nDotX > nDotY && nDotX > nDotZ)
		return 1;
	else if(nDotY > nDotX && nDotY > nDotZ)
		return 2;
	else
		return 3;
}