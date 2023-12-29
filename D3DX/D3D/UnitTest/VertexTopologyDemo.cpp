#include "stdafx.h"
#include "VertexTopologyDemo.h"

void VertexTopologyDemo::Initialize()
{
	shader = new Shader(L"01_Line.fxo");

	//Line - 0
	vertices[0].Position = Vector3(0, 0, 0);
	vertices[1].Position = Vector3(1, 0, 0);

	//Line - 1
	vertices[2].Position = Vector3(0, 0.5f, 0);
	vertices[3].Position = Vector3(1, 0.5f, 0);

	//Line - 2
	vertices[4].Position = Vector3(0, -0.5f, 0);
	vertices[5].Position = Vector3(1, -0.5f, 0);

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.ByteWidth = sizeof(Vertex) * 6;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	
	D3D11_SUBRESOURCE_DATA subResource;
	subResource.pSysMem = vertices;

	Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &vertexBuffer));
}

void VertexTopologyDemo::Destroy()
{
	SafeRelease(vertexBuffer);
	SafeDelete(shader);
}

void VertexTopologyDemo::Update()
{
}

void VertexTopologyDemo::Render()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	static bool bStrip;
	ImGui::Checkbox("Strip", &bStrip);

	D3D11_PRIMITIVE_TOPOLOGY topologies[] = 
	{
		D3D_PRIMITIVE_TOPOLOGY_LINELIST,
		D3D_PRIMITIVE_TOPOLOGY_LINESTRIP,
	};

	D3D::GetDC()->IASetPrimitiveTopology(bStrip ? topologies[1] : topologies[0]);


	static UINT startLocation;
	ImGui::SliderInt("Start Location", (int*)&startLocation, 0, 6);
	shader->Draw(0, 0, 6, startLocation);
}
