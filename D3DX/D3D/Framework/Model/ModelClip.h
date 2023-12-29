#pragma once

//1 Bone, 1 Frame
struct ModelKeyFrameData
{
	float Frame;

	Vector3 Scale;
	Quaternion Rotation;
	Vector3 Translation;
};

//1 Bone, All Frame
struct ModelKeyFrame
{
	wstring BoneName;
	vector<ModelKeyFrameData> Transforms;
};

//All Bone, All Frame(Final)
class ModelClip
{
public:
	friend class Model;

public:
	ModelClip() = default;
	~ModelClip() = default;;

	UINT FrameCount() { return frameCount; }
	float FrameRate() { return frameRate; }

	ModelKeyFrame* Keyframe(wstring boneName);

private:
	wstring name;

	UINT frameCount;
	float frameRate;

	unordered_map<wstring, ModelKeyFrame*> keyframeMap; //Key : BoneName, Value : All Frame
};