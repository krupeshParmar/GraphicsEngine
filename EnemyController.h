#pragma once
#include <vector>
#include "Enemy.h"
#include "Physics/PhysicsSystem.h"
#include "inc_opengl.h"
#include "Explosives.h"
#define XMIN -37
#define XMAX 37
#define ZMIN -2
#define ZMAX 33
#define MINSPEED 8
#define MAXSPEED 30
class EnemyController
{
public:
	EnemyController();
	~EnemyController();
	void Initialize(std::vector<GameObject*> gameObjectList, PhysicsSystem* physicsSystem);
	Explosives* explosives;
	void Update();
	void ShootEnemy(GameObject* gameobject, std::vector<GameObject*>* gameObjectList, PhysicsSystem* physicsSystem);
	std::vector<Enemy*> enemies;
};

