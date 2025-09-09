#include "ScoreManager.h"
#include <DxLib.h>
#include <algorithm>

namespace
{
    // タイムスコアの基準値（0秒でクリアした場合のスコア）
    constexpr int kMaxTimeScore = 20000;
    // 1秒経過するごとに減点されるスコア
    constexpr int kTimeScoreSecond = 50;

    // ランク判定のしきい値
    constexpr int kRankS = 25000;
    constexpr int kRankA = 20000;
    constexpr int kRankB = 15000;
}

ScoreManager& ScoreManager::Instance()
{
    static ScoreManager instance;
    return instance;
}

int ScoreManager::GetCurrentElapsedTime() const
{
    // 計測中の場合は、現在時刻 - 開始時刻 を返す
    if (m_isTiming)
    {
        return GetNowCount() - m_startTime;
    }
    // 計測が終了している場合は、最終的なクリア時間を返す
    return m_clearTime;
}

ScoreManager::ScoreManager() :
    m_startTime(0),
    m_clearTime(0),
    m_enemyScore(0),
    m_isTiming(false)
{
}

void ScoreManager::Start()
{
    Reset();
    m_startTime = GetNowCount();
    m_isTiming = true;
}

void ScoreManager::Stop()
{
    if (m_isTiming)
    {
        m_clearTime = GetNowCount() - m_startTime;
        m_isTiming = false;
    }
}

void ScoreManager::AddEnemyScore(int points)
{
    m_enemyScore += points;
}

void ScoreManager::Reset()
{
    m_startTime = 0;
    m_clearTime = 0;
    m_enemyScore = 0;
    m_isTiming = false;
}

int ScoreManager::GetTimeScore() const
{
    // クリアにかかった時間を秒に変換
    int clearTimeInSeconds = m_clearTime / 1000;
    int timeScore = kMaxTimeScore - (clearTimeInSeconds * kTimeScoreSecond);

    if (timeScore <= 0)
    {
        timeScore = 0;
    }
    return timeScore;
}

int ScoreManager::GetTotalScore() const
{
    return GetTimeScore() + GetEnemyScore();
}

char ScoreManager::GetRank() const
{
    int totalScore = GetTotalScore();
    if (totalScore >= kRankS) return 'S';
    if (totalScore >= kRankA) return 'A';
    if (totalScore >= kRankB) return 'B';
    return 'C';
}