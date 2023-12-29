#pragma once

class Canvas : public Renderer
{
public:
	Canvas();
	~Canvas();

	void Update();
	void Render();

	void SRV(ID3D11ShaderResourceView* srv); //srv ����


private:
	struct VertexCanvas
	{
		Vector3 Position;
		Vector2 Uv;
		
	};

private:
	struct Desc
	{
		Matrix View;
		Matrix Projection;
	
	} desc; //��� ��������



		//Matrix ViewCanvas;
		//	Matrix ProjectionCanvas;
private:
	ConstantBuffer* buffer; //ĵ����
	ID3DX11EffectConstantBuffer* sBuffer; //��


	ID3DX11EffectShaderResourceVariable* sSrv;//srv����
};