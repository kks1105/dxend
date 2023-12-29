#include "stdafx.h"
#include "RawBufferDemo.h"

void RawBufferDemo::Initialize()
{
	Shader* shader = new Shader(L"16_ByteAddress.fxo");

	UINT count = 10 * 8 * 3 * 2; //쉐이더에서지정한소그룹수 *내가넣을디스패치크기

	//쉐이더에 있는 아웃풋 쓸 구조체
	struct Output
	{
		UINT GroupID[3];
		UINT GroupThreadID[3];
		UINT DispatchThreadID[3];
		UINT GroupIndex;
		float RetVal;//쉐이더에서받은애
	};

	

	float* inputDatas = new float[count];
	for (UINT i = 0; i < count; i++)
		inputDatas[i] = (float)i;

	RawBuffer* rawBuffer = new RawBuffer(inputDatas, sizeof(float)*count,sizeof(Output)*count); //로우버퍼  입력데이터 ,입력바이트크기,넣을애크기

	shader->AsSRV("Input")->SetResource(rawBuffer->SRV());
	shader->AsUAV("Output")->SetUnorderedAccessView(rawBuffer->UAV());
	shader->Dispatch(0, 0, 2, 1, 1); //compute shader 동작함수 테크니크번호 , 패스번호 ,x우 ,y하 ,z장

	Output* output = new Output[count];
	rawBuffer->CopyFromOutput(output);

	FILE* fileBuffer;
	fopen_s(&fileBuffer, "../ThreadID.csv", "w");
	{
		for (UINT i = 0; i < count; i++)
		{
			Output temp = output[i];
		

		fprintf
			(
			fileBuffer,
			"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%f\n",
			temp.GroupID[0], temp.GroupID[1], temp.GroupID[2],
			temp.GroupThreadID[0], temp.GroupThreadID[1], temp.GroupThreadID[2],
			temp.DispatchThreadID[0], temp.DispatchThreadID[1], temp.DispatchThreadID[2],
			temp.GroupIndex,
			temp.RetVal
			);//csv에쓸에 두번째꺼는 칸수와타입
		}

	}
	fclose(fileBuffer);
	

}

void RawBufferDemo::Destroy()
{

}