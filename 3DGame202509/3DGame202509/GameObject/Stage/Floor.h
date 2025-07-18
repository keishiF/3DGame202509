#pragma once
#include "StageObjectBase.h"

class Floor : public StageObjectBase
{
public:
	Floor();
	~Floor();

	virtual void Init(const std::string& modelPath, Vec3 pos, Vec3 rot, Vec3 scale) override;
	virtual void Draw() override;

private:
	int m_modelHandle = -1;
};

