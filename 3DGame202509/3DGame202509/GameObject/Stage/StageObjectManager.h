#pragma once
#include <memory>
#include <vector>

class Mountain;
class Floor;
class StageObjectManager
{
public:
	StageObjectManager();
	~StageObjectManager();

	void Init();
	void Update();
	void Draw();

private:
	std::vector<std::shared_ptr<Mountain>> m_mountains;
	std::vector<std::shared_ptr<Floor>> m_floors;
};

