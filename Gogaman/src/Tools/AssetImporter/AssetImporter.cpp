#include <fbxsdk.h>

#include <iostream>
#include <string>

namespace Gogaman
{
	static void ProcessNodeRecursive(FbxNode *node)
	{
		//do things

		for(int i = 0; i < node->GetChildCount(); i++)
			ProcessNodeRecursive(node->GetChild(i));
	}

	static void ImportFBX(const char *filepath)
	{
		FbxManager    *sdkManager = FbxManager::Create();
		FbxIOSettings *ioSettings = FbxIOSettings::Create(sdkManager, IOSROOT);
		sdkManager->SetIOSettings(ioSettings);
		FbxImporter   *importer   = FbxImporter::Create(sdkManager, "");
		FbxScene      *scene      = FbxScene::Create(sdkManager, "Scene");

		if(importer->Initialize(filepath, -1, sdkManager->GetIOSettings()))
		{
			std::cout << "Importing FBX file at location: " << filepath << std::endl;

			int fileVersionMajor, fileVersionMinor, fileVersionRevision;
			importer->GetFileVersion(fileVersionMajor, fileVersionMinor, fileVersionRevision);
			std::cout << "FBX version: " << fileVersionMajor << "." << fileVersionMinor << "." << fileVersionRevision << std::endl;
		}
		else
		{
			std::cerr << "Failed to import FBX file at location: " << filepath << std::endl;
			std::cerr << "Error: " << importer->GetStatus().GetErrorString() << std::endl;
			return;
		}

		importer->Import(scene);
		importer->Destroy();

		FbxGeometryConverter geometryConverter(sdkManager);
		geometryConverter.Triangulate(scene, true);

		FbxNode *sceneRootNode = scene->GetRootNode();
		if(sceneRootNode)
			ProcessNodeRecursive(sceneRootNode);
	}
}

int main(int argc, char *argv[])
{
	if(argc < 2)
		return -1;

	const char *filepath = argv[1];
	Gogaman::ImportFBX(filepath);

	return 0;
}