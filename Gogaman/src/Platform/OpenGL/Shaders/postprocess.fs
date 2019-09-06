#version 420 core

#define GAMMA 2.2f
//Tonemapping operator (UNCHARTED2 | REINHARD | LUMA_REINHARD | ACESFILMIC)
#define ACESFILMIC
#define GAMMA_CORRECTION 1
#define FILM_GRAIN 1
#define FILM_GRAIN_STRENGTH 2.0f

layout(location = 0) out vec4 FragColor;

in vec2 texCoordsFrag;

uniform float exposureNormalizationCoeffecient;
uniform float time;
uniform sampler2D hdrTexture;

uniform bool debug;

float Luminance(vec3 linearRGB) { return dot(linearRGB, vec3(0.3f, 0.59f, 0.11f)); }
vec3  GammaCorrection(vec3 color) { return pow(color, vec3(1.0f / GAMMA)); }
vec3  Uncharted2Tonemap(vec3 color);
vec3  ReinhardTonemap(vec3 color);
vec3  LumaReinhardTonemap(vec3 color);
vec3  FilmicTonemap(vec3 color);
vec3  ACESFilmicTonemap(vec3 color);
float Noise(vec3 seed);
//vec3  Overlay(vec3 value1, vec3 value2, float weight) { return mix(value1, mix(2.0f * value1 * value2, vec3(1.0f) - 2.0f * (vec3(1.0f) - value1) * (vec3(1.0f) - value2), step(vec3(0.5f), value1)), weight); }
//vec3  Screen(vec3 value1, vec3 value2, float weight) { return mix(value1, vec3(1.0f) - (vec3(1.0f) - value1) * (vec3(1.0f) - value2), weight); }

void main()
{
	vec3 renderedValue = texture(hdrTexture, texCoordsFrag).rgb;
	vec3 result        = vec3(0.0f);

	//Exposure normalization
		renderedValue *= exposureNormalizationCoeffecient;

	//Tonemapping
		#if defined(UNCHARTED2)
			result = Uncharted2Tonemap(renderedValue);

		#elif defined(REINHARD)
			result = ReinhardTonemap(renderedValue);

		#elif defined(LUMA_REINHARD)
			result = LumaReinhardTonemap(renderedValue);

		#elif defined(ACESFILMIC)
			result = ACESFilmicTonemap(renderedValue);
		#endif

	//Film grain
	//TODO: Vary amount of grain based on camera ISO (higher ISO = electronic signal amplified = more noise)
		#if FILM_GRAIN
			float grain = Noise(vec3(texCoordsFrag, time)) * FILM_GRAIN_STRENGTH;
			result += grain;
		#endif

	//Gamma correction (LDR to sRGB)
		#if GAMMA_CORRECTION
			result = GammaCorrection(result);
		#endif

	if(debug) result = renderedValue;
	FragColor = vec4(result, 1.0f);
}

//http://filmicworlds.com/blog/filmic-tonemapping-operators/
vec3 Uncharted2Tonemap(vec3 color)
{
	const float a = 0.15f;
	const float b = 0.50f;
	const float c = 0.10f;
	const float d = 0.20f;
	const float e = 0.02f;
	const float f = 0.30f;
	const float w = 11.2f;
	return ((color * (a * color + c * b) + d * e) / (color * (a * color + b) + d * f)) - e / f;
}

vec3 ReinhardTonemap(vec3 color)
{
	return color / (color + vec3(1.0f));
}

vec3 LumaReinhardTonemap(vec3 color)
{
	return color / (Luminance(color) + vec3(1.0f));
}

//https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
vec3 ACESFilmicTonemap(vec3 color)
{
    const float a = 2.51f;
    const float b = 0.03f;
    const float c = 2.43f;
    const float d = 0.59f;
    const float e = 0.14f;
    return clamp((color * (a * color + b)) / (color * (c * color + d) + e), 0.0f, 1.0f);
}

float Noise(vec3 seed)
{
    float x = (seed.x + 4.0f) * (seed.y + 4.0f) * (seed.z * 10.0f);
    return mod((mod(x, 13.0f) + 1.0f) * (mod(x, 123.0f) + 1.0f), 0.01f) - 0.005f;
}