#include "Framework.h"
#include "ModelAnimator.h"


ModelAnimator::ModelAnimator(Shader* shader)
	: shader(shader)
{
	model = new Model();
	transform = new Transform(shader);

	frameBuffer = new ConstantBuffer(&tweenDesc, sizeof(TweenDesc));
	blendBuffer = new ConstantBuffer(&blendDesc, sizeof(BlendDesc));

	//Create ComputeBuffer Resources
	{
		computeShader = new Shader(L"19_GetAnimBones.fxo");

		sComputeWorld = computeShader->AsMatrix("World");

		sComputeFrameBuffer = computeShader->AsConstantBuffer("CB_AnimationFrame");
		sComputeBlendBuffer = computeShader->AsConstantBuffer("CB_BlendingFrame");
		sComputeTransformsSRV = computeShader->AsSRV("TransformsMap");

		computeBoneBuffer = new StructuredBuffer(nullptr, sizeof(Matrix), MAX_MODEL_TRANSFORMS, sizeof(Matrix), MAX_MODEL_TRANSFORMS);
		sComputeInputBoneBuffer = computeShader->AsSRV("InputBones");
		sComputeOutputBoneBuffer = computeShader->AsUAV("OutputBones");
	}
}

ModelAnimator::~ModelAnimator()
{
	SafeDelete(model);
	SafeDelete(transform);

	SafeDeleteArray(clipTransform);
	SafeRelease(texture);
	SafeRelease(transformsSRV);

	SafeDelete(frameBuffer);
	SafeDelete(blendBuffer);

	SafeDelete(computeShader);
	SafeDelete(computeBoneBuffer);
}

void ModelAnimator::Update()
{
	if (texture == nullptr)
	{
		SetShader(shader, true);
		CreateTexture();

		Matrix bones[MAX_MODEL_TRANSFORMS];
		for (UINT i = 0; i < model->BoneCount(); i++)
			bones[i] = model->BoneByIndex(i)->Transform();

		computeBoneBuffer->CopyToInput(bones);
	}

	if (blendDesc.Mode == 0)
		UpdateAnimationFrame();
	else
		UpdateBledingFrame();

	frameBuffer->Render();
	blendBuffer->Render();

	frameTime += Time::Delta();
	if (frameTime > (1.f / frameRate))
	{
		sComputeWorld->SetMatrix(transform->World());

		sComputeFrameBuffer->SetConstantBuffer(frameBuffer->Buffer());
		sComputeBlendBuffer->SetConstantBuffer(blendBuffer->Buffer());
		sComputeTransformsSRV->SetResource(transformsSRV);

		sComputeInputBoneBuffer->SetResource(computeBoneBuffer->SRV());
		sComputeOutputBoneBuffer->SetUnorderedAccessView(computeBoneBuffer->UAV());

		computeShader->Dispatch(0, 0, 1, 1, 1);
	}
	frameTime = fmod(frameTime, (1.f / frameRate));

	for (ModelMesh* mesh : model->meshes)
		mesh->Update();
}

void ModelAnimator::Render()
{
	sFrameBuffer->SetConstantBuffer(frameBuffer->Buffer());
	sBlendBuffer->SetConstantBuffer(blendBuffer->Buffer());

	sTransformsSRV->SetResource(transformsSRV);

	for (ModelMesh* mesh : model->meshes)
	{
		mesh->SetTransform(transform);
		mesh->Render();
	}
}

void ModelAnimator::UpdateAnimationFrame()
{
	TweenDesc& desc = tweenDesc;
	ModelClip* clip = model->ClipByIndex(desc.Curr.Clip);

	desc.Curr.RunningTime += Time::Delta();

	float time = 1.f / clip->FrameRate() / desc.Curr.Speed;

	//Currernt Clip
	if (desc.Curr.Time >= 1.f)
	{
		desc.Curr.RunningTime = 0;

		desc.Curr.CurrentFrame = (desc.Curr.CurrentFrame + 1) % clip->FrameCount();
		desc.Curr.NextFrame = (desc.Curr.CurrentFrame + 1) % clip->FrameCount();
	}
	desc.Curr.Time = desc.Curr.RunningTime / time;

	//Next Clip
	if (desc.Next.Clip > -1)
	{
		ModelClip* nextClip = model->ClipByIndex(desc.Next.Clip);

		desc.ChangeTime += Time::Delta();
		desc.TweenTime = desc.ChangeTime / desc.TakeTime;

		if (desc.TweenTime >= 1.f)
		{
			desc.Curr = desc.Next;

			desc.Next.Clip = -1;
			desc.Next.CurrentFrame = 0;
			desc.Next.NextFrame = 0;
			desc.Next.Time = 0;
			desc.Next.RunningTime = 0;

			desc.ChangeTime = 0;
			desc.TweenTime = 0;
		}
		else
		{
			desc.Next.RunningTime += Time::Delta();

			float time = 1.f / clip->FrameRate() / desc.Next.Speed;

			if (desc.Next.Time >= 1.f)
			{
				desc.Next.RunningTime = 0;

				desc.Next.CurrentFrame = (desc.Next.CurrentFrame + 1) % clip->FrameCount();
				desc.Next.NextFrame = (desc.Next.CurrentFrame + 1) % clip->FrameCount();
			}
		}
	}
}

void ModelAnimator::UpdateBledingFrame()
{
	BlendDesc& desc = blendDesc;

	for (UINT i = 0; i < 3; i++)
	{
		ModelClip* clip = model->ClipByIndex(desc.Clip[i].Clip);

		desc.Clip[i].RunningTime += Time::Delta();

		float time = 1.f / clip->FrameRate() / desc.Clip[i].Speed;

		//Currernt Clip
		if (desc.Clip[i].Time >= 1.f)
		{
			desc.Clip[i].RunningTime = 0;

			desc.Clip[i].CurrentFrame = (desc.Clip[i].CurrentFrame + 1) % clip->FrameCount();
			desc.Clip[i].NextFrame = (desc.Clip[i].CurrentFrame + 1) % clip->FrameCount();
		}
		desc.Clip[i].Time = desc.Clip[i].RunningTime / time;

	}
}

void ModelAnimator::ReadMesh(wstring file)
{
	model->ReadMesh(file);
}

void ModelAnimator::ReadMaterial(wstring file)
{
	model->ReadMaterial(file);
}

void ModelAnimator::ReadClip(wstring file)
{
	model->ReadClip(file);
}

void ModelAnimator::PlayTweenMode(UINT nextClip, float speed, float takeTime)
{
	blendDesc.Mode = 0;

	tweenDesc.TakeTime = takeTime;
	tweenDesc.Next.Clip = nextClip;
	tweenDesc.Next.Speed = speed;
}

void ModelAnimator::PlayBlendMode(UINT clip1, UINT clip2, UINT clip3)
{
	blendDesc.Mode = 1;

	blendDesc.Clip[0].Clip = clip1;
	blendDesc.Clip[1].Clip = clip2;
	blendDesc.Clip[2].Clip = clip3;
}

void ModelAnimator::SetBlendAlpha(float alpha)
{
	alpha = Math::Clamp(alpha, 0.f, 2.f);

	blendDesc.Alpha = alpha;
}

void ModelAnimator::SetShader(Shader* shader, bool bDontCreateNewTransform)
{
	this->shader = shader;

	if (bDontCreateNewTransform == false)
	{
		SafeDelete(transform);
		transform = new Transform(shader);
	}

	sTransformsSRV = shader->AsSRV("TransformsMap");

	sFrameBuffer = shader->AsConstantBuffer("CB_AnimationFrame");
	sBlendBuffer = shader->AsConstantBuffer("CB_BlendingFrame");

	for (ModelMesh* mesh : model->Meshes())
		mesh->SetShader(shader);
}

void ModelAnimator::Pass(UINT pass)
{
	for (ModelMesh* mesh : model->Meshes())
		mesh->Pass(pass);
}

void ModelAnimator::GetAttachBones(Matrix* matrix)
{
	computeBoneBuffer->CopyFromOutput(matrix);
}

void ModelAnimator::CreateTexture()
{
	clipTransform = new ClipTransform[model->ClipCount()];
	for (UINT i = 0; i < model->ClipCount(); i++)
		CreateClipTransform(i);

	//Create Texture
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
		desc.Width = MAX_MODEL_TRANSFORMS * 4;
		desc.Height = MAX_MODEL_KEYFRAMES;
		desc.ArraySize = model->ClipCount();
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; //16 Byte
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;

		UINT pageSize = MAX_MODEL_TRANSFORMS * MAX_MODEL_KEYFRAMES * sizeof(Matrix);
		void* p = VirtualAlloc(nullptr, pageSize * model->ClipCount(), MEM_RESERVE, PAGE_READWRITE);

		for (UINT c = 0; c < model->ClipCount(); c++)
		{
			UINT clipStart = pageSize * c;

			for (UINT k = 0; k < MAX_MODEL_KEYFRAMES; k++)
			{
				void* temp = (BYTE*)p + MAX_MODEL_TRANSFORMS * k * sizeof(Matrix) + clipStart;

				VirtualAlloc(temp, MAX_MODEL_TRANSFORMS * sizeof(Matrix), MEM_COMMIT, PAGE_READWRITE);
				memcpy(temp, clipTransform[c].Trasnform[k], MAX_MODEL_TRANSFORMS * sizeof(Matrix));
			}
		}

		D3D11_SUBRESOURCE_DATA* subResource = new D3D11_SUBRESOURCE_DATA[model->ClipCount()];
		for (UINT c = 0; c < model->ClipCount(); c++)
		{
			void* temp = (BYTE*)p + c * pageSize;

			subResource[c].pSysMem = temp;
			subResource[c].SysMemPitch = MAX_MODEL_TRANSFORMS * sizeof(Matrix);
			subResource[c].SysMemSlicePitch = pageSize;
		}

		Check(D3D::GetDevice()->CreateTexture2D(&desc, subResource, &texture));

		SafeDeleteArray(subResource);
		VirtualFree(p, 0, MEM_RELEASE);
	}

	//Create SRV
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.MipLevels = 1;
		desc.Texture2DArray.ArraySize = model->ClipCount();

		Check(D3D::GetDevice()->CreateShaderResourceView(texture, &desc, &transformsSRV));
	}
}

void ModelAnimator::CreateClipTransform(UINT index)
{
	Matrix* bones = new Matrix[MAX_MODEL_TRANSFORMS];

	ModelClip* clip = model->ClipByIndex(index);

	for (UINT f = 0; f < clip->FrameCount(); f++)
	{
		for (UINT b = 0; b < model->BoneCount(); b++)
		{
			//MeshBone을 얻어와서 역행렬을 건다
			ModelBone* bone = model->BoneByIndex(b);

			Matrix inv = bone->Transform();
			D3DXMatrixInverse(&inv, nullptr, &inv);

			//ParentBone을 가져온다
			Matrix parent;
			int parentIndex = bone->ParentIndex();
			if (parentIndex < 0)
				D3DXMatrixIdentity(&parent);
			else
				parent = bones[parentIndex];

			//ModelClip에 저장된 SRT들을 얻어온다
			Matrix animation;

			ModelKeyFrame* frame = clip->Keyframe(bone->Name());

			if (frame != nullptr)
			{
				ModelKeyFrameData& data = frame->Transforms[f];

				Matrix S, R, T;
				D3DXMatrixScaling(&S, data.Scale.x, data.Scale.y, data.Scale.z);
				D3DXMatrixRotationQuaternion(&R, &data.Rotation);
				D3DXMatrixTranslation(&T, data.Translation.x, data.Translation.y, data.Translation.z);

				animation = S * R * T;
			}
			else
			{
				D3DXMatrixIdentity(&animation);
			}

			bones[b] = animation * parent;
			clipTransform[index].Trasnform[f][b] = inv * bones[b];
			//-> (inv * animation) * parent;
		}
	}



}
