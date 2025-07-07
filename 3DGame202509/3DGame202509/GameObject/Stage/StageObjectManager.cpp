#include "StageObjectManager.h"
#include "Mountain.h"
#include "Meadow.h"
#include "TransformDataLoader.h"

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
			Vec3 pos = { data.pos.x, data.pos.y , data.pos.z };
			Vec3 rot = { data.rot.x, data.rot.y , data.rot.z };
			Vec3 scale = { data.scale.x, data.scale.y , data.scale.z };
			mountain->Init(modelPath, pos, rot, scale);
			m_mountains.emplace_back(mountain);
		}
		else if (data.name == "Meadow")
		{
			modelPath = "Data/Model/Stage/Meadow/Meadow.mv1";
			auto meadow = std::make_shared<Meadow>();
			Vec3 pos = { data.pos.x, data.pos.y , data.pos.z };
			Vec3 rot = { data.rot.x, data.rot.y , data.rot.z };
			Vec3 scale = { data.scale.x, data.scale.y , data.scale.z };
			meadow->Init(modelPath, pos, rot, scale);
			m_meadows.emplace_back(meadow);
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

	for (auto& meadow : m_meadows)
	{
		meadow->Draw();
	}
}