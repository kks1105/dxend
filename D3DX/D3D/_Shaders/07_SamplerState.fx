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

SamplerState PointSampler
{
	Filter = MIN_MAG_MIP_POINT;
};

SamplerState LinearSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
};

//uint Filter;
//float4 PS(VertexOutput input) : SV_Target
//{
//	if (Filter == 0)
//		return BaseMap.Sample(PointSampler, input.Uv);
	
//	return BaseMap.Sample(LinearSampler, input.Uv);
//}

SamplerState WrapSampler
{
	AddressU = WRAP;
	AddressV = WRAP;
};

SamplerState MirrorSampler
{
	AddressU = MIRROR;
	AddressV = MIRROR;
};

SamplerState ClampSampler
{
	AddressU = CLAMP;
	AddressV = CLAMP;
};

SamplerState BorderSampler
{
	AddressU = BORDER;
	AddressV = BORDER;

	BorderColor = float4(1, 0, 0, 1);
};


uint Address;
float4 PS(VertexOutput input) : SV_Target
{
	if (Address == 0)
		return BaseMap.Sample(WrapSampler, input.Uv);
	
	if (Address == 1)
		return BaseMap.Sample(MirrorSampler, input.Uv);
	
	if (Address == 2)
		return BaseMap.Sample(ClampSampler, input.Uv);
	
	if (Address == 3)
		return BaseMap.Sample(BorderSampler, input.Uv);
	
	return BaseMap.Sample(LinearSampler, input.Uv);
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