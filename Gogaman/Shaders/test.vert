#version 450

layout(location = 0) in vec3 v_Position;
layout(location = 1) in vec2 v_UV;
layout(location = 2) in vec3 v_Normal;
layout(location = 3) in vec3 v_Tangent;

layout(location = 0) out vec3 p_Position;
layout(location = 1) out vec2 p_UV;
layout(location = 2) out vec3 p_Normal;

layout(set = 0, binding = 0) uniform CameraData
{
	mat4 cameraView;
	mat4 cameraProjection;
	mat4 cameraViewProjection;
	vec3 cameraPosition;
};

layout(set = 2, binding = 0) uniform MeshData
{
	mat4 meshMatrix;
};

void main()
{
	p_Position = vec3(meshMatrix * vec4(v_Position, 1.0f));
	gl_Position = cameraViewProjection * vec4(p_Position, 1.0f);

	p_UV = v_UV;

	p_Normal = mat3(transpose(inverse(meshMatrix))) * v_Normal;
}