#pragma once

struct ColliderObject
{
	class Transform* Init = nullptr;
	class Transform* World = nullptr;
	class Collider* Collision = nullptr;

public:
	ColliderObject();
	~ColliderObject();
};

class Collider
{
public:
	Collider(Transform* transform, Transform* init = nullptr);
	~Collider();

	void Render(Color color = Color(0, 1, 0, 1));

private:
	Transform* transform;
	Transform* init;

	Vector3 lines[8];

};