#pragma once
#include <memory>
#include <vector>

class SubPlayer;
class SubEnemyBase;
class SubEnemyBoss;
class SubEnemyMage;
class SubEnemyMinion;

class Player;
class EnemyBase;
class EnemyMinion;
class EnemyMage;
class EnemyBoss;
class Camera;
class SkyDome;
class GameObjectManager
{
public:
	static GameObjectManager& Instance();
	GameObjectManager(const GameObjectManager&) = delete;
	GameObjectManager& operator =(const GameObjectManager&) = delete;

	GameObjectManager();
	~GameObjectManager();

	void Init();
	void Update();
	void Draw();
	void Finalize();

	std::vector<std::shared_ptr<SubEnemyBase>> GetSubEnemies();

	std::vector<std::shared_ptr<EnemyBase>> GetEnemies();

	bool IsClear()	  { return m_isClear; }
	bool IsGameOver() { return m_isGameOver; }

	std::shared_ptr<SubPlayer> GetSubPlayer() const { return m_subPlayer; }
	std::shared_ptr<SubEnemyBoss> GetSubEnemyBoss() const { return m_subBoss; }
	std::vector<std::shared_ptr<SubEnemyMinion>> GetSubEnemyMinions() const { return m_subMinions; }
	std::vector<std::shared_ptr<SubEnemyMage>> GetSubEnemyMages() const { return m_subMages; }

	std::shared_ptr<Player> GetPlayer() const { return m_player; }
	std::shared_ptr<EnemyBoss> GetEnemyBoss() const { return m_boss; }
	std::vector<std::shared_ptr<EnemyMinion>> GetEnemyMinions() const {return m_minions; }
	std::vector<std::shared_ptr<EnemyMage>> GetEnemyMages() const { return m_mages; }

private:
	std::shared_ptr<SubPlayer> m_subPlayer;
	std::vector<std::shared_ptr<SubEnemyMinion>>  m_subMinions;
	std::vector<std::shared_ptr<SubEnemyMage>>  m_subMages;
	std::shared_ptr<SubEnemyBoss> m_subBoss;

	std::shared_ptr<Player> m_player;
	std::vector<std::shared_ptr<EnemyMinion>>  m_minions;
	std::vector<std::shared_ptr<EnemyMage>>  m_mages;
	std::shared_ptr<EnemyBoss> m_boss;
	std::shared_ptr<Camera> m_camera;
	std::shared_ptr<SkyDome> m_skyDome;

	bool m_isClear;
	bool m_isGameOver;
};

