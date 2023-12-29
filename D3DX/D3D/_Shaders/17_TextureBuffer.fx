Texture2DArray<float4> Input; //srv타고온에
RWTexture2DArray<float4> Output; //uav로넘길애




[numthreads(32, 32, 1)] //디스패치 대그룹에서 나눈 소그룹수(직접지정)
void CS_Diffuse(uint3 id : SV_DispatchThreadID)
{
    Output[id] = Input.Load(int4(id, 0)); 
    
    
   
}



[numthreads(32, 32, 1)] //디스패치 대그룹에서 나눈 소그룹수(직접지정)
void CS_Inverse(uint3 id :SV_DispatchThreadID)
{
    Output[id] = 1- Input.Load(int4(id,0)); //아웃풋에 텍스쳐읽은거 민맵핑최대퀄리티 색반전
    
    
   
}

[numthreads(32, 32, 1)] //디스패치 대그룹에서 나눈 소그룹수(직접지정)
void CS_RedChannel(uint3 id : SV_DispatchThreadID)
{
    
    float4 input = Input.Load(int4(id, 0));
    
    Output[id] = float4(input.r, 0, 0, 1); 
    
    
   
}

[numthreads(32, 32, 1)] //디스패치 대그룹에서 나눈 소그룹수(직접지정)
void CS_Grayscale(uint3 id : SV_DispatchThreadID)
{
    
    float4 input = Input.Load(int4(id, 0));
    
    Output[id] = (input.r+input.g,+input.b)*0.33333f;
    
    
   
}

technique11 T0
{
    pass p0
    {
        SetVertexShader(NULL);
        SetPixelShader(NULL);

        SetComputeShader(CompileShader(cs_5_0, CS_Diffuse()));
    }
    pass p1
    {
        SetVertexShader(NULL);
        SetPixelShader(NULL);

        SetComputeShader(CompileShader(cs_5_0, CS_Inverse()));
    }
    pass p2
    {
        SetVertexShader(NULL);
        SetPixelShader(NULL);

        SetComputeShader(CompileShader(cs_5_0, CS_RedChannel()));
    }
    pass p3
    {
        SetVertexShader(NULL);
        SetPixelShader(NULL);

        SetComputeShader(CompileShader(cs_5_0, CS_Grayscale()));
    }
}