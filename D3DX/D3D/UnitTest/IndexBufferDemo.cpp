#include "stdafx.h"
#include "IndexBufferDemo.h"

void IndexBufferDemo::Initialize()
{
	shader = new Shader(L"04_World.fxo");

	//Create Vertex Buffer
	{
		vertices[0].Position = Vector3(-0.5f, -0.5f, +0.0f);
		vertices[1].Position = Vector3(-0.5f, +0.5f, +0.0f);
		vertices[2].Position = Vector3(+0.5f, -0.5f, +0.0f);
		vertices[3].Position = Vector3(+0.5f, +0.5f, +0.0f);

		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.ByteWidth = sizeof(Vertex) * ARRAYSIZE(vertices);
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subResource = { 0 };
		subResource.pSysMem = vertices;

		Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &vertexBuffer));
	}

	//Create Index Buffer
	{
		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 2;
		indices[3] = 2;
		indices[4] = 1;
		indices[5] = 3;

		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.ByteWidth = sizeof(UINT) * ARRAYSIZE(indices);
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subResource = { 0 };
		subResource.pSysMem = indices;

		Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &indexBuffer));
	}

	D3DXMatrixIdentity(&world);
}

void IndexBufferDemo::Destroy()
{
	SafeDelete(shader);
	SafeRelease(vertexBuffer);
	SafeRelease(indexBuffer);
}

void IndexBufferDemo::Update()
{
	// x 위치 : world._41
	// y 위치 : world._42
	// z 위치 : world._43

#ifdef MatrixMember
	if (Keyboard::Get()->Press('D'))
		world._41 += 2.f * Time::Delta();
	else if (Keyboard::Get()->Press('A'))
		world._41 -= 2.f * Time::Delta();

	if (Keyboard::Get()->Press('W'))
		world._42 += 2.f * Time::Delta();
	else if (Keyboard::Get()->Press('S'))
		world._42 -= 2.f * Time::Delta();
#endif

	static Vector3 position = Vector3(0, 0, 0);
	static Vector3 scale = Vector3(1, 1, 1);

	if (Keyboard::Get()->Press(VK_LSHIFT))
	{
		if (Keyboard::Get()->Press('D'))
			scale.x += 2.f * Time::Delta();
		else if (Keyboard::Get()->Press('A'))
			scale.x -= 2.f * Time::Delta();

		if (Keyboard::Get()->Press('W'))
			scale.y += 2.f * Time::Delta();
		else if (Keyboard::Get()->Press('S'))
			scale.y -= 2.f * Time::Delta();
	}
	else
	{
		if (Keyboard::Get()->Press('D'))
			position.x += 2.f * Time::Delta();
		else if (Keyboard::Get()->Press('A'))
			position.x -= 2.f * Time::Delta();

		if (Keyboard::Get()->Press('W'))
			position.y += 2.f * Time::Delta();
		else if (Keyboard::Get()->Press('S'))
			position.y -= 2.f * Time::Delta();
	}

	Matrix S, T;
	D3DXMatrixScaling(&S, scale.x, scale.y, scale.z);
	D3DXMatrixTranslation(&T, position.x, position.y, position.z);
	world = S * T;

	shader->AsMatrix("World")->SetMatrix(world);
	shader->AsMatrix("View")->SetMatrix(Context::Get()->View());
	shader->AsMatrix("Projection")->SetMatrix(Context::Get()->Projection());
}

void IndexBufferDemo::Render()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	static bool bWireFrame;
	ImGui::Checkbox("WireFrame", &bWireFrame);

	shader->DrawIndexed(0, bWireFrame ? 1 : 0, 6);
}
