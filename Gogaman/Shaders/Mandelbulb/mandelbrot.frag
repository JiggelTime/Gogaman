#version 450

#define EPSILON 0.000001f

#define MAX_STEPS 200

layout(location = 0) in vec2 p_UV;

layout(location = 0) out vec4 resultValue;

layout(set = 0, binding = 0) uniform Data
{
	vec2 resolution;
};

float Sphere(vec3 position, float radius)
{
	return length(position) - radius;
}

float Map(vec3 position)
{
	float distance = Sphere(position, 0.25f);
	
	return distance;
}

vec3 SphereTrace(vec3 ro, vec3 rd)
{
	float depth = 0.0f;
	for(int i = 0; i < MAX_STEPS; i++)
	{
		vec3 position = ro + rd * depth;
		
		float distance = Map(position);
		if(distance < EPSILON)
			break;
		
		depth += distance;
		
		if(depth > 100.0f)
			break;
	}
	
	if(depth < 100.0f)
		return vec3(1.0f);
	
	return vec3(0.0f, 0.075f, 0.15f);
}

void main()
{
	vec2 uv = (2.0f * (p_UV * resolution) - resolution) / resolution.y;
	
	vec3 ro = vec3(0.0f, 0.0f, 1.25f);
	vec3 rd = vec3(uv, -1.5f);
	
	vec3 color = SphereTrace(ro, rd);

	resultValue = vec4(color, 1.0f);
}