#include "GameObjectManager.h"
#include "Physics.h"
#include "Player/Player.h"
#include "Enemy/EnemyBase.h"
#include "Enemy/EnemyMinion.h"
#include "Enemy/EnemyMage.h"
#include "Enemy/EnemyBoss.h"
#include "Camera/Camera.h"
#include "SkyDome/SkyDome.h"
#include "TransformDataLoader.h"

#include "SubPlayer/SubPlayer.h"
#include "SubEnemy/SubEnemyBase.h"
#include "SubEnemy/SubEnemyBoss.h"
#include "SubEnemy/SubEnemyMage.h"
#include "SubEnemy/SubEnemyMinion.h"

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
	TransformDataLoader loader;
	auto transformDataList = loader.LoadDataCSV("Data/CSV/CharacterTransformData.csv");

	for (const auto& data : transformDataList)
	{
		// オブジェクト名がKnightだったら
		if (data.name == "Knight")
		{
			//// Playerに配置データを渡して初期化
			//m_player   = std::make_shared<Player>();
			//Vector3 pos   = { data.pos.x, data.pos.y, data.pos.z };
			//Vector3 rot   = { data.rot.x, data.rot.y, data.rot.z };
			//Vector3 scale = { data.scale.x, data.scale.y , data.scale.z };
			//m_player->Init(pos, rot, scale);

			// Playerに配置データを渡して初期化
			m_subPlayer = std::make_shared<SubPlayer>();
			Vector3 pos = { data.pos.x, data.pos.y, data.pos.z };
			Vector3 rot = { data.rot.x, data.rot.y, data.rot.z };
			Vector3 scale = { data.scale.x, data.scale.y , data.scale.z };
			m_subPlayer->Init(pos, rot, scale);
		}
		// オブジェクト名がSkeleton_Minionだったら
		else if (data.name == "Skeleton_Minion")
		{
			//// EnemyMinionに配置データを渡して初期化
			//auto minion = std::make_shared<EnemyMinion>();
			//Vector3 pos = { data.pos.x, data.pos.y, data.pos.z };
			//Vector3 rot = { data.rot.x, data.rot.y, data.rot.z };
			//Vector3 scale = { data.scale.x, data.scale.y , data.scale.z };
			//minion->Init(pos, rot, scale);
			//m_minions.emplace_back(minion);

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
			//// EnemyMageに配置データを渡して初期化
			//auto mage = std::make_shared<EnemyMage>();
			//Vector3 pos = { data.pos.x, data.pos.y, data.pos.z };
			//Vector3 rot = { data.rot.x, data.rot.y, data.rot.z };
			//Vector3 scale = { data.scale.x, data.scale.y , data.scale.z };
			//mage->Init(pos, rot, scale);
			//m_mages.emplace_back(mage);

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
			//// EnemyBossに配置データを渡して初期化
			//m_boss = std::make_shared<EnemyBoss>();
			//Vector3 pos = { data.pos.x, data.pos.y, data.pos.z };
			//Vector3 rot = { data.rot.x, data.rot.y, data.rot.z };
			//Vector3 scale = { data.scale.x, data.scale.y , data.scale.z };
			//m_boss->Init(pos, rot, scale);

			// EnemyBossに配置データを渡して初期化
			m_subBoss = std::make_shared<SubEnemyBoss>();
			Vector3 pos = { data.pos.x, data.pos.y, data.pos.z };
			Vector3 rot = { data.rot.x, data.rot.y, data.rot.z };
			Vector3 scale = { data.scale.x, data.scale.y , data.scale.z };
			m_subBoss->Init(pos, rot, scale);
		}
	}

	/*if (m_player)
	{
		m_camera = std::make_shared<Camera>();
		m_camera->SetCamera(m_player);
	}*/

	if (m_subPlayer)
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
	if (!m_subPlayer) return;

	Physics::Instance().Update();
	m_subPlayer->Update();

	for (auto& minion : m_subMinions)
	{
		if (minion.get())
		{
			minion->Update(m_subPlayer);
		}
	}

	for (auto& mage : m_subMages)
	{
		if (mage.get())
		{
			mage->Update(m_subPlayer);
		}
	}

	if (m_subBoss.get())
	{
		m_subBoss->Update(m_subPlayer);
	}

	if (m_camera) m_camera->Update(m_subPlayer);
	if (m_skyDome) m_skyDome->Update(m_camera);

	m_isGameOver = m_subPlayer->IsDead();
	m_isClear = m_subBoss->IsDead();
	/*if (!m_player) return;

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
	m_isClear = m_boss->IsDead();*/
}

void GameObjectManager::Draw()
{
#ifdef _DEBUG
	Physics::Instance().DebugDraw();
#endif

	m_skyDome->Draw();

	if (m_subPlayer) m_subPlayer->Draw();

	for (auto& minion : m_subMinions)
	{
		minion->Draw();
	}

	for (auto& mage : m_subMages)
	{
		mage->Draw();
	}

	m_subBoss->Draw();
//#ifdef _DEBUG
//	Physics::Instance().DebugDraw();
//#endif
//
//	m_skyDome->Draw();
//
//	if (m_player) m_player->Draw();
//
//	for (auto& minion : m_minions)
//	{
//		minion->Draw();
//	}
//
//	for (auto& mage : m_mages)
//	{
//		mage->Draw();
//	}
//
//	m_boss->Draw();
}

void GameObjectManager::Finalize()
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

std::vector<std::shared_ptr<SubEnemyBase>> GameObjectManager::GetSubEnemies()
{
	std::vector<std::shared_ptr<SubEnemyBase>> result;
	for (const auto& minion : m_subMinions)
	{
		if (minion) result.push_back(minion);
	}
	for (const auto& mage : m_subMages)
	{
		if (mage) result.push_back(mage);
	}
	if (m_subBoss)
	{
		result.push_back(m_subBoss);
	}
	return result;
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
    if (m_boss)
	{  
        result.push_back(m_boss);  
    }  
    return result;  
}

GameObjectManager& GameObjectManager::Instance()
{
	// TODO: return ステートメントをここに挿入します
	static GameObjectManager instance;
	return instance;
}
