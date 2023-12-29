#pragma once
#include "Systems/IExecute.h"

#define Max_Instance_Count 5000

class InstancingDemo : public IExecute
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
	Shader* shader;
	Material* material;

	vector<Mesh::VertexMesh> vertices;
	vector<UINT> indices;

	VertexBuffer* vertexBuffer;
	IndexBuffer* indexBuffer;

	VertexBuffer* instanceWorldBuffer;
	VertexBuffer* instaceColorBuffer;

	PerFrame* perFrame;
	Transform* transform[Max_Instance_Count];
	Matrix worlds[Max_Instance_Count];
	Color colors[Max_Instance_Count];

};