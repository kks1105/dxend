#include "stdafx.h"
#include "TerrainDemo.h"

void TerrainDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(15, 0, 0);
	Context::Get()->GetCamera()->Position(113, 63, -90);
	((Freedom*)Context::Get()->GetCamera())->Speed(50, 2);

	shader = new Shader(L"10_Terrain.fxo");
	terrain = new Terrain(shader, L"Terrain/Gray256.png");
	terrain->BaseMap(L"Terrain/Cliff (Grassy).jpg");
}

void TerrainDemo::Destroy()
{
	SafeDelete(shader);
	SafeDelete(terrain);
}

void TerrainDemo::Update()
{
	static UINT pass = shader->PassCount() - 1;
	ImGui::InputInt("Pass", (int*)&pass);
	pass %= shader->PassCount();
	terrain->Pass() = pass;

	static Vector3 lightDirection = Vector3(-1, -1, 1);
	ImGui::SliderFloat3("Light Direction", lightDirection, -1, 1);
	shader->AsVector("LightDirection")->SetFloatVector(lightDirection);

	terrain->Update();
}

void TerrainDemo::Render()
{
	terrain->Render();
}
