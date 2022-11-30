#pragma once
#include "System/Components/iComponent.h"
#include "Physics/BoundingBox/BoundingBox.h"
#include <string>
class BoxCollider : public iComponent
{
public:
	std::string box_model_path;
	BoundingBox boundingBox;
	BoxCollider();

	virtual void Reset();
	virtual void CopyComponent();
	virtual void GetParentValues();
	virtual void Update(PoRoSc porosc);

};

