#pragma once
#include "StageObjectBase.h"
#include <vector>

class Mountain : public StageObjectBase
{
public:
    Mountain();
    ~Mountain();

    virtual void Init(const std::string& modelPath, Vec3 pos, Vec3 rot, Vec3 scale) override;
    virtual void Draw() override;

private:
    int m_modelHandle = -1;
};

