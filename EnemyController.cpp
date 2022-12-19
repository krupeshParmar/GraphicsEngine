#include "EnemyController.h"
#include <time.h>

EnemyController::EnemyController()
{
	srand(glfwGetTime());
	explosives = new Explosives();
}

EnemyController::~EnemyController()
{
}

Vector3 FloatRandom()
{
	int randx = rand() % XMAX + XMIN;
	int randz = rand() % ZMAX + ZMIN;
	return Vector3(randx, 1.f, randz);
}

void EnemyController::Update()
{
	explosives->Update(1.f);
	for (Enemy* enemy : enemies)
	{
		if (enemy->canMove)
		{
			enemy->wayPoint = FloatRandom();
			enemy->canMove = false;
		}
		enemy->Update();
		if (!enemy->isAlive)
		{
			enemy->enemy->transform->position = FloatRandom().GetGLM();
			enemy->physicsObject->position = Vector3(enemy->enemy->transform->position);
			enemy->isAlive = true;
			enemy->canMove = false;
			enemy->wayPoint = FloatRandom();
		}
	}
}

void EnemyController::ShootEnemy(GameObject* gameobject, std::vector<GameObject*>* gameObjectList, PhysicsSystem* physicsSystem)
{
	for (Enemy* enemy : enemies)
	{
		if (enemy->enemy == gameobject)
		{
			enemy->Kill();
			enemy->speed = rand() % MAXSPEED + MINSPEED;
			explosives->CreateExplosion(enemy->enemy->transform->position, gameObjectList);
		}
	}
}

void EnemyController::Initialize(std::vector<GameObject*> gameObjectList, PhysicsSystem* physicsSystem)
{
	for (GameObject* gameObject : gameObjectList)
	{
		if (gameObject->name == "exp_prefab")
			explosives->explosive_prefab = gameObject;
		if (gameObject->name == "enemy")
		{
			Enemy* enemy = new Enemy();
			enemy->physicsObject = physicsSystem->CreatePhysicsObject(gameObject->transform->position, new Sphere(0, 2.f));
			enemy->enemy = gameObject;
			enemy->wayPoint = FloatRandom();
			enemy->physicsObject->SetMass(1.f);
			enemy->speed = rand() % MAXSPEED + MINSPEED;
			enemies.push_back(enemy);
		}
	}
}
