#include "00_Global.fx"

struct BoneDesc
{
    matrix Transform;
};
StructuredBuffer<BoneDesc> InputBones;
RWStructuredBuffer<BoneDesc> OutputBones;

#define MAX_MODEL_TRANSFORMS 250

struct AnimationFrame
{
    int Clip;

    uint CurrentFrame;
    uint NextFrame;

    float Time;
    float RunningTime;

    float Speed;
};

struct TweenFrame
{
    float TakeTime;
    float TweenTime;
    float ChangeTime;
    float Padding;

    AnimationFrame Curr;
    AnimationFrame Next;
};

cbuffer CB_AnimationFrame
{
    TweenFrame TweenFrames;
};

Texture2DArray TransformsMap;

void SetAnimationWorld(inout matrix world, uint3 id)
{
	//[0] 현재 재생할 클립 번호, [1] 다음에 재생할 클립 번호
    int clip[2];
    int currFrame[2], nextFrame[2];
    float time[2];
	
    clip[0] = TweenFrames.Curr.Clip;
    currFrame[0] = TweenFrames.Curr.CurrentFrame;
    nextFrame[0] = TweenFrames.Curr.NextFrame;
    time[0] = TweenFrames.Curr.Time;
	
    clip[1] = TweenFrames.Next.Clip;
    currFrame[1] = TweenFrames.Next.CurrentFrame;
    nextFrame[1] = TweenFrames.Next.NextFrame;
    time[1] = TweenFrames.Next.Time;
	
    float4 c0, c1, c2, c3;
    float4 n0, n1, n2, n3;
	
    matrix transform = 0; //Skinning이 누적된 결과 값
    matrix curr = 0, next = 0; //텍스쳐 읽어온 값을 저장받을 매트릭스
    matrix currAnim = 0, nextAnim = 0; //현재 프레임 ~ 다음 프레임의 보간된 매트릭스

    c0 = TransformsMap.Load(int4(id.x * 4 + 0, currFrame[0], clip[0], 0)); //_11_12_13_14
    c1 = TransformsMap.Load(int4(id.x * 4 + 1, currFrame[0], clip[0], 0)); //_21_22_23_24
    c2 = TransformsMap.Load(int4(id.x * 4 + 2, currFrame[0], clip[0], 0)); //_31_32_33_34
    c3 = TransformsMap.Load(int4(id.x * 4 + 3, currFrame[0], clip[0], 0)); //_41_42_43_44
    curr = matrix(c0, c1, c2, c3);
		
    n0 = TransformsMap.Load(int4(id.x * 4 + 0, nextFrame[0], clip[0], 0)); //_11_12_13_14
    n1 = TransformsMap.Load(int4(id.x * 4 + 1, nextFrame[0], clip[0], 0)); //_21_22_23_24
    n2 = TransformsMap.Load(int4(id.x * 4 + 2, nextFrame[0], clip[0], 0)); //_31_32_33_34
    n3 = TransformsMap.Load(int4(id.x * 4 + 3, nextFrame[0], clip[0], 0)); //_41_42_43_44
    next = matrix(n0, n1, n2, n3);
		
    currAnim = lerp(curr, next, time[0]);
		
    if (clip[1] > -1)
    {
        c0 = TransformsMap.Load(int4(id.x * 4 + 0, currFrame[1], clip[1], 0)); //_11_12_13_14
        c1 = TransformsMap.Load(int4(id.x * 4 + 1, currFrame[1], clip[1], 0)); //_21_22_23_24
        c2 = TransformsMap.Load(int4(id.x * 4 + 2, currFrame[1], clip[1], 0)); //_31_32_33_34
        c3 = TransformsMap.Load(int4(id.x * 4 + 3, currFrame[1], clip[1], 0)); //_41_42_43_44
        curr = matrix(c0, c1, c2, c3);
		
        n0 = TransformsMap.Load(int4(id.x * 4 + 0, nextFrame[1], clip[1], 0)); //_11_12_13_14
        n1 = TransformsMap.Load(int4(id.x * 4 + 1, nextFrame[1], clip[1], 0)); //_21_22_23_24
        n2 = TransformsMap.Load(int4(id.x * 4 + 2, nextFrame[1], clip[1], 0)); //_31_32_33_34
        n3 = TransformsMap.Load(int4(id.x * 4 + 3, nextFrame[1], clip[1], 0)); //_41_42_43_44
        next = matrix(n0, n1, n2, n3);
		
        nextAnim = lerp(curr, next, time[1]);
			
        currAnim = lerp(currAnim, nextAnim, TweenFrames.TweenTime);
    }
		
    world = mul(currAnim, world);
	//invMesh * anim * parent (C++) * weight * World (FX)
}

//-----------------------------------------------------------------------------
struct BlendDesc
{
    uint Mode;
    float Alpha; //0~2
    float2 Padding;

    AnimationFrame Clip[3];
};

cbuffer CB_BlendingFrame
{
    BlendDesc BlendingFrame;
};

void SetBlendingWorld(inout matrix world, uint3 id)
{
    float4 c0, c1, c2, c3;
    float4 n0, n1, n2, n3;
	
    matrix transform = 0;
    matrix curr = 0, next = 0;
    matrix currAnim[3];
    matrix anim = 0;

	[unroll(3)]
    for (int c = 0; c < 3; c++)
    {
        c0 = TransformsMap.Load(int4(id.x * 4 + 0, BlendingFrame.Clip[c].CurrentFrame, BlendingFrame.Clip[c].Clip, 0)); //_11_12_13_14
        c1 = TransformsMap.Load(int4(id.x * 4 + 1, BlendingFrame.Clip[c].CurrentFrame, BlendingFrame.Clip[c].Clip, 0)); //_21_22_23_24
        c2 = TransformsMap.Load(int4(id.x * 4 + 2, BlendingFrame.Clip[c].CurrentFrame, BlendingFrame.Clip[c].Clip, 0)); //_31_32_33_34
        c3 = TransformsMap.Load(int4(id.x * 4 + 3, BlendingFrame.Clip[c].CurrentFrame, BlendingFrame.Clip[c].Clip, 0)); //_41_42_43_44
        curr = matrix(c0, c1, c2, c3);
		
        n0 = TransformsMap.Load(int4(id.x * 4 + 0, BlendingFrame.Clip[c].NextFrame, BlendingFrame.Clip[c].Clip, 0)); //_11_12_13_14
        n1 = TransformsMap.Load(int4(id.x * 4 + 1, BlendingFrame.Clip[c].NextFrame, BlendingFrame.Clip[c].Clip, 0)); //_21_22_23_24
        n2 = TransformsMap.Load(int4(id.x * 4 + 2, BlendingFrame.Clip[c].NextFrame, BlendingFrame.Clip[c].Clip, 0)); //_31_32_33_34
        n3 = TransformsMap.Load(int4(id.x * 4 + 3, BlendingFrame.Clip[c].NextFrame, BlendingFrame.Clip[c].Clip, 0)); //_41_42_43_44
        next = matrix(n0, n1, n2, n3);
		
        currAnim[c] = lerp(curr, next, BlendingFrame.Clip[c].Time);
    } //for (c)
			
		
    float alpha = BlendingFrame.Alpha;
    int clipIndex[2] = { 0, 1 };
		
    if (alpha > 1)
    {
        clipIndex[0] = 1;
        clipIndex[1] = 2;

        alpha -= 1.f;
    } //if (alpha > 1)
			
    anim = lerp(currAnim[clipIndex[0]], currAnim[clipIndex[1]], alpha);
	
    world = mul(anim, world);
}

[numthreads(MAX_MODEL_TRANSFORMS, 1, 1)]
void CS(uint3 id : SV_GroupThreadID)
{
    matrix world = World;
	
    if (BlendingFrame.Mode == 0)
        SetAnimationWorld(world, id);
    else
        SetBlendingWorld(world, id);
	
    world = mul(InputBones[id.x].Transform, world);
	
    OutputBones[id.x].Transform = world;
}

technique11 T0
{
    pass p0
    {
        SetVertexShader(NULL);
        SetPixelShader(NULL);

        SetComputeShader(CompileShader(cs_5_0, CS()));
    }
}