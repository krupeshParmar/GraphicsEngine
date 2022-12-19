#include "Enemy.h"

Enemy::Enemy()
{
}

Enemy::~Enemy()
{
}

void Enemy::Update()
{
	if (!this->isAlive)
		return;

	if (
		this->enemy->transform->position.x <= this->wayPoint.x + 1 
			&&
		this->enemy->transform->position.x >= this->wayPoint.x - 1 
			&& 
		this->enemy->transform->position.z <= this->wayPoint.z + 1
			&& 
		this->enemy->transform->position.z >= this->wayPoint.z - 1
		)
	{
		if (glfwGetTime() - this->time > 5.f)
		{
			this->canMove = true;
		}
	}
	else {
		Vector3 direction = Vector3(
			this->wayPoint.x - this->physicsObject->GetPosition().x,
			0.f,
			this->wayPoint.z - this->physicsObject->GetPosition().z
		);
		direction.Normalize();
		this->physicsObject->ApplyForce(direction * speed);
		this->time = glfwGetTime();
		this->enemy->transform->position = this->physicsObject->GetPosition().GetGLM();
	}

}

void Enemy::Kill()
{
	this->isAlive = false;
}
