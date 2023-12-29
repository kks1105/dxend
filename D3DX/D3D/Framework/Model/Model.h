#pragma once

#define MAX_MODEL_TRANSFORMS 250
#define MAX_MODEL_KEYFRAMES 500
#define MAX_MODEL_INSTANCE 500

class ModelBone;
class ModelMesh;
class ModelMeshPart;
class ModelClip;

class Model
{
public:
	friend class ModelRender;
	friend class ModelAnimator;

private:
	Model();
	~Model();

private:
	void ReadMesh(wstring file);
	void ReadMaterial(wstring file);
	void ReadClip(wstring file);

private:
	void BindBone(); //Make Bone Tree
	void BindMesh(); //(1)meshes[i]->Binding() »£√‚, (2) boneIndex∏¶ Set

public:
	UINT BoneCount() { return bones.size(); }
	vector<ModelBone*>& Bones() { return bones; }
	ModelBone* BoneByIndex(UINT index) { return bones[index]; }
	ModelBone* BoneByName(wstring name);

	UINT MeshCount() { return meshes.size(); }
	vector<ModelMesh*>& Meshes() { return meshes; }
	ModelMesh* MeshByIndex(UINT index) { return meshes[index]; }

	UINT MaterialCount() { return materials.size(); }
	vector<Material*>& Materials() { return materials; }
	Material* MaterialByIndex(UINT index) { return materials[index]; }
	Material* MaterialByName(wstring name);

	UINT ClipCount() { return clips.size(); }
	vector<ModelClip*>& Clips() { return clips; }
	ModelClip* ClipByIndex(UINT index) { return clips[index]; }
	ModelClip* ClipByName(wstring name);

public:
	struct VertexModel
	{
		Vector3 Position;
		Vector2 Uv;
		Vector3 Normal;
		Vector3 Tangent;
		Vector4 BlendIndices;
		Vector4 BlendWeights;
	};

private:
	ModelBone* root;

	vector<ModelBone*> bones;
	vector<ModelMesh*> meshes;
	vector<Material*> materials;
	vector<ModelClip*> clips;
};