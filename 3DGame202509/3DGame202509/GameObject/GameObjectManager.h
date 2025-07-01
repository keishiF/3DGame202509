#pragma once

#include <memory>

class Physics;
class Player;
class EnemyMinion;
class EnemyMage;
class Camera;
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
	std::shared_ptr<EnemyMinion>  m_minion;
	std::shared_ptr<EnemyMage>  m_mage;
	std::shared_ptr<Camera> m_camera;
};

