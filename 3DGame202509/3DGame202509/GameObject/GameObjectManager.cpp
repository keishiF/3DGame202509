#include "GameObjectManager.h"
#include "Physics.h"
#include "Player/Player.h"
#include "Enemy/EnemyMinion.h"
#include "Enemy/EnemyMage.h"
#include "Camera.h"
#include "TransformDataLoader.h"

GameObjectManager::GameObjectManager()
{
}

GameObjectManager::~GameObjectManager()
{
}

void GameObjectManager::Init()
{
	m_physics = std::make_shared<Physics>();

	TransformDataLoader loader;
	auto transformDataList = loader.LoadDataCSV("Data/CSV/CharacterTransformData.csv");

	for (const auto& data : transformDataList)
	{
		if (data.name == "Player")
		{
			m_player   = std::make_shared<Player>();
			Vec3 pos   = { data.pos.x, data.pos.y , data.pos.z };
			Vec3 rot   = { data.rot.x, data.rot.y , data.rot.z };
			Vec3 scale = { data.scale.x, data.scale.y , data.scale.z };
			m_player->Init(m_physics, pos, rot, scale);
		}
		else if (data.name == "Minion")
		{
			auto minion = std::make_shared<EnemyMinion>();
			Vec3 pos = { data.pos.x, data.pos.y , data.pos.z };
			Vec3 rot = { data.rot.x, data.rot.y , data.rot.z };
			Vec3 scale = { data.scale.x, data.scale.y , data.scale.z };
			minion->Init(m_physics, pos, rot, scale);
			m_minions.emplace_back(minion);
		}
		else if (data.name == "Mage")
		{
			auto mage = std::make_shared<EnemyMage>();
			Vec3 pos = { data.pos.x, data.pos.y , data.pos.z };
			Vec3 rot = { data.rot.x, data.rot.y , data.rot.z };
			Vec3 scale = { data.scale.x, data.scale.y , data.scale.z };
			mage->Init(m_physics, pos, rot, scale);
			m_mages.emplace_back(mage);
		}
	}

	if (m_player)
	{
		m_camera = std::make_shared<Camera>();
		m_camera->SetCamera(m_player);
	}

	m_camera = std::make_shared<Camera>();
	m_camera->SetCamera(m_player);
}

void GameObjectManager::Update()
{
	if (!m_player) return;

	m_physics->Update();
	m_player->Update();

	for (auto& minion : m_minions)
	{
		minion->Update(m_player);
	}

	for (auto& mage : m_mages)
	{
		mage->Update(m_player);
	}

	if (m_camera) m_camera->Update(m_player);
}

void GameObjectManager::Draw()
{
#ifdef _DEBUG
	m_physics->DebugDraw();
#endif

	if (m_player) m_player->Draw();

	for (auto& minion : m_minions)
	{
		minion->Draw();
	}

	for (auto& mage : m_mages)
	{
		mage->Draw();
	}
}
