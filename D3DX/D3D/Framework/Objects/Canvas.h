#pragma once

class Canvas : public Renderer
{
public:
	Canvas();
	~Canvas();

	void Update();
	void Render();

	void SRV(ID3D11ShaderResourceView* srv); //srv ¼¼ÆÃ


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
	
	} desc; //ºä¿Í ÇÁ·ÎÁ§¼Ç



		//Matrix ViewCanvas;
		//	Matrix ProjectionCanvas;
private:
	ConstantBuffer* buffer; //Äµ¹ö½º
	ID3DX11EffectConstantBuffer* sBuffer; //ºä


	ID3DX11EffectShaderResourceVariable* sSrv;//srv¹öÆÛ
};