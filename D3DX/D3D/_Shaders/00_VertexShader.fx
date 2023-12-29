#include "00_Global.fx"
//-----------------------------------------------------------------------------
//VS_GENERATE Macro
//-----------------------------------------------------------------------------
#define VS_GENERATE \
output.oPosition = input.Position.xyz; \
\
output.Position = WorldPosition(input.Position); \
output.wPosition = output.Position.xyz; \
\
output.Position = ViewProjection(output.Position); \
\
output.Normal = WorldNormal(input.Normal); \
output.Uv = input.Uv; \
output.Color = input.Color;


//-----------------------------------------------------------------------------
//Mesh
//-----------------------------------------------------------------------------
struct VertexMesh
{
    float4 Position : Position;
    float2 Uv : Uv;
    float3 Normal : Normal;
    
    matrix Transform : Inst1_Transform;
    float4 Color : Inst2_Color;
};

void SetMeshWorld(inout matrix world, VertexMesh input)
{
    world = input.Transform;
    
}


MeshOutput VS_Mesh(VertexMesh input)
{
    MeshOutput output;
    SetMeshWorld(World, input);
    
	VS_GENERATE
	
    return output;
}

//-----------------------------------------------------------------------------
//Model
//-----------------------------------------------------------------------------
Texture2DArray TransformsMap;

struct VertexModel
{
    float4 Position : Position;
    float2 Uv : Uv;
    float3 Normal : Normal;
    float3 Tangent : Tangent;
    float4 BlendIndices : BlendIndices;
    float4 BlendWeights : BlendWeights;
    
    matrix Transform : Inst1_Transform;
    float4 Color : Inst2_Color;
    
    uint InstanceID : SV_InstanceID;
};

#define MAX_MODEL_TRANSFORMS 250
cbuffer CB_Bones
{
    Matrix BoneTransforms[MAX_MODEL_TRANSFORMS]; //전체 Bone의 배열

    uint BoneIndex; //현재 내가 붙어야할 BoneIndex
};

void SetModelWorld(inout matrix world,VertexModel input)
{
    float4 m0 = TransformsMap.Load(int4(BoneIndex * 4 + 0, input.InstanceID, 0, 0));
    float4 m1 = TransformsMap.Load(int4(BoneIndex * 4 + 1, input.InstanceID, 0, 0));
    float4 m2 = TransformsMap.Load(int4(BoneIndex * 4 + 2, input.InstanceID, 0, 0));
    float4 m3 = TransformsMap.Load(int4(BoneIndex * 4 + 3, input.InstanceID, 0, 0));
    
    matrix transform = matrix(m0, m1, m2, m3);
    world = mul(transform,input.Transform);
    
    
}

MeshOutput VS_Model(VertexModel input)
{
    MeshOutput output;
	
    SetModelWorld(World, input);
	VS_GENERATE
	
    return output;
}

//-----------------------------------------------------------------------------
//Animation
//-----------------------------------------------------------------------------
#define MAX_MODEL_KEYFRAMES 500

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

void SetAnimationWorld(inout matrix world, VertexModel input)
{
    float indices[4] = { input.BlendIndices.x, input.BlendIndices.y, input.BlendIndices.z, input.BlendIndices.w };
    float weights[4] = { input.BlendWeights.x, input.BlendWeights.y, input.BlendWeights.z, input.BlendWeights.w };
	
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
	
    matrix transform = 0;
    matrix curr = 0, next = 0;
    matrix currAnim = 0, nextAnim = 0;

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
		
        if (clip[1] > -1)
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
	
    matrix transform = 0;
    matrix curr = 0, next = 0;
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
	
        transform += mul(weights[i], anim);
    } //for(i)
	
    world = mul(transform, world);
	
}

MeshOutput VS_Animation(VertexModel input)
{
    MeshOutput output;
	
    if (BlendingFrame.Mode == 0)
        SetAnimationWorld(World, input);
    else
        SetBlendingWorld(World, input);
	
	VS_GENERATE
	
    return output;
}