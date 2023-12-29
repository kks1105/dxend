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

	void Pass(UINT val) { mesh->Pass(val); }//�н�

	Transform* AddTransform();//�ν��Ͻ̶�ȣ���Ҷ����ٺ��� Ư��Ʈ����������(����)
	Transform* GetTransform(UINT index) { return transforms[index]; }//�ν��Ͻ̳����� ��Ư���ν��Ͻ�Ʈ�����������ϴ¿���
	void UpdateTransforms();//Refresh instance Buffer �ν��Ͻ����ۿ� ����ȳ���˷��ֱ�

	void SetColor(UINT instance, Color& color);//�� ����


private:
	Mesh* mesh;

	vector<Transform*> transforms;
	Matrix worlds[MAX_MESH_INSTANCE];//500
	VertexBuffer* instanceWorldBuffer;

	Color colors[MAX_MESH_INSTANCE];//500
	VertexBuffer* instanceColorBuffer;

};
