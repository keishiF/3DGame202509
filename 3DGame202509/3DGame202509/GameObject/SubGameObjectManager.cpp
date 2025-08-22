#include "SubGameObjectManager.h"
#include "Physics.h"
#include "SubPlayer/SubPlayer.h"
#include "SubEnemy/SubEnemyBase.h"
#include "SubEnemy/SubEnemyMage.h"
#include "SubEnemy/SubEnemyMinion.h"
#include "SubEnemy/SubEnemyBoss.h"
#include "Camera/Camera.h"
#include "SkyDome/SkyDome.h"
#include "TransformDataLoader.h"

SubGameObjectManager::SubGameObjectManager() :
	m_isClear(false),
	m_isGameOver(false)
{
}

SubGameObjectManager::~SubGameObjectManager()
{
}

void SubGameObjectManager::Init()
{
	TransformDataLoader loader;
	auto transformDataList = loader.LoadDataCSV("Data/CSV/CharacterTransformData.csv");

	for (const auto& data : transformDataList)
	{
		// オブジェクト名がKnightだったら
		if (data.name == "Knight")
		{
			// Playerに配置データを渡して初期化
			m_player = std::make_shared<SubPlayer>();
			Vector3 pos = { data.pos.x, data.pos.y, data.pos.z };
			Vector3 rot = { data.rot.x, data.rot.y, data.rot.z };
			Vector3 scale = { data.scale.x, data.scale.y , data.scale.z };
			m_player->Init(pos, rot, scale);
		}
		// オブジェクト名がSkeleton_Minionだったら
		else if (data.name == "Skeleton_Minion")
		{
			// EnemyMinionに配置データを渡して初期化
			auto minion = std::make_shared<SubEnemyMinion>();
			Vector3 pos = { data.pos.x, data.pos.y, data.pos.z };
			Vector3 rot = { data.rot.x, data.rot.y, data.rot.z };
			Vector3 scale = { data.scale.x, data.scale.y , data.scale.z };
			minion->Init(pos, rot, scale);
			m_minions.emplace_back(minion);
		}
		// オブジェクト名がSkeleton_Mageだったら
		else if (data.name == "Skeleton_Mage")
		{
			// EnemyMageに配置データを渡して初期化
			auto mage = std::make_shared<SubEnemyMage>();
			Vector3 pos = { data.pos.x, data.pos.y, data.pos.z };
			Vector3 rot = { data.rot.x, data.rot.y, data.rot.z };
			Vector3 scale = { data.scale.x, data.scale.y , data.scale.z };
			mage->Init(pos, rot, scale);
			m_mages.emplace_back(mage);
		}
		// オブジェクト名がSkeleton_Warriorだったら
		else if (data.name == "Skeleton_Warrior")
		{
			// EnemyBossに配置データを渡して初期化
			m_boss = std::make_shared<SubEnemyBoss>();
			Vector3 pos = { data.pos.x, data.pos.y, data.pos.z };
			Vector3 rot = { data.rot.x, data.rot.y, data.rot.z };
			Vector3 scale = { data.scale.x, data.scale.y , data.scale.z };
			m_boss->Init(pos, rot, scale);
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

void SubGameObjectManager::Update()
{
	if (!m_player) return;

	Physics::Instance().Update();
	m_player->Update();

	for (auto& minion : m_minions)
	{
		if (minion.get())
		{
			minion->Update(m_player);
		}
	}

	for (auto& mage : m_mages)
	{
		if (mage.get())
		{
			mage->Update(m_player);
		}
	}

	if (m_boss.get())
	{
		m_boss->Update(m_player);
	}

	if (m_camera) m_camera->Update(m_player);
	if (m_skyDome) m_skyDome->Update(m_camera);

	m_isGameOver = m_player->IsDead();
	m_isClear = m_boss->IsDead();
}

void SubGameObjectManager::Draw()
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

	m_boss->Draw();
}

void SubGameObjectManager::Finalize()
{
	if (m_player) m_player->Final();
	if (m_boss) m_boss->Final();
	for (auto& minion : m_minions) minion->Final();
	for (auto& mage : m_mages)   mage->Final();
	m_minions.clear();
	m_mages.clear();
	m_player = nullptr;
	m_camera = nullptr;
	m_skyDome = nullptr;
}

std::vector<std::shared_ptr<SubEnemyBase>> SubGameObjectManager::GetEnemies()
{
	std::vector<std::shared_ptr<SubEnemyBase>> result;
	for (const auto& minion : m_minions)
	{
		if (minion) result.push_back(minion);
	}
	for (const auto& mage : m_mages)
	{
		if (mage) result.push_back(mage);
	}
	if (m_boss)
	{
		result.push_back(m_boss);
	}
	return result;
}

SubGameObjectManager& SubGameObjectManager::Instance()
{
	// TODO: return ステートメントをここに挿入します
	static SubGameObjectManager instance;
	return instance;
}
