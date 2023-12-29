#include "stdafx.h"
#include "AnimationDemo.h"
#include "Converter.h"

void AnimationDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(12, 0, 0);
	Context::Get()->GetCamera()->Position(2, 4, -12);

	shader = new Shader(L"15_Animation.fxo");

	Kachujin();


}

void AnimationDemo::Destroy()
{
	SafeDelete(shader);
	SafeDelete(kachujin);
	SafeDelete(colliderObject);


}

void AnimationDemo::Update()
{
	//Test
	static int clip = 0;
	static float speed = 1.f;
	static float takeTime = 1.f;

	static bool bBlendMode = false;
	static float blendAlpha = 0.f;

	ImGui::Checkbox("BlendMode", &bBlendMode);

	if (bBlendMode == false)
	{
		ImGui::InputInt("Clip", &clip);
		clip = Math::Clamp(clip, 0, 4);

		ImGui::SliderFloat("Speed", &speed, 0.1f, 5.f);
		ImGui::SliderFloat("TakeTime", &takeTime, 0.1f, 5.f);

		if (ImGui::Button("Apply"))
			kachujin->PlayTweenMode(clip, speed, takeTime);
	}
	else
	{
		ImGui::SliderFloat("BlendAlpha", &blendAlpha, 0, 2);
		kachujin->SetBlendAlpha(blendAlpha);

		if (ImGui::Button("Apply"))
			kachujin->PlayBlendMode(0, 1, 2);
	}


	static Vector3 LightDirection = Vector3(-1, -1, +1);
	ImGui::SliderFloat3("LightDirection", LightDirection, -1, +1);
	shader->AsVector("LightDirection")->SetFloatVector(LightDirection);


	static UINT pass = 0;
	ImGui::InputInt("Pass", (int*)&pass);
	pass %= 2;


	if (kachujin != nullptr)
	{
		kachujin->Pass(pass);
		kachujin->Update();

		kachujin->GetAttachBones(bones);
		colliderObject->World->World(bones[40]);

		//Vector3 Init = Vector3(-2.9f, 1.45f, -50.0f);
		//ImGui::SliderFloat3("Init", (float*)&Init, -10, 10);
		//colliderObject->Init->Position(Init);

		//vector<ModelBone*> bones = kachujin->GetModel()->Bones();
		//int a = 0;
	}

}

void AnimationDemo::Render()
{

	if (kachujin != nullptr)
	{
		kachujin->Render();
		colliderObject->Collision->Render();
	}

}

void AnimationDemo::Kachujin()
{
	kachujin = new ModelAnimator(shader);
	kachujin->ReadMesh(L"Kachujin/Mesh");
	kachujin->ReadMaterial(L"Kachujin/Mesh");

	kachujin->ReadClip(L"Kachujin/Idle");
	kachujin->ReadClip(L"Kachujin/Walk");
	kachujin->ReadClip(L"Kachujin/Run");
	kachujin->ReadClip(L"Kachujin/Slash");
	kachujin->ReadClip(L"Kachujin/Uprock");

	kachujin->GetTransform()->Position(0, 0, 0);
	kachujin->GetTransform()->Scale(0.025f, 0.025f, 0.025f);

	colliderObject = new ColliderObject();
	colliderObject->Init->Position(-2.9f, 1.45f, -50.0f);
	colliderObject->Init->Scale(5, 5, 75);
	colliderObject->Init->Rotation(0, 0, 1);

}

