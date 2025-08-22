#pragma once
#include "SceneBase.h"
#include "Vector3.h"
#include <memory>

class GameObjectManager;
class Physics;
class StageObjectManager;
class SubGameObjectManager;
class UIManager;
class GameScene final : public SceneBase
{
public:
	explicit GameScene(SceneController& controller);
	~GameScene();

	virtual void Update() override;
	virtual void Draw() override;

private:
	int m_frame;
	int m_fadeFrame;
	int m_blinkFrame;

	using UpdateFunc_t = void(GameScene::*)();
	using DrawFunc_t = void(GameScene::*)();
	UpdateFunc_t m_update;
	DrawFunc_t m_draw;

	std::shared_ptr<Physics> m_physics;
	std::shared_ptr<StageObjectManager> m_stageObjectManager;
	std::shared_ptr<UIManager> m_uiManager;

	GameObjectManager& m_gameObjectManager;
	SubGameObjectManager& m_subGameObjectManager;

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
};

