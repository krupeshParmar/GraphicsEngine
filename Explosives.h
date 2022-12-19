#pragma once
#include "GameObject/GameObject.h"
#include "Physics/PhysicsObject.h"
#include "inc_opengl.h"
#include <vector>
class Explosives
{
public:
	Explosives();
	~Explosives();
	GameObject* explosive_prefab;
	std::vector<GameObject*> createdExplosives;
	std::vector<GameObject*>* p_DrawList;
	void CreateExplosion(glm::vec3 position, std::vector<GameObject*>* drawList);
	void Update(float dt);
	const float removeOffset = 0.001f;
};

