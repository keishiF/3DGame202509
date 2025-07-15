#include "GameObjectManager.h"
#include "Physics.h"
#include "Player/Player.h"
#include "Enemy/EnemyBase.h"
#include "Enemy/EnemyMinion.h"
#include "Enemy/EnemyMage.h"
#include "Enemy/EnemyBoss.h"
#include "Camera.h"
#include "Stage/Stage.h"
#include "SkyDome.h"
#include "TransformDataLoader.h"

GameObjectManager::GameObjectManager() :
	m_isClear(false),
	m_isGameOver(false)
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
		if (data.name == "Knight")
		{
			m_player   = std::make_shared<Player>();
			Vec3 pos   = { data.pos.x, data.pos.y, data.pos.z };
			Vec3 rot   = { data.rot.x, data.rot.y, data.rot.z };
			Vec3 scale = { data.scale.x, data.scale.y , data.scale.z };
			m_player->Init(m_physics, pos, rot, scale);
		}
		else if (data.name == "Skeleton_Minion")
		{
			auto minion = std::make_shared<EnemyMinion>();
			Vec3 pos = { data.pos.x, data.pos.y, data.pos.z };
			Vec3 rot = { data.rot.x, data.rot.y, data.rot.z };
			Vec3 scale = { data.scale.x, data.scale.y , data.scale.z };
			minion->Init(m_physics, pos, rot, scale);
			m_minions.emplace_back(minion);
		}
		else if (data.name == "Skeleton_Mage")
		{
			auto mage = std::make_shared<EnemyMage>();
			Vec3 pos = { data.pos.x, data.pos.y, data.pos.z };
			Vec3 rot = { data.rot.x, data.rot.y, data.rot.z };
			Vec3 scale = { data.scale.x, data.scale.y , data.scale.z };
			mage->Init(m_physics, pos, rot, scale);
			m_mages.emplace_back(mage);
		}
		else if (data.name == "Skeleton_Warrior")
		{
			auto boss = std::make_shared<EnemyBoss>();
			Vec3 pos = { data.pos.x, data.pos.y, data.pos.z };
			Vec3 rot = { data.rot.x, data.rot.y, data.rot.z };
			Vec3 scale = { data.scale.x, data.scale.y , data.scale.z };
			boss->Init(m_physics, pos, rot, scale);
			m_boss.emplace_back(boss);
		}
	}

	if (m_player)
	{
		m_camera = std::make_shared<Camera>();
		m_camera->SetCamera(m_player);
	}

	m_camera = std::make_shared<Camera>();
	m_camera->SetCamera(m_player);
	m_skyDome = std::make_shared<SkyDome>();
	m_skyDome->Init(m_camera);
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

	for (auto& boss: m_boss)
	{
		boss->Update(m_player);
	}

	if (m_camera) m_camera->Update(m_player);
	if (m_skyDome) m_skyDome->Update(m_camera);

	m_isGameOver = m_player->IsDead();
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

	for (auto& boss : m_boss)
	{
		boss->Draw();
	}

	m_skyDome->Draw();
}

std::vector<std::shared_ptr<EnemyBase>> GameObjectManager::GetEnemies()
{
	std::vector<std::shared_ptr<EnemyBase>> result;
	for (const auto& minion : m_minions) 
	{
		if (minion) result.push_back(minion);
	}
	for (const auto& mage : m_mages) 
	{
		if (mage) result.push_back(mage);
	}
	for (const auto& boss : m_boss) 
	{
		if (boss) result.push_back(boss);
	}
	return result;
}

GameObjectManager& GameObjectManager::Instance()
{
	// TODO: return ステートメントをここに挿入します
	static GameObjectManager gameObjectManager;
	return gameObjectManager;
}
