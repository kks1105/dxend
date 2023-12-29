#pragma once

//-----------------------------------------------------------------------------
// ModelBone
// @ *.mesh의 bones data를 저장받을 클래스
// @ Tree 형태로 저장
//-----------------------------------------------------------------------------
class ModelBone
{
public:
	friend class Model;

private:
	ModelBone() = default;
	~ModelBone() = default;

public:
	int Index() { return index; }

	int ParentIndex() { return parentIndex; }
	ModelBone* Parent() { return parent; }

	wstring Name() { return name; }

	Matrix& Transform() { return transform; }
	void Transform(Matrix& matrix) { transform = matrix; }

	vector<ModelBone*>& Children() { return children; }

private:
	int index;
	wstring name;

	int parentIndex;
	ModelBone* parent;

	Matrix transform;
	vector<ModelBone*> children;
};

//-----------------------------------------------------------------------------
// ModelMesh
// *.mesh의 mesh data를 저장받을 클래스
// VertexBuffer, IndexBuffer 생성
// 모든 Bone의 Matrix를 boneBuffer(cBuffer)로 복사
// Trasnform 객체 생성(Root Bone)
//-----------------------------------------------------------------------------
class ModelMesh
{
public:
	friend class Model;

private:
	ModelMesh();
	~ModelMesh();

	//(1) 이 Mesh를 그리기 위한 VertexBuffer, IndexBuffer 생성
	//(2) meshPart->Binding() 호출, 머티리얼이 각각 할당
	void Binding(Model* model); 

public:
	//파이프라인 변경용
	void Pass(UINT val);

	//쉐이더가 변경되면 transform, perFrame, cbuffer도 변경된 쉐이더로 적용되어야 함
	void SetShader(Shader* shader);

	//현재 메쉬가 그려져야 할 boneIndex를 Get
	void Update();

	//렌더링 자원들을 쉐이더로 전송
	void Render();
	void Render(UINT drawCount);

	int BoneIndex() { return boneIndex; }
	ModelBone* Bone() { return bone; }

	void Transforms(Matrix* transforms); //cbuffer의 전체 Bone 배열 세팅용
	void SetTransform(Transform* transform); //Root Bone 위치 세팅용

private:
	struct BoneDesc
	{
		Matrix Transforms[MAX_MODEL_TRANSFORMS]; //전체 Bone의 배열

		UINT BoneIndex; //현재 내가 붙어야할 BoneIndex
		float Padding[3];
	} boneDesc;

private:
	Shader* shader;

	Transform* transform = nullptr; //Root Bone(W)
	PerFrame* perFrame = nullptr; //VP

	int boneIndex;
	ModelBone* bone;

	VertexBuffer* vertexBuffer;
	UINT vertexCount;
	Model::VertexModel* vertices;

	IndexBuffer* indexBuffer;
	UINT indexCount;
	UINT* indices;

	ConstantBuffer* boneBuffer;
	ID3DX11EffectConstantBuffer* sBoneBuffer;

	vector<class ModelMeshPart*> meshParts;
};

//-----------------------------------------------------------------------------
// ModelMeshPart
// @ 실제 Draw Primitive Call
// @ Material 단위로 구별됨
// @ 딱 자기 부위만 그릴거다
//-----------------------------------------------------------------------------
class ModelMeshPart
{
public:
	friend class Model;
	friend class ModelMesh;

private:
	ModelMeshPart();
	~ModelMeshPart();

	void Update();
	void Render();
	void Render(UINT drawCount);

	//DiffuseMap, NormalMap 텍스쳐 쉐이더와 연결
	void Binding(Model* model);

	//Draw Primitive Call을 담당할 쉐이더 세팅용
	void SetShader(Shader* shader);

	//파이프라인 변경용
	void Pass(UINT val) { pass = val; }

private:
	Shader* shader;
	UINT pass = 0;

	wstring materialName;
	Material* material = nullptr;

	UINT startVertex;
	UINT vertexCount;

	UINT startIndex;
	UINT indexCount;
};