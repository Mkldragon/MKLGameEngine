#pragma once
#include "sge_core.h"
#include "LoadShader.h"

namespace sge
{

}
void main()
{
	sge::LoadShader loadShader;
	sge::String file = loadShader.getExecutableFilename();
	sge::String path = sge::FilePath::getDir(file);
	SGE_LOG("dir : {} ", path);
	path.append("/../../../../../Data");

	auto* proj =  sge::ProjectSettings::instance();
	proj->setProjectRoot(path);

	auto dir = loadShader.getCurrentDir();
	char fileName[100];
	scanf("%100s", fileName);
	sge::String filePath = "Assets/Shader/";
	filePath.append(fileName);

	SGE_LOG("dir : {} ", filePath);
	loadShader.loadShaderFile(filePath);


	int a;
	scanf("%d", &a);
	return;
}

