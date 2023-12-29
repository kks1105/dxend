#include "stdafx.h"
#include "VertexLineColorDemo.h"

void VertexLineColorDemo::Initialize()
{
	shader = new Shader(L"02_Pass.fxo");

	vertices[0].Position = Vector3(0, 0, 0);
	vertices[0].Color = Color(1, 0, 0, 1);

	vertices[1].Position = Vector3(1, 0, 0);
	vertices[1].Color = Color(0, 1, 0, 1);

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.ByteWidth = sizeof(Vertex) * 2;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	
	D3D11_SUBRESOURCE_DATA subResource;
	subResource.pSysMem = vertices;

	Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &vertexBuffer));
}

void VertexLineColorDemo::Destroy()
{
	SafeRelease(vertexBuffer);
	SafeDelete(shader);
}

void VertexLineColorDemo::Update()
{
}

void VertexLineColorDemo::Render()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

	static int pass = 0;
	ImGui::InputInt("Pass", &pass);

	static Color color;
	ImGui::ColorEdit3("Color", color);
	shader->AsVector("BaseColor")->SetFloatVector(color);

	pass = Math::Clamp(pass, 0, 4);
	

	shader->Draw(0, pass, 2);
}
