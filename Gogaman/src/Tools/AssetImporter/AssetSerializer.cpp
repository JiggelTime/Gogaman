#pragma once

#include "FlexData.h"
#include "FBX_Importer.h"

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		//std::cerr << "Failed to run: too few arguments" << std::endl;
		//return -1;
	}

	//const char *importFilepath = argv[1];
	//const char *exportFilepath = argv[2];
	const char *importFilepath = "D:/dev/testScene/testScene.fbx";
	const char *exportFilepath = "D:/dev/testScene/testScene.flex";

	//Import FBX file
	Gogaman::Tools::FBX_Importer importer;
	FlexData::FlexData testFlexData = importer.Import(importFilepath);

	//FlexData::PrintFlexData(testFlexData, true);

	//Export FlexData
	FlexData::ExportFlexData(exportFilepath, testFlexData);

	//Import FlexData
	FlexData::FlexData testFlexData2 = FlexData::ImportFlexData(exportFilepath);

	FlexData::PrintFlexData(testFlexData2);

	return 0;
}