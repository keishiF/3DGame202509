#pragma once
#include "StageObjectBase.h"

class Floor : public StageObjectBase
{
public:
	Floor();
	~Floor();

	virtual void Init(const std::string& modelPath, Vector3 pos, Vector3 rot, Vector3 scale) override;
	virtual void Draw() override;

private:
	int m_model = -1;
};

