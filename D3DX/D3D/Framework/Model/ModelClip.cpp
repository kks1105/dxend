#include "Framework.h"
#include "ModelClip.h"

ModelKeyFrame* ModelClip::Keyframe(wstring boneName)
{
	if (keyframeMap.count(boneName) < 1)
		return nullptr;

	return keyframeMap[boneName];
}
