#pragma once
#include "SceneBase.h"
#include <memory>

class Player;
class ResultScene final : public SceneBase
{
public:
	explicit ResultScene(SceneController& controller, bool isClear);
    ~ResultScene();

    virtual void Update() override;
    virtual void Draw() override;

private:
	int m_skyModel;
	int m_bgmHandle;
	int m_backGroundHandle;
	int m_fontHandle;

	float m_fadeFrame;
	int m_blinkFrame;

	using UpdateFunc_t = void(ResultScene::*)();
	using DrawFunc_t = void(ResultScene::*)();

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

private:
	// 表示アニメーションの状態
	enum class DisplayState
	{
		SHOW_TIME,
		SHOW_ENEMY_SCORE,
		SHOW_TOTAL,
		SHOW_RANK,
		DONE
	};
	// アニメーション関連の変数
	DisplayState m_displayState;
	int m_animationCounter; // 状態遷移の遅延タイマー

	// スコア関連の変数
	bool m_isClear;
	int m_finalTimeScore;
	int m_finalEnemyScore;
	int m_finalTotalScore;
	char m_rank;

	// カウントアップアニメーション用の現在表示スコア
	float m_displayTimeScore;
	float m_displayEnemyScore;
	float m_displayTotalScore;

	float m_timeScoreIncrement;
	float m_enemyScoreIncrement;
};

