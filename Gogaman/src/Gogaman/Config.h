#pragma once

#include "Base.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

namespace Gogaman
{
	class Config
	{
	public:
		static Config &GetConfig()
		{
			static Config instance;
			return instance;
		}
	private:
		Config()  {}
		~Config() {}
	public:
		//Logging
			const std::string logFilepath = "Gogaman.log";

		//Window
			//Rendering resolution scale
			const float resScale = 1.0f;
			//Window resolution
			const int screenWidth  = 1920;
			const int screenHeight = 1080;
			//Vertical synchronization toggle
			const bool vSync = true;

		//Depth of field
			bool dof = false;
			bool dofKeyPressed = false;
			const float dofResScale = 1.0f;
			//Distance to the point of focus (meters)
			const float focalDistance = 10.0f;
			//Ratio of the aperture (f-stop)
			const float fStop = 0.01f;
			//Distance between the lens and the film (mm)
			const float focalLength = 50.0f;

		//Bloom
			//Bloom toggle
			bool bloom = true;
			bool bloomKeyPressed = false;
			//Bloom resolution scale
			const float bloomResScale = 0.25f;
			//Bloom effect blurriness
			const float bloomBlurAmount = 0.01f;
			//Controls strength of bloom effect
			const float bloomStrength = 0.1f;

		//Normal mapping toggle
			bool normalMapping = true;
			bool normalMappingKeyPressed = false;

		//Wireframe mode toggle
			bool wireframe = false;
			bool wireframeKeyPressed = false;

		//Temporal anti-aliasing
			bool taa = true;
			bool taaKeyPressed = false;
			//Less = sharper but more aliasing, higher = blurrier but less aliasing
			const float temporalJitterSpread = 1.0f;

		//Voxel cone traced global illumination
			//GI resolution scale
			const float giResScale = 0.25f * resScale;
			//Toggle indirect lighting upscaling
			bool giUpscaling = true;
			bool giUpscalingKeyPressed = false;
			//Automatic voxelization toggle
			bool autoVoxelize = true;
			bool autoVoxelizeKeyPressed = false;
			//Width, length, and height of voxel volume
			const int voxelResolution = 64;
			//Number of voxel GI compute shader work groups
			const int voxelComputeWorkGroups = static_cast<const int>(glm::ceil(voxelResolution / 8.0f));
			float voxelGridSize = 1.7f;
			glm::vec3 voxelGridPos = glm::vec3(0.0f, 0.0f, 0.0f);
			//Frequency of re-voxelization (every N frames)
			const int voxelizationFrequency = 1;

		//Debug
			int renderMode = 0;
			bool debug = false;
			bool debugKeyPressed = false;
			bool debug2 = false;
			bool debug2KeyPressed = false;
	};
}

#define GM_CONFIG Gogaman::Config::GetConfig()