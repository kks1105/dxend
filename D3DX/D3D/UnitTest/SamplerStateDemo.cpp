#include "stdafx.h"
#include "SamplerStateDemo.h"

void SamplerStateDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(0, 0, 0);
	Context::Get()->GetCamera()->Position(0, 0, -5);
	((Freedom*)Context::Get()->GetCamera())->Speed(5, 0);

	shader = new Shader(L"07_SamplerState.fxo");

	vertices = new VertexTexture[4];

	vertices[0].Position = Vector3(-0.5f, -0.5f, 0.f);
	vertices[1].Position = Vector3(-0.5f, +0.5f, 0.f);
	vertices[2].Position = Vector3(+0.5f, -0.5f, 0.f);
	vertices[3].Position = Vector3(+0.5f, +0.5f, 0.f);

	vertices[0].Uv = Vector2(0, 2);
	vertices[1].Uv = Vector2(0, 0);
	vertices[2].Uv = Vector2(2, 2);
	vertices[3].Uv = Vector2(2, 0);

	//Create Vertex Buffer
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.ByteWidth = sizeof(VertexTexture) * 4;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subResource = { 0 };
		subResource.pSysMem = vertices;

		Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &vertexBuffer));
	}
	
	indices = new UINT[6]{ 0, 1, 2, 2, 1, 3};
	D3D11_SAMPLER_DESC;
	//Create Index Buffer
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.ByteWidth = sizeof(UINT) * 6;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subResource = { 0 };
		subResource.pSysMem = indices;

		Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &indexBuffer));
	}

	SafeDeleteArray(vertices);
	SafeDeleteArray(indices);

	baseMap = new Texture(L"Box.png");
}

void SamplerStateDemo::Destroy()
{
	SafeDelete(shader);
	SafeRelease(vertexBuffer);
	SafeRelease(indexBuffer);

	SafeDelete(baseMap);
}

void SamplerStateDemo::Update()
{
	if (ImGui::Button("Load Texture"))
	{
		function<void(wstring)> OnLoad = bind(&SamplerStateDemo::OnLoadTexture, this, placeholders::_1);
		Path::OpenFileDialog(L"", Path::ImageFilter, L"../../_Textures/", OnLoad, D3D::GetDesc().Handle);
	}

	/*static UINT Filter = 0;
	ImGui::InputInt("Filter", (int*)&Filter);
	Filter %= 2;
	shader->AsScalar("Filter")->SetInt(Filter);*/

	static UINT Address = 0;
	ImGui::InputInt("Address", (int*)&Address);
	Address %= 4;
	shader->AsScalar("Address")->SetInt(Address);

	Matrix world;
	D3DXMatrixIdentity(&world);

	shader->AsMatrix("World")->SetMatrix(world);
	shader->AsMatrix("View")->SetMatrix(Context::Get()->View());
	shader->AsMatrix("Projection")->SetMatrix(Context::Get()->Projection());
}

void SamplerStateDemo::Render()
{
	UINT stride = sizeof(VertexTexture);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	shader->AsSRV("BaseMap")->SetResource(baseMap->SRV());

	shader->DrawIndexed(0, 0, 6);
}

void SamplerStateDemo::OnLoadTexture(wstring path)
{
	SafeDelete(baseMap);

	baseMap = new Texture(path);
}
