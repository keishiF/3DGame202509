#pragma once
#include "SceneBase.h"

class ClearScene final : public SceneBase
{
public:
	explicit ClearScene(SceneController& controller);
	~ClearScene();

	virtual void Update() override;
	virtual void Draw() override;

private:
	int m_skyModel;

	float m_fadeFrame;
	int m_blinkFrame;

	using UpdateFunc_t = void(ClearScene::*)();
	using DrawFunc_t = void(ClearScene::*)();

	UpdateFunc_t m_update;
	DrawFunc_t m_draw;

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

