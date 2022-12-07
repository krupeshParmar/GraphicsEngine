#pragma once
#include "GameObject/GameObject.h"
#include "Physics/PhysicsObject.h"
#include "inc_opengl.h"
class Enemy
{
public:
	Enemy();
	~Enemy();
	bool isAlive = true;
	Vector3 wayPoint;
	GameObject* enemy;
	PhysicsObject* physicsObject;
	void Update();
	void Kill();
	double time = 0;
	bool canMove = false;
};
