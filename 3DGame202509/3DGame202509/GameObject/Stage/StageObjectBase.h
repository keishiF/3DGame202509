#pragma once
#include "Collidable.h"
#include "Vec3.h"
#include <string>

class StageObjectBase : public Collidable
{
public:
	StageObjectBase();

	virtual void Init(const std::string& modelPath, Vec3 pos, Vec3 rot, Vec3 scale) abstract;
	virtual void Draw() abstract;

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override {};

private:
	int m_modelHandle;
};