ByteAddressBuffer Input; //srv타고온에
RWByteAddressBuffer Output; //uav로넘길애


struct ComputeInput
{
    uint3 GroupID : SV_GroupID;//대그룹 에서 지정한 소그룹에서 자신의소그룹 번호 
    uint3 GroupThreadID : SV_GroupThreadID;// 소그룹내에서 의 자신의위치
    uint3 DispatchThreadID : SV_DispatchThreadID;//대그룹에서부터 떨어진 자신의 위치
    uint GroupIndex : SV_GroupIndex;//대그룹에서 부터 자신의위치까지 x+Y 한위치 
    
};


struct ComputeOutput
{
    uint3 GroupID;
    uint3 GroupThreadID;
    uint3 DispatchThreadID;
    uint GroupIndex;

};

[numthreads(10, 8, 3)] //디스패치 대그룹에서 나눈 소그룹수(직접지정)
void CS(ComputeInput input)
{
    ComputeOutput result;//uav 아웃풋에저장할거담을놈
    result.GroupID = asuint(input.GroupID); //위에 쉐이더시스템벨류로받은 인풋의그룹아이디
    result.GroupThreadID = asuint(input.GroupThreadID);
    result.DispatchThreadID = asuint(input.DispatchThreadID);
    result.GroupIndex = asuint(input.GroupIndex);
	
    uint inAddress = (input.GroupID.x * 240 + input.GroupIndex)*4; //들어올데이터 뒤의숫자는크기 플롯한개 4;
    float value = asfloat(Input.Load(inAddress))*100.f;//내보낸에 수정해서 읽기
    
    
    uint outAddress = (input.GroupID.x * 240 + input.GroupIndex) * 11 * 4; //들어갈데이터 뒤의숫자는크기 유인트3개 인트1개 플롯1개 =10 크기4
    
    
    Output.Store3(outAddress + 0, asuint(result.GroupID)); //저장 uint3만큼띄우기
    Output.Store3(outAddress + 12, asuint(result.GroupThreadID));//uint3만큼띄우기
    Output.Store3(outAddress + 24, asuint(result.DispatchThreadID)); //uint3만큼띄우기
    Output.Store(outAddress + 36, asuint(result.GroupIndex)); //uint3만큼띄우기
    Output.Store(outAddress + 40, asuint(value));//uint만큼띄우기
    
   
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