#include "Framework.h"
#include "Canvas.h"

Canvas::Canvas()
	: Renderer(L"18_Canvas.fxo")
{
	D3DXMatrixLookAtLH(&desc.View, &Vector3(0, 0, -1), &Vector3(0, 0, 0), &Vector3(0, 1, 0));//뷰세팅

	D3DXMatrixOrthoOffCenterLH(&desc.Projection,0,D3D::Width(),0, D3D::Height(),-1,+1);//left,right,아래,위,두께범위

	buffer = new ConstantBuffer(&desc, sizeof(Desc));
	shader->AsConstantBuffer("CB_Canvas")->SetConstantBuffer(buffer->Buffer());

	VertexCanvas vertices[4];
	vertices[0].Position = Vector3(-0.5f, -0.5f, 0.f);
	vertices[1].Position = Vector3(-0.5f, +0.5f, 0.f);
	vertices[2].Position = Vector3(+0.5f, -0.5f, 0.f);
	vertices[3].Position = Vector3(+0.5f, +0.5f, 0.f);

	vertices[0].Uv = Vector2(0,1);
	vertices[1].Uv = Vector2(0,0);
	vertices[2].Uv = Vector2(1,1);
	vertices[3].Uv = Vector2(1,0);

	vertexBuffer = new VertexBuffer(vertices, 4, sizeof(VertexCanvas));

	UINT indices[6] = { 0, 1, 2, 2, 1, 3 };
	indexBuffer = new IndexBuffer(indices,6);

	sSrv = shader->AsSRV("DiffuseMap");

}

Canvas::~Canvas()
{
}

void Canvas::Update()
{
	Super::Update();

}

void Canvas::Render()
{
	Super::Render();

	buffer->Render();
	shader->DrawIndexed(0, 0, 6);

}

void Canvas::SRV(ID3D11ShaderResourceView* srv)
{
	sSrv->SetResource(srv);

}
