ByteAddressBuffer Input; //srvŸ��¿�
RWByteAddressBuffer Output; //uav�γѱ��


struct ComputeInput
{
    uint3 GroupID : SV_GroupID;//��׷� ���� ������ �ұ׷쿡�� �ڽ��Ǽұ׷� ��ȣ 
    uint3 GroupThreadID : SV_GroupThreadID;// �ұ׷쳻���� �� �ڽ�����ġ
    uint3 DispatchThreadID : SV_DispatchThreadID;//��׷쿡������ ������ �ڽ��� ��ġ
    uint GroupIndex : SV_GroupIndex;//��׷쿡�� ���� �ڽ�����ġ���� x+Y ����ġ 
    
};


struct ComputeOutput
{
    uint3 GroupID;
    uint3 GroupThreadID;
    uint3 DispatchThreadID;
    uint GroupIndex;

};

[numthreads(10, 8, 3)] //����ġ ��׷쿡�� ���� �ұ׷��(��������)
void CS(ComputeInput input)
{
    ComputeOutput result;//uav �ƿ�ǲ�������ҰŴ�����
    result.GroupID = asuint(input.GroupID); //���� ���̴��ý��ۺ����ι��� ��ǲ�Ǳ׷���̵�
    result.GroupThreadID = asuint(input.GroupThreadID);
    result.DispatchThreadID = asuint(input.DispatchThreadID);
    result.GroupIndex = asuint(input.GroupIndex);
	
    uint inAddress = (input.GroupID.x * 240 + input.GroupIndex)*4; //���õ����� ���Ǽ��ڴ�ũ�� �÷��Ѱ� 4;
    float value = asfloat(Input.Load(inAddress))*100.f;//�������� �����ؼ� �б�
    
    
    uint outAddress = (input.GroupID.x * 240 + input.GroupIndex) * 11 * 4; //�������� ���Ǽ��ڴ�ũ�� ����Ʈ3�� ��Ʈ1�� �÷�1�� =10 ũ��4
    
    
    Output.Store3(outAddress + 0, asuint(result.GroupID)); //���� uint3��ŭ����
    Output.Store3(outAddress + 12, asuint(result.GroupThreadID));//uint3��ŭ����
    Output.Store3(outAddress + 24, asuint(result.DispatchThreadID)); //uint3��ŭ����
    Output.Store(outAddress + 36, asuint(result.GroupIndex)); //uint3��ŭ����
    Output.Store(outAddress + 40, asuint(value));//uint��ŭ����
    
   
}

technique11 T0
{
    pass p0
    {
        SetVertexShader(NULL);
        SetPixelShader(NULL);

        SetComputeShader(CompileShader(cs_5_0, CS()));
    }
}