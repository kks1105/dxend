#include "00_Global.fx"

Texture2D DiffuseMap;
cbuffer CB_Canvas
	
{
    matrix ViewCanvas;
    matrix ProjectionCanvas;
	
};

struct VertexInput
{
	float4 Position : Position;
	float2 Uv : Uv;
};

struct VertexOutput
{
	float4 Position : SV_Position;
	float2 Uv : Uv;
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;
    output.Position = WorldPosition(input.Position);
    output.Position = mul(output.Position, ViewCanvas);
    output.Position = mul(output.Position, ProjectionCanvas);

	output.Uv = input.Uv;
	
	return output;
}

float4 PS(VertexOutput input) : SV_Target
{
	
	
	return DiffuseMap.Sample(LinearSampler, input.Uv);


}


technique11 T0
{
	P_DSS_VP(P0, DepthEnable_False, VS, PS)

}