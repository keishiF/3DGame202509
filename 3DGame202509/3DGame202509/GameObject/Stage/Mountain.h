#pragma once
#include "StageObjectBase.h"
#include <vector>

class Mountain : public StageObjectBase
{
public:
    Mountain();
    ~Mountain();

    virtual void Init(const std::string& modelPath, Vector3 pos, Vector3 rot, Vector3 scale) override;
    virtual void Draw() override;

private:
    int m_model= -1;
};

