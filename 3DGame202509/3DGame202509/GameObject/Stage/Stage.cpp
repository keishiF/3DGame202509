#include "Stage.h"
#include "TransformDataLoader.h"
#include <unordered_map>

void Stage::Init()
{
	TransformDataLoader loader;
	auto objectDataList = loader.LoadDataCSV("Data/CSV/StageTransformData.csv");

	for (const auto& data : objectDataList)
	{
		std::string modelPath;

		// オブジェクト名に応じてモデルファイルを決める
		if (data.name == "Path")
			modelPath = "Data/Model/Stage/Path/Path.mv1";
		else if (data.name == "Floor_Brick")
			modelPath = "Data/Model/Stage/Floor/Floor.mv1";
		else if (data.name == "Road")
			modelPath = "Data/Model/Stage/Road.mv1";
		else
			continue;

		StageObject obj;
		Vec3 pos   = { data.pos.x, data.pos.y, data.pos.z };
		Vec3 rot   = { data.rot.x, data.rot.y, data.rot.z };
		Vec3 scale = { data.scale.x, data.scale.y, data.scale.z };
		obj.Init(modelPath, pos, rot, scale);
		m_objects.emplace_back(obj);
	}
}

void Stage::Draw()
{
	for (auto& obj : m_objects)
	{
		obj.Draw();
	}
}
