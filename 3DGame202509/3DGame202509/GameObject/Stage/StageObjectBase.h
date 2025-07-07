#pragma once
#include "Vec3.h"
#include <string>

class StageObjectBase
{
public:
	StageObjectBase();

	virtual void Init(const std::string& modelPath, Vec3 pos, Vec3 rot, Vec3 scale) abstract;
	virtual void Draw() abstract;

private:
	int m_modelHandle;
};