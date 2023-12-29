#include "00_Global.fx"
#include "00_Light.fx"

float3 LightDirection = float3(-1, -1, 1);

struct VertexModel
{
    float4 Position : Position;
    float2 Uv : Uv;
    float3 Normal : Normal;
    float3 Tangent : Tangent;
    float4 BlendIndices : BlendIndices;
    float4 BlendWeights : BlendWeights;
};

struct VertexOutput
{
    float4 Position : SV_Position;
    float2 Uv : Uv;
    float3 Normal : Normal;
};

#define MAX_MODEL_TRANSFORMS 250
#define MAX_MODEL_KEYFRAMES 500

cbuffer CB_Bones
{
    Matrix BoneTransforms[MAX_MODEL_TRANSFORMS]; //전체 Bone의 배열

    uint BoneIndex; //현재 내가 붙어야할 BoneIndex
};

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
    float TakeTime; //다음 클립으로 전환이 이뤄질 요구 시간
    float TweenTime; //다음 클립으로 넘어갈 시간(이 값이 1이 됐을 때)
    float ChangeTime; //델타 타임 누적
    float Padding;

    AnimationFrame Curr;
    AnimationFrame Next;
    
};

cbuffer CB_AnimationFrame
{
    TweenFrame TweenFrames;
};


Texture2DArray TransformsMap;

void SetAnimationWorld(inout matrix world, VertexModel input)
{
    float indices[4] = { input.BlendIndices.x, input.BlendIndices.y, input.BlendIndices.z, input.BlendIndices.w };
    float weights[4] = { input.BlendWeights.x, input.BlendWeights.y, input.BlendWeights.z, input.BlendWeights.w };
	
    //[0] 현재 재생할 클립 번호, [1] 다음에 재생할 클립 번호
    int clip[2]; 
    int currFrame[2], nextFrame[2];
    float time[2];
	
    clip[0] = TweenFrames.Curr.Clip; 
    currFrame[0] = TweenFrames.Curr.CurrentFrame; //현재 재생 중인 프레임 번호
    nextFrame[0] = TweenFrames.Curr.NextFrame; //다음 재생 할 프레임 번호
    time[0] = TweenFrames.Curr.Time; //현재 프레임 ~ 다음 프레임의 보간 Alpha
	
    clip[1] = TweenFrames.Next.Clip;
    currFrame[1] = TweenFrames.Next.CurrentFrame; //현재 재생 중인 프레임 번호
    nextFrame[1] = TweenFrames.Next.NextFrame; //다음 재생 할 프레임 번호
    time[1] = TweenFrames.Next.Time; //현재 프레임 ~ 다음 프레임의 보간 Alpha
	
    
    
    float4 c0, c1, c2, c3;
    float4 n0, n1, n2, n3;
    
	
    matrix transform = 0; //Skinning이 누적된 결과 값
    matrix curr = 0,next=0; //텍스쳐 읽어온 값을 저장받을 매트릭스
    matrix currAnim = 0,nextAnim = 0; //현재 프레임 ~ 다음 프레임의 보간된 매트릭스
    
    
	//Skinning
	[unroll(4)]
    for (int i = 0; i < 4; i++)
    {
        c0 = TransformsMap.Load(int4(indices[i] * 4 + 0, currFrame[0], clip[0], 0)); //_11_12_13_14
        c1 = TransformsMap.Load(int4(indices[i] * 4 + 1, currFrame[0], clip[0], 0)); //_21_22_23_24
        c2 = TransformsMap.Load(int4(indices[i] * 4 + 2, currFrame[0], clip[0], 0)); //_31_32_33_34
        c3 = TransformsMap.Load(int4(indices[i] * 4 + 3, currFrame[0], clip[0], 0)); //_41_42_43_44
        curr = matrix(c0, c1, c2, c3);
        
        n0 = TransformsMap.Load(int4(indices[i] * 4 + 0, nextFrame[0], clip[0], 0)); //_11_12_13_14
        n1 = TransformsMap.Load(int4(indices[i] * 4 + 1, nextFrame[0], clip[0], 0)); //_21_22_23_24
        n2 = TransformsMap.Load(int4(indices[i] * 4 + 2, nextFrame[0], clip[0], 0)); //_31_32_33_34
        n3 = TransformsMap.Load(int4(indices[i] * 4 + 3, nextFrame[0], clip[0], 0)); //_41_42_43_44
        next = matrix(n0, n1, n2, n3);
        
        currAnim = lerp(curr, next, time[0]);
        
        if(clip[1]>-1)
        {
            c0 = TransformsMap.Load(int4(indices[i] * 4 + 0, currFrame[1], clip[1], 0)); //_11_12_13_14
            c1 = TransformsMap.Load(int4(indices[i] * 4 + 1, currFrame[1], clip[1], 0)); //_21_22_23_24
            c2 = TransformsMap.Load(int4(indices[i] * 4 + 2, currFrame[1], clip[1], 0)); //_31_32_33_34
            c3 = TransformsMap.Load(int4(indices[i] * 4 + 3, currFrame[1], clip[1], 0)); //_41_42_43_44
            curr = matrix(c0, c1, c2, c3);
        
            n0 = TransformsMap.Load(int4(indices[i] * 4 + 0, nextFrame[1], clip[1], 0)); //_11_12_13_14
            n1 = TransformsMap.Load(int4(indices[i] * 4 + 1, nextFrame[1], clip[1], 0)); //_21_22_23_24
            n2 = TransformsMap.Load(int4(indices[i] * 4 + 2, nextFrame[1], clip[1], 0)); //_31_32_33_34
            n3 = TransformsMap.Load(int4(indices[i] * 4 + 3, nextFrame[1], clip[1], 0)); //_41_42_43_44
            next = matrix(n0, n1, n2, n3);
        
            nextAnim = lerp(curr, next, time[1]);
            
            currAnim = lerp(currAnim, nextAnim, TweenFrames.TweenTime);

        }
        
        transform += mul(weights[i], currAnim);
        
       
    }
	
    world = mul(transform, world);
	//invMesh * anim * parent (C++) * weight * World (FX)
}

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


void SetBlendingWorld(inout matrix world, VertexModel input)
{
    float indices[4] = { input.BlendIndices.x, input.BlendIndices.y, input.BlendIndices.z, input.BlendIndices.w };
    float weights[4] = { input.BlendWeights.x, input.BlendWeights.y, input.BlendWeights.z, input.BlendWeights.w };
	
	
	
    float4 c0, c1, c2, c3;
    float4 n0, n1, n2, n3;
	
    matrix transform = 0; //Skinning이 누적된 결과 값
    matrix curr = 0, next = 0; //텍스쳐 읽어온 값을 저장받을 매트릭스
    matrix currAnim[3];
    matrix anim = 0;

	//Skinning
	[unroll(4)]
    for (int i = 0; i < 4; i++)
    {
        [unroll(3)]
        for (int c = 0; c < 3; c++)
        {
            c0 = TransformsMap.Load(int4(indices[i] * 4 + 0, BlendingFrame.Clip[c].CurrentFrame, BlendingFrame.Clip[c].Clip, 0)); //_11_12_13_14
            c1 = TransformsMap.Load(int4(indices[i] * 4 + 1, BlendingFrame.Clip[c].CurrentFrame, BlendingFrame.Clip[c].Clip, 0)); //_21_22_23_24
            c2 = TransformsMap.Load(int4(indices[i] * 4 + 2, BlendingFrame.Clip[c].CurrentFrame, BlendingFrame.Clip[c].Clip, 0)); //_31_32_33_34
            c3 = TransformsMap.Load(int4(indices[i] * 4 + 3, BlendingFrame.Clip[c].CurrentFrame, BlendingFrame.Clip[c].Clip, 0)); //_41_42_43_44
            curr = matrix(c0, c1, c2, c3);
		
            n0 = TransformsMap.Load(int4(indices[i] * 4 + 0, BlendingFrame.Clip[c].NextFrame, BlendingFrame.Clip[c].Clip, 0)); //_11_12_13_14
            n1 = TransformsMap.Load(int4(indices[i] * 4 + 1, BlendingFrame.Clip[c].NextFrame, BlendingFrame.Clip[c].Clip, 0)); //_21_22_23_24
            n2 = TransformsMap.Load(int4(indices[i] * 4 + 2, BlendingFrame.Clip[c].NextFrame, BlendingFrame.Clip[c].Clip, 0)); //_31_32_33_34
            n3 = TransformsMap.Load(int4(indices[i] * 4 + 3, BlendingFrame.Clip[c].NextFrame, BlendingFrame.Clip[c].Clip, 0)); //_41_42_43_44
            next = matrix(n0, n1, n2, n3);
		
           
            currAnim[c] = lerp(curr, next,  BlendingFrame.Clip[0].Time);
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
        
        transform += mul(weights[i], anim);
    }//for(i)
    
    
	
    world = mul(transform, world);
	//invMesh * anim * parent (C++) * weight * World (FX)
}



//--------------------------------------------------------------------------------
VertexOutput VS(VertexModel input)
{
    VertexOutput output;
	
    if (BlendingFrame.Mode == 0)
        SetAnimationWorld(World, input);
    else
        SetBlendingWorld(World, input);

    //SetAnimationWorld(World, input);//두번하면트오세됨
	
    output.Position = WorldPosition(input.Position);
    output.Position = ViewProjection(output.Position);
	
    output.Normal = WorldNormal(input.Normal);
    output.Uv = input.Uv;
	
    return output;
}

float4 PS(VertexOutput input) : SV_Target
{
    float3 normal = normalize(input.Normal);
	
    float4 diffuseColor = DiffuseMap.Sample(LinearSampler, input.Uv);
    float lambert = saturate(dot(normal, -LightDirection));

    return diffuseColor * lambert;
}

technique11 T0
{
	P_VP(P0, VS, PS)
	P_RS_VP(P1, FillMode_WireFrame, VS, PS)
}