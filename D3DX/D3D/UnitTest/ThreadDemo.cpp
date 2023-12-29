#include "stdafx.h"
#include "ThreadDemo.h"

//�����Ҷ���� �ݼָ��ιٲ��ִ¿���
//#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#include <iostream>
using namespace std;

void ThreadDemo::Initialize()
{
	//cout << "HAHA" << endl;
	//MultiThread();

	//PrintProgress();


	RaceCondition();
}



void ThreadDemo::Update()
{
	progress += 0.1f;
	ImGui::ProgressBar(progress / 1000.f);
}

void ThreadDemo::Loop1()
{
	for (int i = 0; i < 100; i++)
		printf("Loop 1:%d\n", i+1);
	printf("Loop1complate");


}


void ThreadDemo::Loop2()
{
	for (int i = 0; i < 100; i++)
		printf("Loop 2:%d\n", i+1);
	printf("Loop2complate");
}

void ThreadDemo::MultiThread()
{
	thread t1(bind(&ThreadDemo::Loop1, this));
	thread t2(bind(&ThreadDemo::Loop2, this));

	t2.join();
	printf("t2 thread is end");


	
	t1.join();
	printf("t1 thread is end");



}

void ThreadDemo::PrintProgress()
{
	thread t([=]() {while (true) { Sleep(100); printf("Progress : %f\n", progress); if (progress >= 1000) { printf("Done!!!\n"); break; } }});

	t.detach();


}

void ThreadDemo::Thread(int& count)
{
	lock_guard<mutex> lock(m); //��������ڵ�ȣ�� �Լ��������̴ϼȶ�����������
	//m.lock();//�ٸ������尡�����°ɸ���
	for (int i = 0; i < 1000000; i++)
		count++;
	//m.unlock();//������Ǯ����

}

void ThreadDemo::RaceCondition()
{
	int sharedResource = 0;

	vector<thread> workers;

	for (int i = 0; i < 4; i++)
	{
		function<void(int&)> onThread = bind(&ThreadDemo::Thread, this,placeholders::_1);

		workers.push_back(thread(onThread, ref(sharedResource)));
	}

	for (int i = 0; i < workers.size(); i++)
		workers[i].join();

	printf("SharedResource:%d", sharedResource);
}

