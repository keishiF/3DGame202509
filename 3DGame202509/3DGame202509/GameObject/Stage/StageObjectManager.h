#pragma once
#include <memory>
#include <vector>

class Floor;
class Mountain;
class physics;
class StageObjectManager
{
public:
	StageObjectManager();
	~StageObjectManager();

	void Init();
	void Update();
	void Draw();
	void Finalize();

private:
	std::vector<std::shared_ptr<Floor>> m_floors;
	std::vector<std::shared_ptr<Mountain>> m_mountains;
};

