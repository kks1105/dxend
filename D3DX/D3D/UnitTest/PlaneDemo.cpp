#include "stdafx.h"
#include "PlaneDemo.h"

void PlaneDemo::Initialize()
{
	shader = new Shader(L"03_Mesh.fxo");

	//Polygon
	{
		vertices[0].Position = Vector3(-0.5f, -0.5f, +0.0f);
		vertices[1].Position = Vector3(-0.5f, +0.5f, +0.0f);
		vertices[2].Position = Vector3(+0.5f, -0.5f, +0.0f);
		vertices[3].Position = Vector3(+0.5f, -0.5f, +0.0f);
		vertices[4].Position = Vector3(-0.5f, +0.5f, +0.0f);
		vertices[5].Position = Vector3(+0.5f, +0.5f, +0.0f);

		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.ByteWidth = sizeof(Vertex) * ARRAYSIZE(vertices);
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subResource = { 0 };
		subResource.pSysMem = vertices;

		Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &vertexBuffer));
	}
}

void PlaneDemo::Destroy()
{
	SafeDelete(shader);
	SafeRelease(vertexBuffer);
}

void PlaneDemo::Update()
{
}

void PlaneDemo::Render()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	static bool bWireFrame;
	ImGui::Checkbox("WireFrame", &bWireFrame);

	shader->Draw(0, bWireFrame ? 1 : 0, 6);
}
