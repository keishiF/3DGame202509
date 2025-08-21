#pragma once
#include "CharacterBase.h"

/// <summary>
/// プレイヤーの状態
/// </summary>
enum class PlayerState
{
	Idle,       // 待機
	Walk,       // 歩き
	NormalRun,  // 走り
	TiredRun,   // 遅い走り
	Tired,		// 疲れ
	Chop,       // 攻撃1段目
	Slice,      // 攻撃2段目
	Stab,       // 攻撃3段目
	Spin,       // 強攻撃
	Shot,       // 射撃
	Special,    // 必殺技
	Dodge,      // 回避
	Hit,        // 被弾
	Dead        // 死亡
};

class EnemyBase;
class PlayerBullet;
class PlayerRightWeapon;
class SubPlayer : public CharacterBase
{
public:
	SubPlayer();
	~SubPlayer();

	virtual void Init(Vector3& pos, Vector3& rot, Vector3& scale) override;
	virtual void Draw() override;
	virtual void OnDamage(float atk) override;

	// ゲッター
	// スタミナ取得
	float GetStamina() const { return m_stamina; }
	// スタミナ割合取得
	float GetStaminaRate() const { return m_staminaRate; }
	// 必殺技ゲージ取得
	float GetSpecialGauge() const { return m_specialGauge; }
	// 必殺技ゲージ割合取得
	float GetSpecialGaugeRate() const { return m_specialGaugeRate; }

	void Update();
private:
	PlayerState m_state;
	void ChangeState(PlayerState newState);

	// 待機状態
	void IdleUpdate();
	// 移動
	// 歩き
	void WalkUpdate();
	// 走り
	void RunUpdate();
	// 遅い走り
	void TiredRunUpdate();
	// 疲れ
	void TiredUpdate();
	// 各攻撃状態
	void ChopUpdate();
	void SliceUpdate();
	void StabUpdate();
	void SpinUpdate();
	void ShotUpdate();
	void SpecialUpdate();
	// 回避状態
	void DodgeUpdate();
	// 被弾状態
	void HitUpdate();
	// 死亡状態
	void DeadUpdate();

private:
	void RotateToNearestEnemy(float radius);
	std::shared_ptr<EnemyBase> FindNearestEnemy(float radius);

private:
	// スタミナ
	float m_stamina = 0.0f;
	// スタミナ割合
	float m_staminaRate = 0.0f;
	// 必殺技ゲージ
	float m_specialGauge = 0.0f;
	// 必殺技ゲージ割合
	float m_specialGaugeRate = 0.0f;
	// コンボフラグ
	bool m_isCombo = false;
	// 回転
	Quaternion m_currentRot = { 1.0f, 0.0f, 0.0, 0.0f };
	// 武器を持つ
	std::shared_ptr<PlayerRightWeapon> m_rightWeapon;
};

