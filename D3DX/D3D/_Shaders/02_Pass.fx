struct VertexInput
{
	float4 Position : Position;
	float4 Color : Color;
};

struct VertexOutput
{
	float4 Position : SV_Position;
	float4 Color : Color;
};


VertexOutput VS(VertexInput input)
{
	VertexOutput output;
	output.Position = input.Position;
	output.Color = input.Color;
	
	return output;
}

float4 PS_Red(VertexOutput input) : SV_Target
{
	return float4(1, 0, 0, 1);
}

float4 PS_Green(VertexOutput input) : SV_Target
{
	return float4(0, 1, 0, 1);
}

float4 PS_Blue(VertexOutput input) : SV_Target
{
	return float4(0, 0, 1, 1);
}

float4 PS_VertexColor(VertexOutput input) : SV_Target
{
	return input.Color;
}

float4 BaseColor;
float4 PS_VectorParam(VertexOutput input) : SV_Target
{
	return BaseColor;
}

technique11 T0
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader(CompileShader(ps_5_0, PS_Red()));
	}

	pass P1
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader(CompileShader(ps_5_0, PS_Green()));
	}

	pass P2
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader(CompileShader(ps_5_0, PS_Blue()));
	}

	pass P3
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader(CompileShader(ps_5_0, PS_VertexColor()));
	}

	pass P4
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader(CompileShader(ps_5_0, PS_VectorParam()));
	}
}