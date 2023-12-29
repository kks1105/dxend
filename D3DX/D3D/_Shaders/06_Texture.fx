matrix World;
matrix View;
matrix Projection;

Texture2D BaseMap;

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
	
	output.Position = mul(input.Position, World);
	output.Position = mul(output.Position, View);
	output.Position = mul(output.Position, Projection);
	
	output.Uv = input.Uv;
	
	return output;
}

SamplerState samplerState;

float4 PS(VertexOutput input) : SV_Target
{
	return BaseMap.Sample(samplerState, input.Uv);
	
	//if (input.Uv.x < 0.5f)
	//	return float4(1, 0, 0, 1);
}

RasterizerState FillMode_WireFrame
{
	FillMode = WIREFRAME;
};


technique11 T0
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}

	pass P1
	{
		SetRasterizerState(FillMode_WireFrame);

		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}

	
}