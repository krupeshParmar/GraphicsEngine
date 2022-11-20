#include "GameObject.h"

void GameObject::Update()
{
	PoRoSc porosc;
	porosc.pos = transform->position;
	porosc.rot = transform->rotation;
	porosc.sca = transform->scale;

	if (parent != nullptr)
	{
		porosc.pos += parent->transform->position;
		porosc.rot += parent->transform->rotation;
	}
	for (int i = 0; i < components.size(); i++)
	{
		components[i]->Update(porosc);
	}
}

GameObject::GameObject()
{
	transform = new Transform();
	transform->Reset();
	parent = nullptr;
	this->name = "";
	this->meshObject = new MeshObject();
}


GameObject::GameObject(MeshObject* mesh)
{
	transform = new Transform();
	transform->Reset();
	parent = nullptr;
	this->meshObject = mesh;
}

GameObject::GameObject(MeshObject* mesh, Transform* trans)
{
	transform = new Transform();
	transform->Reset();
	parent = nullptr;
	this->meshObject = mesh;
	this->transform = trans;
}

GameObject::~GameObject()
{

}

void GameObject::CreateCopy(GameObject* gameObject)
{

}
