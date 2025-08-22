#pragma once
#include <memory>
#include <vector>

class SubPlayer;
class SubEnemyBase;
class SubEnemyMinion;
class SubEnemyMage;
class SubEnemyBoss;
class Camera;
class SkyDome;
class SubGameObjectManager
{
public:
	static SubGameObjectManager& Instance();
	SubGameObjectManager(const SubGameObjectManager&) = delete;
	SubGameObjectManager& operator =(const SubGameObjectManager&) = delete;

	SubGameObjectManager();
	~SubGameObjectManager();

	void Init();
	void Update();
	void Draw();
	void Finalize();

	std::vector<std::shared_ptr<SubEnemyBase>> GetEnemies();

	bool IsClear() { return m_isClear; }
	bool IsGameOver() { return m_isGameOver; }

	std::shared_ptr<SubPlayer> GetPlayer() const { return m_player; }
	std::shared_ptr<SubEnemyBoss> GetEnemyBoss() const { return m_boss; }
	std::vector<std::shared_ptr<SubEnemyMinion>> GetEnemyMinions() const { return m_minions; }
	std::vector<std::shared_ptr<SubEnemyMage>> GetEnemyMages() const { return m_mages; }

private:
	std::shared_ptr<SubPlayer> m_player;
	std::vector<std::shared_ptr<SubEnemyMinion>>  m_minions;
	std::vector<std::shared_ptr<SubEnemyMage>>  m_mages;
	std::shared_ptr<SubEnemyBoss> m_boss;
	std::shared_ptr<Camera> m_camera;
	std::shared_ptr<SkyDome> m_skyDome;

	bool m_isClear;
	bool m_isGameOver;
};

