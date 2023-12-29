#pragma once

#define MAX_MESH_INSTANCE 500

class MeshRender
{
public:
	MeshRender(Shader* shader,Mesh* mesh);
	~MeshRender();

	Mesh* GetMesh() { return mesh; }

	void Update();
	void Render();

	void Pass(UINT val) { mesh->Pass(val); }//패스

	Transform* AddTransform();//인스턴싱때호출할때마다복제 특정트랜스폼으로(스폰)
	Transform* GetTransform(UINT index) { return transforms[index]; }//인스턴싱넣으면 그특정인스턴싱트랜스폼리턴하는역할
	void UpdateTransforms();//Refresh instance Buffer 인스턴스버퍼에 변경된내용알려주기

	void SetColor(UINT instance, Color& color);//색 세팅


private:
	Mesh* mesh;

	vector<Transform*> transforms;
	Matrix worlds[MAX_MESH_INSTANCE];//500
	VertexBuffer* instanceWorldBuffer;

	Color colors[MAX_MESH_INSTANCE];//500
	VertexBuffer* instanceColorBuffer;

};
