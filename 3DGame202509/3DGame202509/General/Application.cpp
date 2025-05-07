#include "Application.h"
#include "SceneController.h"
#include "Input.h"
#include "DxLib.h"
#include "game.h"
#include <cassert>

Application& Application::GetInstance()
{
	// TODO: return ステートメントをここに挿入します
	static Application app;
	return app;
}

bool Application::Init()
{
	// フルスクリーンではなく、ウインドウモードで開くようにする
	ChangeWindowMode(Game::kDefaultWindowMode);
	// ウィンドウ名設定
	SetMainWindowText(Game::kTitleText);
	// 画面のサイズを変更する
	SetGraphMode(Game::kScreenWidth, Game::kScreenHeight, Game::kColorBItNum);

	if (DxLib_Init() == -1)		// ＤＸライブラリ初期化処理
	{
		return false;			// エラーが起きたら直ちに終了
	}

	SetUseZBuffer3D(true);
	SetWriteZBuffer3D(true);
	SetUseBackCulling(true);

	// 描画先を裏画面にする
	SetDrawScreen(DX_SCREEN_BACK);

	return true;
}

void Application::Run()
{
	m_frame = 0;

	// メモリの確保
	SceneController sceneController;
	Input input;

	// ゲームループ
	while (ProcessMessage() == 0)	// Windowsが行う処理を待つ必要がある
	{
		// 今回のループが始まった時間を覚えておく
		LONGLONG time = GetNowHiPerformanceCount();

		// 画面全体をクリアする
		ClearDrawScreen();

		// ここにゲームの処理を書く
		// パッドの入力更新処理
		input.Update();

		sceneController.Update(input);
		sceneController.Draw();

		// 画面の切り替わりを待つ必要がある
		ScreenFlip();	// 1/60秒経過するまで待つ
		++m_frame;

		// FPS(Frame Per Second)60に固定
		while (GetNowHiPerformanceCount() - time < 16667)
		{
		}

		// もしＥＳＣキーが押されていたらループから抜ける
		if (CheckHitKey(KEY_INPUT_ESCAPE))
		{
			break;
		}
	}
}

void Application::Terminate()
{
	DxLib_End();				// ＤＸライブラリ使用の終了処理
}
