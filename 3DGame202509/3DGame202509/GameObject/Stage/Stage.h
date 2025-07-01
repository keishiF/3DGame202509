#pragma once

#include "StageObject.h"
#include <vector>
#include <memory>

enum class StageType : int
{
	Tutorial,
	Stage1
};

class Stage
{
public:
	void Init();
	void Draw();

private:
	std::vector<StageObject> m_objects;
};

