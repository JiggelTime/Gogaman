//Atomic operations courtesy of Cyril Crassin and Simon Green.
//"Octree-Based Sparse Voxelization Using the GPU Hardware Rasterizer."
//OpenGL Insights 22, pp. 309-310.
//Available online https://www.seas.upenn.edu/~pcozzi/OpenGLInsights/OpenGLInsights-SparseVoxelization.pdf
//Modified by adding max iterations to avoid freezes and Timeout Detection and Recovery (TDR)

#version 450 core

layout(pixel_center_integer) in vec4 gl_FragCoord;
in vec3 voxelPositionFrag;
in vec3 normalFrag;
in vec2 texCoordsFrag;

uniform int voxelResolution;
uniform bool flagStatic;
uniform bool emissive;

uniform bool debug;

//Material textures
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_emissivity1;
//Voxel textures
layout(binding = 0, r32ui) volatile coherent uniform uimage3D voxelAlbedo;
layout(binding = 1, r32ui) volatile coherent uniform uimage3D voxelNormal;
layout(binding = 2, r8) uniform restrict image3D voxelStaticFlag;

bool isStatic(ivec3 voxelPos)
{ return imageLoad(voxelStaticFlag, voxelPos).r > 0.0001f; }
vec4 convertRGBA8ToVec4(uint val);
uint convertVec4ToRGBA8(vec4 val);
void imageAtomicRGBA8Average(layout(r32ui) volatile coherent uimage3D voxelTexture, ivec3 voxelPosition, vec4 value);

vec4  albedo             = texture(texture_diffuse1, texCoordsFrag);
vec3  encodedNormal      = normalFrag * 0.5f + vec3(0.5f);
float emissivity         = texture(texture_emissivity1, texCoordsFrag).r;
const float transparency = 0.0f;

void main()
{
	ivec3 voxelPos = ivec3(voxelPositionFrag);
	
	if(voxelPos.x >= voxelResolution || voxelPos.x < 0 || voxelPos.y >= voxelResolution || voxelPos.y < 0 || voxelPos.z >= voxelResolution || voxelPos.z < 0)
		return;

	bool toWrite = true;
	if(flagStatic == false)
		toWrite = !isStatic(voxelPos);
	
	//Write voxel information to textures
	if(toWrite && any(greaterThan(albedo, vec4(0.00001f))))
	{
		//For soft shadows to work better with transparent materials
		float alpha = pow(1 - transparency, 4);
		emissivity = emissive ? 1.0f : 0.0f;

		vec4 albedoData = vec4(albedo.rgb * alpha, 1.0f);
		vec4 normalData = vec4(encodedNormal,      1.0f);

		imageAtomicRGBA8Average(voxelAlbedo, voxelPos, albedoData);
		imageAtomicRGBA8Average(voxelNormal, voxelPos, normalData);
		
		if(flagStatic)
			imageStore(voxelStaticFlag, voxelPos, vec4(1.0f));
	}
}

vec4 convertRGBA8ToVec4(uint value)
{
    return vec4(float((value & 0x000000FF)), float((value & 0x0000FF00) >> 8U), float((value & 0x00FF0000) >> 16U), float((value & 0xFF000000) >> 24U));
}

uint convertVec4ToRGBA8(vec4 value)
{
    return (uint(value.w) & 0x000000FF) << 24U | (uint(value.z) & 0x000000FF) << 16U | (uint(value.y) & 0x000000FF) << 8U | (uint(value.x) & 0x000000FF);
}

void imageAtomicRGBA8Average(layout(r32ui) volatile coherent uimage3D voxelTexture, ivec3 voxelPosition, vec4 value)
{
    value.rgb *= 255.0f;
    uint newVal = convertVec4ToRGBA8(value);
    uint prevStoredVal = 0;
    uint curStoredVal;
    uint i = 0;
	const uint maxIterations = 100;

	//Loop until threads stop writing to voxel
    while((curStoredVal = imageAtomicCompSwap(voxelTexture, voxelPosition, prevStoredVal, newVal)) != prevStoredVal && i < maxIterations)
    {
        prevStoredVal = curStoredVal;
        vec4 rval = convertRGBA8ToVec4(curStoredVal);
		//Denormalize
        rval.rgb = (rval.rgb * rval.a);
		//Add
        vec4 curValF = rval + value;
		//Renormalize
        curValF.rgb /= curValF.a;

        newVal = convertVec4ToRGBA8(curValF);
        ++i;
    }
}