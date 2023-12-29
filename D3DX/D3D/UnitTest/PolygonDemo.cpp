#include "stdafx.h"
#include "PolygonDemo.h"

void PolygonDemo::Initialize()
{
	shader = new Shader(L"02_Pass.fxo");

	//Polygon - 1
	{
		vertices[0].Position = Vector3(-0.5f, +0.0f, +0.0f);
		vertices[1].Position = Vector3(+0.0f, +0.5f, +0.0f);
		vertices[2].Position = Vector3(+0.5f, +0.0f, +0.0f);

		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.ByteWidth = sizeof(Vertex) * ARRAYSIZE(vertices);
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subResource = { 0 };
		subResource.pSysMem = vertices;

		Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &vertexBuffer));
	}

	//Polygon - 2
	{
		vertices2[0].Position = Vector3(-0.5f, +0.0f - 0.5f, +0.0f);
		vertices2[1].Position = Vector3(+0.0f, +0.5f - 0.5f, +0.0f);
		vertices2[2].Position = Vector3(+0.5f, +0.0f - 0.5f, +0.0f);

		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.ByteWidth = sizeof(Vertex) * ARRAYSIZE(vertices2);
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subResource = { 0 };
		subResource.pSysMem = vertices2;

		Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &vertexBuffer2));
	}
}

void PolygonDemo::Destroy()
{
	SafeDelete(shader);
	SafeRelease(vertexBuffer);
	SafeRelease(vertexBuffer2);
}

void PolygonDemo::Update()
{
}

void PolygonDemo::Render()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	shader->Draw(0, 0, 3);

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer2, &stride, &offset);
	shader->Draw(0, 1, 3);
}
