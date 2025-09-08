#pragma once

class ScoreManager
{
public:
    static ScoreManager & Instance();
    // スコア計算の開始
    void Start();
    // スコア計算の停止
    void Stop();
    // 敵を倒した際のスコアを加算
    void AddEnemyScore(int points);
    // スコアをリセット
    void Reset();
    
    // 各スコアを取得
    int GetTimeScore() const;
    int GetEnemyScore() const { return m_enemyScore; }
    int GetTotalScore() const;
    char GetRank() const;
    // クリア時間をミリ秒で取得
    int GetClearTime() const { return m_clearTime; }

    int GetCurrentElapsedTime() const;

private:
    ScoreManager();
    ScoreManager(const ScoreManager&) = delete;
    ScoreManager & operator=(const ScoreManager&) = delete;
    int m_startTime;  // 計測開始時間(ms)
    int m_clearTime;  // クリアにかかった時間(ms)
    int m_enemyScore; // 敵を倒したスコア
    bool m_isTiming;  // 計測中かどうかのフラグ
};

