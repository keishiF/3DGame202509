#include "GameObjectManager.h"
#include "Physics.h"
#include "Player/Player.h"
#include "Enemy/EnemyBase.h"
#include "Enemy/EnemyMinion.h"
#include "Enemy/EnemyMage.h"
#include "Enemy/EnemyBoss.h"
#include "Camera.h"
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
	if (m_player) m_player->Final();
	for (auto& minion : m_minions) minion->Final();
	for (auto& mage : m_mages)   mage->Final();
	for (auto& boss : m_boss)    boss->Final();
	m_minions.clear();
	m_mages.clear();
	m_boss.clear();
	m_player = nullptr;
	m_camera = nullptr;
	m_skyDome = nullptr;
	m_isClear = false;
	m_isGameOver = false;

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
			m_player->Init(pos, rot, scale);
		}
		else if (data.name == "Skeleton_Minion")
		{
			auto minion = std::make_shared<EnemyMinion>();
			Vec3 pos = { data.pos.x, data.pos.y, data.pos.z };
			Vec3 rot = { data.rot.x, data.rot.y, data.rot.z };
			Vec3 scale = { data.scale.x, data.scale.y , data.scale.z };
			minion->Init(pos, rot, scale);
			m_minions.emplace_back(minion);
		}
		else if (data.name == "Skeleton_Mage")
		{
			auto mage = std::make_shared<EnemyMage>();
			Vec3 pos = { data.pos.x, data.pos.y, data.pos.z };
			Vec3 rot = { data.rot.x, data.rot.y, data.rot.z };
			Vec3 scale = { data.scale.x, data.scale.y , data.scale.z };
			mage->Init(pos, rot, scale);
			m_mages.emplace_back(mage);
		}
		else if (data.name == "Skeleton_Warrior")
		{
			auto boss = std::make_shared<EnemyBoss>();
			Vec3 pos = { data.pos.x, data.pos.y, data.pos.z };
			Vec3 rot = { data.rot.x, data.rot.y, data.rot.z };
			Vec3 scale = { data.scale.x, data.scale.y , data.scale.z };
			boss->Init(pos, rot, scale);
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

	Physics::Instance().Update();
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
	Physics::Instance().DebugDraw();
#endif

	m_skyDome->Draw();

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
	static GameObjectManager instance;
	return instance;
}
