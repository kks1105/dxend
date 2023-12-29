#pragma once

class ModelRender
{
public:
	ModelRender(Shader* shader);
	~ModelRender();

	void Update();
	void Render();

public:
	void ReadMesh(wstring file);
	void ReadMaterial(wstring file);

	Model* GetModel() { return model; }


	Transform* AddTransform();
	Transform* GetTransform(UINT index) { return transforms[index]; }
	void UpdateTransforms(); //Refresh instance Buffer
	void SetColor(UINT instance, Color& color);
	UINT TransformsCount() { return transforms.size(); }

	void Pass(UINT pass);

private:
	void CreateTexture();


private:

	Shader* shader;
	Model* model;


	vector<Transform*> transforms;
	Matrix worlds[MAX_MESH_INSTANCE];
	VertexBuffer* instanceWorldBuffer;

	Color colors[MAX_MESH_INSTANCE];
	VertexBuffer* instanceColorBuffer;

	Matrix boneTransforms[MAX_MODEL_INSTANCE][MAX_MODEL_TRANSFORMS]; //Bone Transform(BS)

	ID3D11Texture2D* texture = nullptr;
	ID3D11ShaderResourceView* transformsSRV;
	ID3DX11EffectShaderResourceVariable* sTransformsSRV;

};
