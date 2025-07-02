#pragma once
#include <memory>
#include <vector>

class Physics;
class Player;
class EnemyMinion;
class EnemyMage;
class Camera;
class Stage;
class GameObjectManager
{
public:
	GameObjectManager();
	~GameObjectManager();

	void Init();
	void Update();
	void Draw();

private:
	std::shared_ptr<Physics> m_physics;
	std::shared_ptr<Player> m_player;
	std::vector<std::shared_ptr<EnemyMinion>>  m_minions;
	std::vector<std::shared_ptr<EnemyMage>>  m_mages;
	std::shared_ptr<Camera> m_camera;
	std::shared_ptr<Stage> m_stage;
};

