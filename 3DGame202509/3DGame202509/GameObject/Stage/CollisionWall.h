#pragma once
#include "StageObjectBase.h"
#include "Vec3.h"

class Physics;
class CollisionWall : public StageObjectBase
{
public:
	virtual void Init(const std::string& modelPath, Vec3 pos, Vec3 rot, Vec3 scale) override;
	virtual void Draw() override;

private:
	Vec3 m_start;
	Vec3 m_end;
};

