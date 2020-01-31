#version 450

#define MAX_ITERATION_COUNT 256.0f

layout(location = 0) in vec2 p_UV;

layout(location = 0) out vec4 resultValue;

layout(set = 0, binding = 0) uniform Data
{
	vec3 positionAndZoom;
};

vec3 GenerateColor(float value)
{
	float red   = sin(value) * 0.51f + 0.5f;
	float green = sin(value * 2.14f) * 0.5f + 0.5f;
	float blue  = sin(value * 0.47f) * 0.5f + 0.5f;
	return vec3(red, green, blue);
}

vec3 Mandelbrot(vec2 c)
{
	vec2 z = vec2(0.0f);

	const float radius  = 20.0f;
	const float radius2 = radius * radius;
	
	float i;
	for(i = 0.0f; i < MAX_ITERATION_COUNT ; i++)
	{
		z = vec2(z.x * z.x - z.y * z.y, 2.0f * z.x * z.y) + c;
		if(length(z) > radius)
			break;
	}

	float distance = length(z);

	i -= log2(log(distance) / log(radius));

	float value = i / MAX_ITERATION_COUNT;
	vec3 color = GenerateColor(sin(value) * 19.3f);
	return color;
}

void main()
{
	float zoom = positionAndZoom.z;
	vec2  pos  = positionAndZoom.xy;

	vec2 c = 1.333f * (p_UV - 0.5f) * zoom - pos;

	vec3 color = Mandelbrot(c);

	resultValue = vec4(color, 1.0f);
}