#include "game.h"
#include "GameTimer.h"
#include "ScoreManager.h"
#include <DxLib.h>
#include <string>

GameTimer::GameTimer()
{
    // タイマー表示用のフォントを作成
    m_fontHandle = CreateFontToHandle("Impact", 32, 2, DX_FONTTYPE_ANTIALIASING_8X8);
}

GameTimer::~GameTimer()
{
    if (m_fontHandle != -1)
    {
        DeleteFontToHandle(m_fontHandle);
    }
}

void GameTimer::Draw()
{
    // ScoreManagerからミリ秒単位の経過時間を取得
    int elapsed_ms = ScoreManager::Instance().GetCurrentElapsedTime();

    // 分と秒に変換
    int totalSeconds = elapsed_ms / 1000;
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;

    // MM:SS 形式の文字列を作成
    char timeStr[16];
    sprintf_s(timeStr, sizeof(timeStr), "%02d:%02d", minutes, seconds);

    // 文字列の幅を計算して、画面中央上部に表示する座標を決定
    int textWidth = GetDrawStringWidthToHandle(timeStr, static_cast<int>(strlen(timeStr)), m_fontHandle);
    int x = (Game::kScreenWidth - textWidth) / 2;
    int y = 20; // 画面上部からのマージン

    // 時間を描画
    DrawStringToHandle(x, y, timeStr, 0xffffff, m_fontHandle);
}
