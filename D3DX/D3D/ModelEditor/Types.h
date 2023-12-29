#pragma once

#include "stdafx.h"

//-----------------------------------------------------------------------------
//Mesh
//-----------------------------------------------------------------------------
struct asBone
{
	int Index;
	string Name;

	int Parent;
	Matrix Transform;
};

struct asMeshPart
{
	string MaterialName;

	UINT StartVertex;
	UINT VertexCount;
	UINT StartIndex;
	UINT IndexCount;
};

struct asMesh
{
	int BoneIndex;

	vector<Model::VertexModel> Vertices;
	vector<UINT> Indices;

	vector<asMeshPart*> MeshParts;
};

//-----------------------------------------------------------------------------
//Material
//-----------------------------------------------------------------------------
struct asMaterial
{
	string Name;

	Color Ambient;
	Color Diffuse;
	Color Specular;
	Color Emissive;

	string DiffuseFile;
	string SpecularFile;
	string NormalFile;
};

//-----------------------------------------------------------------------------
//Animation
//-----------------------------------------------------------------------------
//1 Bone, 1 Frame
struct asKeyFrameData
{
	float Frame;

	Vector3 Scale;
	Quaternion Rotation;
	Vector3 Translation;
};

//1 Bone, All Frame
struct asKeyFrame
{
	string BoneName;
	vector<asKeyFrameData> Transforms;
};

//All Bone, All Frame(Final)
struct asClip
{
	string Name;

	UINT FrameCount;
	float FrameRate;

	vector<asKeyFrame*> Keyframes;
};

//1 Bone, All Frame(Temp)
struct asClipNode
{
	aiString Name; //animBone Name
	vector<asKeyFrameData> Keyframe;
};

//-----------------------------------------------------------------------------
//Skinning
//-----------------------------------------------------------------------------
//Write Bone Weights in Vertex(Temp)
struct asBlendWeight
{
	Vector4 Indices = Vector4(0, 0, 0, 0);
	Vector4 Weights = Vector4(0, 0, 0, 0);

	void Set(UINT iterator, UINT boneIndex, float weight)
	{
		float i = (float)boneIndex;
		float w = weight;

		switch (iterator)
		{
			case 0: Indices.x = i; Weights.x = w; break;
			case 1: Indices.y = i; Weights.y = w; break;
			case 2: Indices.z = i; Weights.z = w; break;
			case 3: Indices.w = i; Weights.w = w; break;
		}
	}
};

//Calculate Bone Weights -> Normalize(0~1) & Desc Sort
struct asBoneWeights
{
private:
	typedef pair<int, float> Pair;
	vector<Pair> BoneWeigths; //BoneIndex, BoneWeihgt

public:
	void AddWeights(UINT boneIndex, float boneWeights)
	{
		if (boneWeights <= 0.f) return;

		bool bAdd = false;

		vector<Pair>::iterator it = BoneWeigths.begin();
		while (it != BoneWeigths.end())
		{
			if (boneWeights > it->second)
			{
				BoneWeigths.insert(it, Pair(boneIndex, boneWeights));
				bAdd = true;

				break;
			}

			it++;
		}

		if (bAdd == false)
			BoneWeigths.push_back(Pair(boneIndex, boneWeights));
	}

	void Normalize()
	{
		float totalWeight = 0.f;
		int i = 0;

		vector<Pair>::iterator it = BoneWeigths.begin();
		while (it != BoneWeigths.end())
		{
			if (i < 4)
			{
				totalWeight += it->second;
				i++;  it++;
			}
			else
				it = BoneWeigths.erase(it);
		}

		float scale = 1.f / totalWeight;

		it = BoneWeigths.begin();
		while (it != BoneWeigths.end())
		{
			it->second *= scale;
			it++;
		}
	}

	void GetBlendWeigths(asBlendWeight& outBlendWeights)
	{
		for (UINT i = 0; i < BoneWeigths.size(); i++)
		{
			if (i >= 4) return;

			outBlendWeights.Set(i, BoneWeigths[i].first, BoneWeigths[i].second);
		}
	}

};