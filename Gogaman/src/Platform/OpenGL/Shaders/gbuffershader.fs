//Albedo encoding courtesy of Pavlos Mavridis and Georgios Papaioannou.
//"The Compact YCoCg Frame Buffer."
//Journal of Computer Graphics Techniques (JCGT), vol. 1, no. 1, 19-35, 2012.
//Available online http://jcgt.org/published/0001/01/02/

//Signed octahedron normal encoding courtesy of Zina H. Cigolle, Sam Donow, Daniel Evangelakos, Michael Mara, Morgan McGuire, and Quirin Meyer.
//"Survey of Efficient Representations for Independent Unit Vectors." 
//Journal of Computer Graphics Techniques (JCGT), vol. 3, no. 2, 1-30, 2014.
//Available online http://jcgt.org/published/0003/02/01/

#version 450 core

#define NORMAL_MAP_STRENGTH 1.0f
#define ROUGHNESS_MINIMUM 0.001f

layout(location = 0) out vec4  gPositionMetalness;
layout(location = 1) out vec2  gNormal;
layout(location = 2) out vec4  gAlbedoEmissivityRoughness;
layout(location = 3) out vec2  gVelocity;
layout(location = 4) out float gDepth;

in vec3 worldPositionFrag;
in vec4 clipPositionFrag;
in vec4 previousClipPositionFrag;
in vec2 texCoordsFrag;
in vec3 normalFrag;
in mat3 TBNFrag;

//Material textures
uniform sampler2D materialAlbedo;
uniform sampler2D materialNormal;
uniform sampler2D materialRoughness;
uniform sampler2D materialMetalness;
uniform sampler2D materialEmissivity;

uniform vec2 temporalJitter;
uniform vec2 previousTemporalJitter;
uniform bool normalMapping;

uniform bool debug;

vec2 EncodeAlbedo(vec3 color);
vec2 SignNotZero(vec2 v) { return vec2((v.x >= 0.0f) ? 1.0f : -1.0f, (v.y >= 0.0f) ? 1.0f : -1.0f); }
vec2 EncodeSignedOctahedronNormal(in vec3 v);

const mat3 RGBtoYCOCG = mat3(0.25f, 0.5f, -0.25f, 0.5f, 0.0f, 0.5f, 0.25f, -0.5f, -0.25f);

void main()
{
	//Calculate normal
		vec3 normal = normalFrag;
		if(normalMapping)
		{
			normal = texture(materialNormal, texCoordsFrag).xyz;
			normal = (normal.xyz * 2.0f) - 1.0f;
			normal = normalize(TBNFrag * normal.xyz);
			normal = mix(normal.xyz, normalFrag, 1.0f - NORMAL_MAP_STRENGTH);
		}
		normal.xy = EncodeSignedOctahedronNormal(normal);

	//Calculate Velocity
		vec2 firstPoint  = (((clipPositionFrag.xy) / clipPositionFrag.w) * 0.5f + 0.5f) - temporalJitter;
		vec2 secondPoint = (((previousClipPositionFrag.xy) / previousClipPositionFrag.w) * 0.5f + 0.5f) - previousTemporalJitter;
		vec2 velocity    = firstPoint - secondPoint;

	//Write information to multiple render targets
		//World space position
		gPositionMetalness.rgb        = worldPositionFrag;
		//Metalness
		//gPositionMetalness.a          = texture(texture_metalness1, texCoordsFrag).r;
		gPositionMetalness.a          = texture(materialMetalness, texCoordsFrag).x;
		//Normal
		gNormal                       = normal.xy;
		//Albedo
		//gAlbedoEmissivityRoughness.rg = EncodeAlbedo(texture(texture_diffuse1, texCoordsFrag).rgb);
		gAlbedoEmissivityRoughness.rg = EncodeAlbedo(texture(materialAlbedo, texCoordsFrag).rgb);
		//Emissivity
		//gAlbedoEmissivityRoughness.b  = emissive ? texture(texture_emissivity1, texCoordsFrag).r : 0.0f;
		gAlbedoEmissivityRoughness.b  = texture(materialEmissivity, texCoordsFrag).x;
		//Roughness
		//gAlbedoEmissivityRoughness.a  = max(texture(texture_roughness1,  texCoordsFrag).r, ROUGHNESS_MINIMUM);
		gAlbedoEmissivityRoughness.a  = max(texture(materialRoughness, texCoordsFrag).x, ROUGHNESS_MINIMUM);
		//Velocity
		gVelocity                     = velocity;
		//Depth
		gDepth                        = gl_FragCoord.z;
}

vec2 EncodeAlbedo(vec3 color)
{
	ivec2 coord = ivec2(gl_FragCoord.xy);
	//Convert albedo to YCoCg space
	vec3 YCoCg = RGBtoYCOCG * color;
	//Store YCo or YCg with checkerboard pattern
	vec2 encodedAlbedo = ((coord.x & 1) == (coord.y & 1)) ? YCoCg.rb : YCoCg.rg;
	return encodedAlbedo;
}

vec2 EncodeSignedOctahedronNormal(in vec3 v)
{
	//Project the sphere onto the octahedron, and then onto the xy plane 
	v.xy /= dot(abs(v), vec3(1.0f));
	//Reflect the folds of the lower hemisphere over the diagonals
	if(v.z <= 0.0f) v.xy = (1.0f - abs(v.yx)) * SignNotZero(v.xy);
	return v.xy;
}