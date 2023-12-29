#pragma once

class Converter
{
public:
	Converter();
	~Converter();

	//fbx 파일 읽어오기
	void ReadFile(wstring file);

	//scene의 meshes, bones 정보를 *.mesh
public:
	void ExportMesh(wstring savePath);

private:
	void ReadBoneData(aiNode* node, int index, int parent);
	void ReadMeshData(aiNode* node, int index);
	void ReadSkinData();
	void WriteMeshData(wstring savePath);

	//scene의 maetrial 정보를 *.material(xml)
public:
	void ExportMaterial(wstring savePath, bool bOverWrite = true);

private:
	void ReadMaterialData();
	bool FoundMaterialData(aiMaterial* material);
	void WriteMaterialData(wstring savePath);
	string WriteTexture(string saveFolder, string file);

	//(Animation.fbx)scene의 Clipo 정보를 *.clip
public:
	void ExportAnimClip(UINT index, wstring savePath);

private:
	struct asClip* ReadClipData(aiAnimation* animation); //Keyframe 저장
	void ReadKeyframeData(struct asClip* clip, aiNode* node, vector<struct asClipNode>& aniNodeInfos); //Retarget
	void WriteClipData(struct asClip* clip, wstring savePath);

private:

private:
	wstring file;

	Assimp::Importer* importer;
	const aiScene* scene;

	vector<struct asBone*> bones;
	vector<struct asMesh*> meshes;
	vector<struct asMaterial*> materials;
};