#pragma once
#include <vector>
#include "Enemy.h"
#include "Physics/PhysicsSystem.h"
#include "inc_opengl.h"
class EnemyController
{
public:
	EnemyController();
	~EnemyController();
	void Initialize(std::vector<GameObject*> gameObjectList, PhysicsSystem* physicsSystem);
	void Update();
	void ShootEnemy(GameObject* gameobject);
	std::vector<Enemy*> enemies;
};

