#include "00_Light.fx"
#include "00_VertexShader.fx"

float3 LightDirection = float3(-1, -1, 0);

float4 PS(MeshOutput input) : SV_Target
{
    float3 normal = normalize(input.Normal);
	
    float4 baseColor = DiffuseMap.Sample(LinearSampler, input.Uv);
    float lambert = saturate(dot(normal, -LightDirection));

    return baseColor * lambert;
}


technique11 T0
{
	P_VP(P0, VS_Mesh, PS)
	P_VP(P1, VS_Model, PS)
	P_VP(P2, VS_Animation, PS)
}