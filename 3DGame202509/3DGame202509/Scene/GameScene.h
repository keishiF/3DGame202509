#pragma once
#include "SceneBase.h"
#include "DxLib.h"
#include "Vec3.h"
#include <memory>

class Physics;
class Player;
class EnemyMinion;
class EnemyMage;
class Camera;
class GameObjectManager;
class GameScene final:
	public SceneBase
{
public:
	GameScene(SceneController& controller);
	~GameScene();

	virtual void Update() override;
	virtual void Draw() override;

private:
	int m_frame;
	int m_fadeFrame;
	int m_blinkFrame;

	int m_skyModel;

	using UpdateFunc_t = void(GameScene::*)();
	using DrawFunc_t = void(GameScene::*)();
	UpdateFunc_t m_update;
	DrawFunc_t m_draw;

	std::shared_ptr<Physics> m_physics;
	std::shared_ptr<Player> m_player;
	std::shared_ptr<EnemyMinion>  m_minion;
	std::shared_ptr<EnemyMage>  m_mage;
	std::shared_ptr<Camera> m_camera;
	std::shared_ptr<GameObjectManager> m_gameObjectManager;

	// 通常時更新処理
	void NormalUpdate();
	// フェードイン
	void FadeInUpdate();
	// フェードアウト
	void FadeOutUpdate();

	// 通常時描画
	void NormalDraw();
	// フェード時の描画
	void FadeDraw();

	// グリッドを描画
	void DrawField();
};

