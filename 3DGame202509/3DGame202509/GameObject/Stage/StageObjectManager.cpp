#include "CollisionWall.h"
#include "Floor.h"
#include "Mountain.h"
#include "Physics.h"
#include "StageObjectManager.h"
#include "TransformDataLoader.h"
#include <memory>

StageObjectManager::StageObjectManager()
{
}

StageObjectManager::~StageObjectManager()
{
}

void StageObjectManager::Init()
{
	TransformDataLoader loader;
	auto transformDataList = loader.LoadDataCSV("Data/CSV/StageTransformData.csv");

	for (const auto& data : transformDataList)
	{
		std::string modelPath;

		if (data.name == "Mountain")
		{
			modelPath = "Data/Model/Stage/Mountain/Mountain.mv1";
			auto mountain = std::make_shared<Mountain>();
			Vec3 pos = { data.pos.x, data.pos.y, data.pos.z };
			Vec3 rot = { data.rot.x, data.rot.y, data.rot.z };
			Vec3 scale = { data.scale.x, data.scale.y , data.scale.z };
			mountain->Init(modelPath, pos, rot, scale);
			m_mountains.emplace_back(mountain);
		}
		else if (data.name == "Plane")
		{
			modelPath = "Data/Model/Stage/Floor/Plane.mv1";
			auto floor = std::make_shared<Floor>();
			Vec3 pos = { data.pos.x, data.pos.y, data.pos.z };
			Vec3 rot = { data.rot.x, data.rot.y, data.rot.z };
			Vec3 scale = { data.scale.x, data.scale.y, data.scale.z};
			floor->Init(modelPath, pos, rot, scale);
			m_floors.emplace_back(floor);
		}
		else if (data.name == "Wall")
		{
			modelPath = "a";
			auto wall = std::make_shared<CollisionWall>();
			Vec3 pos = { data.pos.x, data.pos.y + 50.0f, data.pos.z };
			Vec3 rot = { data.rot.x, data.rot.y, data.rot.z };
			Vec3 scale = { data.scale.x, data.scale.y, data.scale.z };
			wall->Init(modelPath, pos, rot, scale);
			m_walls.emplace_back(wall);
		}
	}
}

void StageObjectManager::Update()
{
}

void StageObjectManager::Draw()
{
	for (auto& mountain : m_mountains)
	{
		mountain->Draw();
	}

	for (auto& floor : m_floors)
	{
		floor->Draw();
	}

	for (auto& wall : m_walls)
	{
		wall->Draw();
	}
}