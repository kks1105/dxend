#pragma once
#include "Systems/IExecute.h"

class CubeMapDemo : public IExecute
{
public:
	virtual void Initialize() override;
	virtual void Destroy() override;
	virtual void Update() override;
	virtual void PreRender() override {};
	virtual void Render() override;
	virtual void PostRender() override {};
	virtual void ResizeScreen() override {};

private:
	void CreateMesh();

private:
	Shader* shader = nullptr;
	
	MeshQuad* quad = nullptr;
	MeshPlane* plane = nullptr;

	MeshCube* cube = nullptr;
	MeshCylinder* cylinders[10];
	MeshSphere* spheres[10];

	Shader* cubeMapShader = nullptr;
	CubeMap* cubeMap = nullptr;

	CubeSky* sky = nullptr;
};