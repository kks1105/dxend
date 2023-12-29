#pragma once
#include "Systems/IExecute.h"


class AnimationDemo : public IExecute
{
public:
	virtual void Initialize() override;
	virtual void Destroy() override;
	virtual void Update() override;
	virtual void PreRender() override {};
	virtual void Render() override;
	virtual void PostRender() override {};
	virtual void ResizeScreen() override {};

private:
	void Kachujin();

private:
	Shader* shader;

	ModelAnimator* kachujin = nullptr;

	ColliderObject* colliderObject;//콜라이더클래스내부구조체
	Matrix bones[MAX_MODEL_TRANSFORMS];
};