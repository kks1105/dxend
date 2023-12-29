#include "00_Global.fx"

float3 LightDirection;
Texture2D BaseMap;

struct VertexInput
{
	float4 Position : Position;
	float3 Normal : Normal;
	float2 Uv : Uv;
};

struct VertexOutput
{
	float4 Position : SV_Position;
	float3 Normal : Normal;
	float2 Uv : Uv;
};


VertexOutput VS(VertexInput input)
{
	VertexOutput output;
	output.Position = WorldPosition(input.Position);
	output.Position = ViewProjection(output.Position);
	
	output.Normal = WorldNormal(input.Normal);
	
	output.Uv = input.Uv;
	
	return output;
}

float4 PS(VertexOutput input) : SV_Target
{
	float4 baseColor = BaseMap.Sample(LinearSampler, input.Uv);
	float lambert = saturate(dot(input.Normal, -LightDirection));

	return baseColor * lambert;
}

technique11 T0
{
	P_VP(P0, VS, PS)
}