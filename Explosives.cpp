#include "Explosives.h"

Explosives::Explosives()
{
}

Explosives::~Explosives()
{
}

void Explosives::CreateExplosion(glm::vec3 position, std::vector<GameObject*>* drawList)
{
	if (explosive_prefab == nullptr)
		return;
	p_DrawList = drawList;
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			for (int k = 0; k < 2; k++)
			{
				GameObject* explosive = new GameObject();
				explosive->transform->position = position - glm::vec3(i, j - position.y, k);
				explosive->meshObject->meshName = explosive_prefab->meshObject->meshName;
				explosive->meshObject->bUse_RGBA_color = true;
				explosive->meshObject->RGBA_color = explosive_prefab->meshObject->RGBA_color;
				explosive->meshObject->bDoNotLight = false;

				drawList->push_back(explosive);
				createdExplosives.push_back(explosive);
			}
		}
	}
}

void Explosives::Update(float dt)
{
	if (explosive_prefab == nullptr)
		return;
	std::vector<GameObject*> toBeErased;
	int pos = 0;
	for (std::vector<GameObject*>::iterator explosivesIterator = createdExplosives.begin();
		explosivesIterator != createdExplosives.end();
		explosivesIterator++)
	{
		GameObject* expPar = *explosivesIterator;
		if (expPar->transform->scale.y < removeOffset)
		{
			toBeErased.push_back(expPar);
			//p_DrawList->erase(explosivesIterator);
		}
		else {
			expPar->transform->scale -= glm::vec3(0.01f);

		}
		pos++;
	}
	for (int i = 0; i < toBeErased.size(); i++)
	{
		std::vector<GameObject*>::iterator iter;
		for (iter = p_DrawList->begin(); iter != p_DrawList->end(); iter++)
		{
			if (*iter == toBeErased[i])
				break;
		}
		if (iter != p_DrawList->end()) {
			p_DrawList->erase(iter);
		}
	}
}
