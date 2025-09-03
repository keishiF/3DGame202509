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
	Dead       // 死亡
};

class EnemyBase;
class PlayerBullet;
class PlayerWeapon;
class Player : public CharacterBase
{
public:
	Player();
	~Player();

	virtual void Init(Vector3& pos, Vector3& rot, Vector3& scale) override;
	virtual void Draw() override;
	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;
	virtual void OnDamage(float atk) override;

	// ゲッター
	// スタミナ取得
	float GetStamina() const { return m_stamina; }
	// 最大スタミナ取得
	float GetMaxStamina() const { return m_maxStamina; }
	// 必殺技ゲージ取得
	float GetSpecialGauge() const { return m_specialGauge; }
	// 最大必殺技ゲージ取得
	float GetMaxSpecialGauge() const { return m_maxSpecialGauge; }
	// 状態取得
	PlayerState GetPlayerState() const { return m_state; }
	Vector3 GetPos() const { return m_rigidbody.GetPos(); }
	// 武器を取得
	std::shared_ptr<PlayerWeapon> GetWeapon() const { return m_weapon; }

	// 必殺技ゲージを増やす
	void SetSpecialGauge(int specialGaugePoint);

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
	// スタミナの最大量
	float m_maxStamina = 0.0f;
	// 必殺技ゲージ
	float m_specialGauge = 0.0f;
	// 必殺技ゲージの最大値
	float m_maxSpecialGauge = 0.0f;
	// コンボフラグ
	bool m_isCombo = false;
	// 回転
	Quaternion m_currentRot = { 1.0f, 0.0f, 0.0, 0.0f };
	// 武器を持つ
	std::shared_ptr<PlayerWeapon> m_weapon;
	// 弾を持つ
	std::vector<std::shared_ptr<PlayerBullet>> m_bullets;

	// 各ハンドル
	// エフェクトハンドル
	int m_specialAtkEffect = -1;
	int m_playingEffect = -1;
	// SEハンドル
	int m_atkSE = -1;
	int m_shotSE = -1;
	int m_dodgeSE = -1;
	int m_specialAtkSE = -1;
};

