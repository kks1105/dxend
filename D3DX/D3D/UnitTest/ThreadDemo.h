#pragma once
#include "Systems/IExecute.h"

class ThreadDemo : public IExecute
{
public:
	virtual void Initialize() override;
	virtual void Destroy() override {};
	virtual void Update() override;
	virtual void PreRender() override {};
	virtual void Render() override {};
	virtual void PostRender() override {};
	virtual void ResizeScreen() override {};

private:
	void Loop1();
	void Loop2();
	void MultiThread();

	void PrintProgress();

	void Thread(int& count);
	void RaceCondition();

private:
	float progress = 0.f; //shared Resource

	mutex m;

};