Texture2DArray<float4> Input; //srvŸ��¿�
RWTexture2DArray<float4> Output; //uav�γѱ��




[numthreads(32, 32, 1)] //����ġ ��׷쿡�� ���� �ұ׷��(��������)
void CS_Diffuse(uint3 id : SV_DispatchThreadID)
{
    Output[id] = Input.Load(int4(id, 0)); 
    
    
   
}



[numthreads(32, 32, 1)] //����ġ ��׷쿡�� ���� �ұ׷��(��������)
void CS_Inverse(uint3 id :SV_DispatchThreadID)
{
    Output[id] = 1- Input.Load(int4(id,0)); //�ƿ�ǲ�� �ؽ��������� �θ����ִ�����Ƽ ������
    
    
   
}

[numthreads(32, 32, 1)] //����ġ ��׷쿡�� ���� �ұ׷��(��������)
void CS_RedChannel(uint3 id : SV_DispatchThreadID)
{
    
    float4 input = Input.Load(int4(id, 0));
    
    Output[id] = float4(input.r, 0, 0, 1); 
    
    
   
}

[numthreads(32, 32, 1)] //����ġ ��׷쿡�� ���� �ұ׷��(��������)
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