#include "stdafx.h"
#include "InstancingDemo.h"

void InstancingDemo::Initialize()
{
	Context::Get()->GetCamera()->Position(0, 0, -2);

	shader = new Shader(L"20_Instance.fxo");

	perFrame = new PerFrame(shader);

	material = new Material(shader);
	material->DiffuseMap(L"Box.png");

	for (UINT i = 0; i < Max_Instance_Count; i++)
	{
		transform[i] = new Transform(shader);

		//transform[i]->Position(Math::RandomVec3(-30, 30));
		//transform[i]->Scale(Math::RandomVec3(1.f, 2.5f));
		//transform[i]->RotationDegree(Math::RandomVec3(-180, 180));
		colors[i] = Color(0, 0, 0, 1);
		if (i % 3 == 0)
			colors[i] = Math::RandomColor4();

		worlds[i] = transform[i]->World();
	}

	CreateMesh();

	instanceWorldBuffer = new VertexBuffer(worlds, Max_Instance_Count,sizeof(Matrix),1);
	instaceColorBuffer = new VertexBuffer(colors, Max_Instance_Count, sizeof(Color), 2);

}

void InstancingDemo::Destroy()
{
	SafeDelete(shader);
	SafeDelete(material);
	SafeDelete(perFrame);

	SafeDelete(vertexBuffer);
	SafeDelete(indexBuffer);
	SafeDelete(instanceWorldBuffer);
	SafeDelete(instaceColorBuffer);

	for (UINT i = 0; i < Max_Instance_Count; i++)
		SafeDelete(transform[i]);


}

void InstancingDemo::Update()
{
	static Vector3 lightDirection = Vector3(-1, -1, 1);
	ImGui::SliderFloat3("Light Direction", lightDirection, -1, 1);
	shader->AsVector("LightDirection")->SetFloatVector(lightDirection);

	perFrame->Update();

}

void InstancingDemo::Render()
{
	perFrame->Render();

	material->Render();

	vertexBuffer->Render();
	indexBuffer->Render();
	instaceColorBuffer->Render();

	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	instanceWorldBuffer->Render();
	shader->DrawIndexedInstanced(0, 0, indices.size(),Max_Instance_Count); // 뒤의두개는 한개당그릴때참조할정점의개수,그릴개수
	//shader->DrawIndexedInstanced(0, 0, indices.size(), 5);
}

void InstancingDemo::CreateMesh()
{
	float w = 0.5f;
	float h = 0.5f;
	float d = 0.5f;

	//Front
	vertices.push_back(Mesh::VertexMesh(-w, -h, -d, 0, 1, 0, 0, -1));
	vertices.push_back(Mesh::VertexMesh(-w, +h, -d, 0, 0, 0, 0, -1));
	vertices.push_back(Mesh::VertexMesh(+w, +h, -d, 1, 0, 0, 0, -1));
	vertices.push_back(Mesh::VertexMesh(+w, -h, -d, 1, 1, 0, 0, -1));

	//Back
	vertices.push_back(Mesh::VertexMesh(-w, -h, +d, 1, 1, 0, 0, 1));
	vertices.push_back(Mesh::VertexMesh(+w, -h, +d, 0, 1, 0, 0, 1));
	vertices.push_back(Mesh::VertexMesh(+w, +h, +d, 0, 0, 0, 0, 1));
	vertices.push_back(Mesh::VertexMesh(-w, +h, +d, 1, 0, 0, 0, 1));

	//Top
	vertices.push_back(Mesh::VertexMesh(-w, +h, -d, 0, 1, 0, 1, 0));
	vertices.push_back(Mesh::VertexMesh(-w, +h, +d, 0, 0, 0, 1, 0));
	vertices.push_back(Mesh::VertexMesh(+w, +h, +d, 1, 0, 0, 1, 0));
	vertices.push_back(Mesh::VertexMesh(+w, +h, -d, 1, 1, 0, 1, 0));

	//Bottom
	vertices.push_back(Mesh::VertexMesh(-w, -h, -d, 1, 1, 0, -1, 0));
	vertices.push_back(Mesh::VertexMesh(+w, -h, -d, 0, 1, 0, -1, 0));
	vertices.push_back(Mesh::VertexMesh(+w, -h, +d, 0, 0, 0, -1, 0));
	vertices.push_back(Mesh::VertexMesh(-w, -h, +d, 1, 0, 0, -1, 0));

	//Left
	vertices.push_back(Mesh::VertexMesh(-w, -h, +d, 0, 1, -1, 0, 0));
	vertices.push_back(Mesh::VertexMesh(-w, +h, +d, 0, 0, -1, 0, 0));
	vertices.push_back(Mesh::VertexMesh(-w, +h, -d, 1, 0, -1, 0, 0));
	vertices.push_back(Mesh::VertexMesh(-w, -h, -d, 1, 1, -1, 0, 0));

	//Right
	vertices.push_back(Mesh::VertexMesh(+w, -h, -d, 0, 1, 1, 0, 0));
	vertices.push_back(Mesh::VertexMesh(+w, +h, -d, 0, 0, 1, 0, 0));
	vertices.push_back(Mesh::VertexMesh(+w, +h, +d, 1, 0, 1, 0, 0));
	vertices.push_back(Mesh::VertexMesh(+w, -h, +d, 1, 1, 1, 0, 0));

	indices =
	{
		0, 1, 2, 0, 2, 3,
		4, 5, 6, 4, 6, 7,
		8, 9, 10, 8, 10, 11,
		12, 13, 14, 12, 14, 15,
		16, 17, 18, 16, 18, 19,
		20, 21, 22, 20, 22, 23
	};

	vertexBuffer = new VertexBuffer(&vertices[0], vertices.size(), sizeof(Mesh::VertexMesh));
	indexBuffer = new IndexBuffer(&indices[0], indices.size());
}
