#pragma once
#include "Collidable.h"
#include "Vector3.h"
#include <string>

class StageObjectBase : public Collidable
{
public:
	StageObjectBase();

	virtual void Init(const std::string& modelPath, Vector3 pos, Vector3 rot, Vector3 scale) abstract;
	virtual void Draw() abstract;

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override {};

	int GetModel() const { return m_modelHandle; }

private:
	int m_modelHandle;
};