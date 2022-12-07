#include "EnemyController.h"
#include <time.h>

EnemyController::EnemyController()
{
}

EnemyController::~EnemyController()
{
}

Vector3 FloatRandom(int min, int max)
{
	srand(time(NULL));
	int randx = rand() % max + min;
	int randz = rand() % max + min;
	return Vector3(randx, 0.f, randz);
}

void EnemyController::Update()
{
	for (Enemy* enemy : enemies)
	{
		if (enemy->canMove)
		{
			enemy->wayPoint = FloatRandom(-20, 40);
			enemy->canMove = false;
		}
		enemy->Update();
	}
}

void EnemyController::ShootEnemy(GameObject* gameobject)
{
	for (Enemy* enemy : enemies)
	{
		if (enemy->enemy == gameobject)
		{
			enemy->Kill();
		}
	}
}

void EnemyController::Initialize(std::vector<GameObject*> gameObjectList, PhysicsSystem* physicsSystem)
{
	for (GameObject* gameObject : gameObjectList)
	{
		if (gameObject->name == "enemy")
		{
			Enemy* enemy = new Enemy();
			enemy->physicsObject = physicsSystem->CreatePhysicsObject(gameObject->transform->position, new Sphere(0, 2.f));
			enemy->enemy = gameObject;
			enemy->wayPoint = FloatRandom(-20,40);
			enemy->physicsObject->SetMass(1.f);
			enemies.push_back(enemy);
		}
	}
}
