#pragma once
#include "StageObjectBase.h"

class CollisionWall : public StageObjectBase
{
public:
	virtual void Init(const std::string& modelPath, Vec3 pos, Vec3 rot, Vec3 scale) override;
	virtual void Draw() override {};
};

