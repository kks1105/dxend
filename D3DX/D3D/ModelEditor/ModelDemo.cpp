#include "stdafx.h"
#include "ModelDemo.h"
#include "Converter.h"

void ModelDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(12, 0, 0);
	Context::Get()->GetCamera()->Position(2, 4, -12);

	shader = new Shader(L"21_OnlyPS.fxo");

	Tank();
	Tower();
	Airplane();


}

void ModelDemo::Destroy()
{
	SafeDelete(shader);
	SafeDelete(tank);
	SafeDelete(tower);
	SafeDelete(airplane);


}

void ModelDemo::Update()
{
	//Directional Light
	static Vector3 LightDirection = Vector3(-1, -1, -1);
	ImGui::SliderFloat3("LightDirection", LightDirection, -1, 1);
	shader->AsVector("LightDirection")->SetFloatVector(LightDirection);

	static UINT index = 0;
	ImGui::SliderInt("InstanceIndex", (int*)&index, 0, airplane->TransformsCount() - 1);
	Transform* transform = airplane->GetTransform(index);

	Vector3 R;
	transform->RotationDegree(&R);
	ImGui::SliderFloat3("Rotatio", R, -180, +180);
	transform->RotationDegree(R);
	airplane->UpdateTransforms();

	if (tank != nullptr)
	{
		//tank->Pass(pass);
		tank->Update();
	}

	if (tower != nullptr)
	{
		//tower->Pass(pass);
		tower->Update();
	}

	if (airplane != nullptr)
	{
		//airplane->Pass(pass);
		airplane->Update();
	}
}

void ModelDemo::Render()
{



	if (tank != nullptr)
	{
		tank->Pass(1);
		tank->Render();
	}

	if (tower != nullptr)
	{
		tower->Pass(1);
		tower->Render();
	}

	if (airplane != nullptr)
	{
		airplane->Pass(1);
		airplane->Render();
	}
}

void ModelDemo::Tank()
{
	tank = new ModelRender(shader);
	tank->ReadMesh(L"Tank/Tank");
	tank->ReadMaterial(L"Tank/Tank");

	for (float x = -50; x < 50; x += 2.5f)
	{
		Transform* transform = tank->AddTransform(); //출력
		transform->Position(Vector3(x, 0.f, 5.f));//위치
		transform->RotationDegree(0, Math::Random(-180, 180), 0);
		transform->Scale(0.1f, 0.1f, 0.1f);

	}
	tank->UpdateTransforms();


}


void ModelDemo::Tower()
{
	tower = new ModelRender(shader);
	tower->ReadMesh(L"Tower/Tower");
	tower->ReadMaterial(L"Tower/Tower");

	for (float x = -50; x < 50; x += 2.5f)
	{
		Transform* transform = tower->AddTransform(); //출력
		transform->Position(Vector3(x, 0.f, 7.5f));//위치
		transform->RotationDegree(0, Math::Random(-180, 180), 0);
		transform->Scale(0.003f, 0.003f, 0.003f);

	}
	tower->UpdateTransforms();


}

void ModelDemo::Airplane()
{
	airplane = new ModelRender(shader);
	airplane->ReadMesh(L"B787/Airplane");
	airplane->ReadMaterial(L"B787/Airplane");

	for (float x = -50; x < 50; x += 2.5f)
	{
		Transform* transform = airplane->AddTransform(); //출력
		transform->Position(Vector3(x, 0.f, 2.5f));//위치
		transform->RotationDegree(0, Math::Random(-180, 180), 0);
		transform->Scale(0.00025f, 0.00025f, 0.00025f);

	}
	airplane->UpdateTransforms();

}
