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
			Vector3 pos = { data.pos.x, data.pos.y, data.pos.z };
			Vector3 rot = { data.rot.x, data.rot.y, data.rot.z };
			Vector3 scale = { data.scale.x, data.scale.y , data.scale.z };
			mountain->Init(modelPath, pos, rot, scale);
			m_mountains.emplace_back(mountain);
		}
		else if (data.name == "Plane")
		{
			modelPath = "Data/Model/Stage/Floor/Plane.mv1";
			auto floor = std::make_shared<Floor>();
			Vector3 pos = { data.pos.x, data.pos.y, data.pos.z };
			Vector3 rot = { data.rot.x, data.rot.y, data.rot.z };
			Vector3 scale = { data.scale.x, data.scale.y, data.scale.z};
			floor->Init(modelPath, pos, rot, scale);
			m_floors.emplace_back(floor);
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
}

void StageObjectManager::Finalize()
{
	for (auto& floor : m_floors)
	{
		floor->Final();
	}
	m_floors.clear();
	
	for (auto& mountain : m_mountains)
	{
		mountain->Final();
	}
	m_mountains.clear();
}
