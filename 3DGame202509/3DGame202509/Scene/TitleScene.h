#pragma once
#include "SceneBase.h"
#include "Vec3.h"

class TitleScene final:
	public SceneBase
{
public:
	TitleScene(SceneController& controller);
	~TitleScene();

	virtual void Update(Input& input) override;
	virtual void Draw() override;

private:
	int m_fadeFrame;
	int m_blinkFrame;

	using UpdateFunc_t = void(TitleScene::*)(Input&);
	using DrawFunc_t = void(TitleScene::*)();

	UpdateFunc_t m_update;
	DrawFunc_t m_draw;

	// 通常時更新処理
	void NormalUpdate(Input& input);
	// フェードイン
	void FadeInUpdate(Input&);
	// フェードアウト
	void FadeOutUpdate(Input&);

	// 通常時描画
	void NormalDraw();
	// フェード時の描画
	void FadeDraw();
};

