#pragma once


class ModelAnimator
{
public:
	ModelAnimator(Shader* shader);
	~ModelAnimator();

	void Update();
	void Render();

private:
	void UpdateAnimationFrame();
	void UpdateBledingFrame();

public:
	void ReadMesh(wstring file);
	void ReadMaterial(wstring file);
	void ReadClip(wstring file);

	void PlayTweenMode(UINT nextClip, float speed = 1.f, float takeTime = 1.f);
	void PlayBlendMode(UINT clip1, UINT clip2, UINT clip3);
	void SetBlendAlpha(float alpha);

public:
	void SetShader(Shader* shader, bool bDontCreateNewTransform = false);
	void Pass(UINT pass);
	Transform* GetTransform() { return transform; }
	Model* GetModel() { return model; }

	void GetAttachBones(Matrix* matrix);

private:
	void CreateTexture();
	void CreateClipTransform(UINT index);

private:
	struct ClipTransform
	{
		Matrix** Trasnform;

		ClipTransform()
		{
			Trasnform = new Matrix*[MAX_MODEL_KEYFRAMES];

			for (UINT i = 0; i < MAX_MODEL_KEYFRAMES; i++)
				Trasnform[i] = new Matrix[MAX_MODEL_TRANSFORMS];
		}

		~ClipTransform()
		{
			for (UINT i = 0; i < MAX_MODEL_KEYFRAMES; i++)
				SafeDeleteArray(Trasnform[i]);

			SafeDeleteArray(Trasnform);
		}
	};
	ClipTransform* clipTransform; //clipTransform[c].Trasnform[f][b] = Matrix

private:
	ID3D11Texture2D* texture = nullptr;
	ID3D11ShaderResourceView* transformsSRV;
	ID3DX11EffectShaderResourceVariable* sTransformsSRV;

private:
	//Single Clip
	struct KeyFrameDesc
	{
		int Clip = 0;

		UINT CurrentFrame = 0;
		UINT NextFrame = 0;

		float Time = 0.f; //다음 프레임으로 넘어갈 시간(이 값이 1이 됐을 때)
		float RunningTime = 0.f; //델타 타임 누적

		float Speed = 1.f;

		Vector2 Padding;
	};

private:
	//Two Clip
	struct TweenDesc
	{
		float TakeTime = 0.1f; //다음 클립으로 전환이 이뤄질 요구 시간
		float TweenTime = 0.f; //다음 클립으로 넘어갈 시간(이 값이 1이 됐을 때)
		float ChangeTime = 0.f; //델타 타임 누적
		float Padding;

		KeyFrameDesc Curr;
		KeyFrameDesc Next;

		TweenDesc()
		{
			Curr.Clip = 0;
			Next.Clip = -1;
		}
	} tweenDesc;

	ConstantBuffer* frameBuffer;
	ID3DX11EffectConstantBuffer* sFrameBuffer;

private:
	struct BlendDesc
	{
		UINT Mode = 0;
		float Alpha = 0; //0~2
		Vector2 Padding;

		KeyFrameDesc Clip[3];
	} blendDesc;

	ConstantBuffer* blendBuffer;
	ID3DX11EffectConstantBuffer* sBlendBuffer;

private:
	Shader* shader;
	Model* model;

	Transform* transform; //Actor Transform(WS)

	//Compute Bone World from CS
private:
	float frameRate = 30.f; //CS Dispatch Per 1/30 
	float frameTime = 0.f; //Add DeltaTime

	Shader* computeShader;

	ID3DX11EffectMatrixVariable* sComputeWorld;
	ID3DX11EffectConstantBuffer* sComputeFrameBuffer;
	ID3DX11EffectConstantBuffer* sComputeBlendBuffer;
	ID3DX11EffectShaderResourceVariable* sComputeTransformsSRV;

	StructuredBuffer* computeBoneBuffer;
	ID3DX11EffectShaderResourceVariable* sComputeInputBoneBuffer;
	ID3DX11EffectUnorderedAccessViewVariable* sComputeOutputBoneBuffer;
};