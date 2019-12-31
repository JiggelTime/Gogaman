#version 450

layout(location = 0) in vec3 v_Position;
layout(location = 1) in vec2 v_UV;
layout(location = 2) in vec3 v_Normal;
layout(location = 3) in vec3 v_Tangent;

layout(location = 0) out vec2 p_UV;

layout(set = 0, binding = 0) uniform CameraMatrices
{
	mat4 cameraView;
	mat4 cameraProjection;
	mat4 cameraViewProjection;
} cameraMatrices;

layout(set = 2, binding = 0) uniform MeshData
{
	mat4 meshMatrix;
} meshData;

void main()
{
	gl_Position = cameraMatrices.cameraViewProjection * vec4(vec3(meshData.meshMatrix * vec4(v_Position, 1.0f)), 1.0f);
	
	p_UV = v_UV;
}