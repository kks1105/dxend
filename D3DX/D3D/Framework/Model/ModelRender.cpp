#include "Framework.h"
#include "ModelRender.h"

ModelRender::ModelRender(Shader* shader)
	: shader(shader)
{
	model = new Model();

	sTransformsSRV = shader->AsSRV("TransformsMap");

	instanceWorldBuffer = new VertexBuffer(worlds, MAX_MODEL_INSTANCE, sizeof(Matrix), 1, true);
	instanceColorBuffer = new VertexBuffer(colors, MAX_MODEL_INSTANCE, sizeof(Color), 2, true);

}

ModelRender::~ModelRender()
{
	SafeDelete(model);

	SafeDelete(instanceWorldBuffer);
	SafeDelete(instanceColorBuffer);

	//SafeDelete(texture);
	//SafeDelete(transformsSRV);

	for (Transform* transform : transforms)
		SafeDelete(transform);
}

void ModelRender::Update()
{
	if (texture == nullptr)
	{

		for (ModelMesh* mesh : model->meshes)
			mesh->SetShader(shader);

		CreateTexture();
	}

	for (ModelMesh* mesh : model->meshes)
		mesh->Update();
}

void ModelRender::Render()
{
	instanceWorldBuffer->Render();
	instanceColorBuffer->Render();
	sTransformsSRV->SetResource(transformsSRV);

	for (ModelMesh* mesh : model->meshes)
	{
		mesh->Render(transforms.size());
	}
}

void ModelRender::ReadMesh(wstring file)
{
	model->ReadMesh(file);
}

void ModelRender::ReadMaterial(wstring file)
{

	model->ReadMaterial(file);
}

void ModelRender::Pass(UINT pass)
{
	for (ModelMesh* mesh : model->Meshes())
		mesh->Pass(pass);
}

void ModelRender::CreateTexture()
{
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
	desc.Width = MAX_MODEL_TRANSFORMS * 4;
	desc.Height = MAX_MODEL_INSTANCE;
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; //16 Byte
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;

	for (UINT i = 0; i < MAX_MODEL_INSTANCE; i++)
	{
		for (UINT b = 0; b < model->BoneCount(); b++)
		{
			//MeshBone을 얻어온다
			ModelBone* bone = model->BoneByIndex(b);
			boneTransforms[i][b] = bone->Transform(); //텍스쳐에탈 서브리소스데이터

		}
	}
	D3D11_SUBRESOURCE_DATA subRousrce;
	subRousrce.pSysMem = boneTransforms;
	subRousrce.SysMemPitch = sizeof(Matrix) * MAX_MODEL_TRANSFORMS;
	subRousrce.SysMemSlicePitch = sizeof(Matrix) * MAX_MODEL_TRANSFORMS * MAX_MODEL_INSTANCE;

	Check(D3D::GetDevice()->CreateTexture2D(&desc, &subRousrce, &texture));//만들기

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	Check(D3D::GetDevice()->CreateShaderResourceView(texture, &srvDesc, &transformsSRV));//쉐이더리쇼스뷰

}

Transform* ModelRender::AddTransform()
{
	Transform* transform = new Transform();
	transforms.push_back(transform);

	colors[transforms.size() - 1] = Color(0, 0, 0, 1);

	return transform;
}

void ModelRender::UpdateTransforms()
{
	for (UINT i = 0; i < transforms.size(); i++)
		worlds[i] = transforms[i]->World();

	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::GetDC()->Map(instanceWorldBuffer->Buffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	{
		memcpy(subResource.pData, worlds, sizeof(Matrix) * MAX_MODEL_INSTANCE);
	}
	D3D::GetDC()->Unmap(instanceWorldBuffer->Buffer(), 0);

	D3D::GetDC()->Map(instanceColorBuffer->Buffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	{
		memcpy(subResource.pData, colors, sizeof(Color) * MAX_MODEL_INSTANCE);
	}
	D3D::GetDC()->Unmap(instanceColorBuffer->Buffer(), 0);
}

void ModelRender::SetColor(UINT instance, Color& color)
{
	colors[instance] = color;

}
