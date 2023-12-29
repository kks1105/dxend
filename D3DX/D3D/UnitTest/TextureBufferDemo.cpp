#include "stdafx.h"
#include "TextureBufferDemo.h"
#include "Objects/Canvas.h"

void TextureBufferDemo::Initialize()
{
	shader = new Shader(L"17_TextureBuffer.fxo");

	inputTexture = new Texture(L"Environment/Compute.png");
	
	textureBuffer = new TextureBuffer(inputTexture->GetTexture());//texture

	shader->AsSRV("Input")->SetResource(textureBuffer->SRV()); //srv
	shader->AsUAV("Output")->SetUnorderedAccessView(textureBuffer->UAV()); //uav

	canvas = new Canvas(); //2d 캔버스
	canvas->GetTransform()->Scale(D3D::Width(), D3D::Height(),1);
	canvas->GetTransform()->Position(D3D::Width()*0.5f, D3D::Height()*0.5f, 0.f);
	canvas->SRV(textureBuffer->OutputSRV());

	/// <summary>
	/// /////////////////////////////////////////////////////////////////////////////////
	/// </summary>
	{
		UINT width = textureBuffer->Width(); //텍스쳐의위스
		UINT height = textureBuffer->Height();

		float x = width / 32;
		float y = height / 32;

		UINT groupX = (UINT)ceilf(x);//uint변환
		UINT groupY = (UINT)ceilf(y);
		shader->Dispatch(0, 0, groupX, groupY, 1);

	}
}

void TextureBufferDemo::Destroy()
{
	SafeDelete(shader);
	SafeDelete(inputTexture);
	SafeDelete(textureBuffer);
	SafeDelete(canvas);


}

void TextureBufferDemo::Update()
{
	UINT width = textureBuffer->Width(); //텍스쳐의위스
	UINT height = textureBuffer->Height();

	float x = width / 32; 
	float y = height / 32;

	UINT groupX = (UINT)ceilf(x);//uint변환
	UINT groupY = (UINT)ceilf(y);

	

	//shader->Dispatch(0, 0, groupX, groupY, 1); 

	if (ImGui::Button("Diffuse"))
		shader->Dispatch(0, 0, groupX, groupY, 1);

	if (ImGui::Button("Inverse"))
		shader->Dispatch(0, 1, groupX, groupY, 1);

	if (ImGui::Button("RedChannel"))
		shader->Dispatch(0, 2, groupX, groupY, 1);

	if (ImGui::Button("GrayScale"))
		shader->Dispatch(0, 3, groupX, groupY, 1);


	canvas->Update();
	//textureBuffer->CopyFromOutput();

}

void TextureBufferDemo::Render()
{
	canvas->Render();


}
