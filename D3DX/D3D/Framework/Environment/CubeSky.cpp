#include "Framework.h"
#include "CubeSky.h"

CubeSky::CubeSky(wstring path)
{
	shader = new Shader(L"13_CubeSky.fxo");

	sphere = new MeshRender(shader, new MeshSphere(0.5f));
	sphere->AddTransform();
	sphere->Pass(2);
	sphere->UpdateTransforms();

	path = L"../../_Textures/" + path;

	Check(D3DX11CreateShaderResourceViewFromFile
	(
		D3D::GetDevice(),
		path.c_str(),
		nullptr,
		nullptr,
		&srv,
		nullptr
	));

	sSrv = shader->AsSRV("SkyCubeMap");
}

CubeSky::~CubeSky()
{
	SafeDelete(shader);
	SafeDelete(sphere);

	SafeRelease(srv);
}

void CubeSky::Update()
{
	Vector3 cameraPosition;
	Context::Get()->GetCamera()->Position(&cameraPosition);
	sphere->GetTransform(0)->Position(cameraPosition);

	sphere->Update();
}

void CubeSky::Render()
{
	sSrv->SetResource(srv);
	sphere->Render();
}
