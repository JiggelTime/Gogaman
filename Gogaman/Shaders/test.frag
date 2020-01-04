#version 450

layout(location = 0) in vec3 p_Position;
layout(location = 1) in vec2 p_UV;
layout(location = 2) in vec3 p_Normal;

layout(location = 0) out vec4 resultValue;

layout(set = 0, binding = 0) uniform CameraData
{
	mat4 cameraView;
	mat4 cameraProjection;
	mat4 cameraViewProjection;
	vec3 cameraPosition;
};

layout(set = 0, binding = 1) uniform sampler pointSampler;
layout(set = 0, binding = 2) uniform sampler anisotropicSampler;

layout(set = 1, binding = 0) uniform texture2D albedoTexture;
layout(set = 1, binding = 1) uniform texture2D normalTexture;
layout(set = 1, binding = 2) uniform texture2D roughnessTexture;
layout(set = 1, binding = 3) uniform texture2D metalnessTexture;
layout(set = 1, binding = 4) uniform texture2D emissivityTexture;

void main()
{
	const vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
	const vec3 lightDirection = normalize(vec3(-0.8f, 0.6f, -0.5f));
	
	vec3 normal = normalize(p_Normal);
	
	vec3 viewDirection = normalize(cameraPosition - p_Position);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	vec3 specular = lightColor * pow(max(dot(viewDirection, reflectionDirection), 0.0f), 32) * 0.8f;

	vec3 diffuse = max(dot(normal, lightDirection), 0.0f) * lightColor;

	vec3 albedo = texture(sampler2D(albedoTexture, anisotropicSampler), p_UV).rgb;
	
	vec3 irradiance = albedo * (diffuse + specular);
	//irradiance = albedo * 1.0f;
	resultValue = vec4(irradiance, 1.0f);
}