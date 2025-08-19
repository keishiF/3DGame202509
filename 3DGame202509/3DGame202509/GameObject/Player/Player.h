#pragma once
#include "Animator.h"
#include "Collidable.h"
#include "Input.h"
#include "Quaternion.h"
#include "Vector3.h"
#include <queue>

enum class PlayerState
{
	Idle,       // 待機
	Walk,       // 歩き
	Run,        // 走り
	Tired,		// 疲れ
	Chop,       // 攻撃1段目
	Slice,      // 攻撃2段目
	Stab,       // 攻撃3段目
	Spin,       // 強攻撃
	Special,    // 必殺技
	Dodge,      // 回避
	Hit,        // 被弾
	Dead        // 死亡
};

class Effect;
class EnemyBase;
class PlayerRightWeapon;
class PlayerLeftWeapon;
class Player : public Collidable
{
public:
	// コンストラクタとデストラクタ
	Player();
	virtual ~Player();

	// 初期化
	void Init(Vector3& pos, const Vector3& rot, const Vector3& scale);
	// 更新
	void Update();
	// 描画
	void Draw();

	// プレイヤーの位置を取得
	Vector3 GetPos() const { return m_rigidbody.GetPos(); }
	float GetRadius() const { return m_radius; }
	int GetModel() const { return m_charModel; }

	void SetSpecialGauge(int specialGaugePoint);

	// ダメージを受けた時の処理をまとめる関数
	void OnDamage();

	// 死んでいるかどうか
	bool IsDead() const { return m_isDead; }

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;

	// プレイヤーの状態
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
	// 疲れ
	void TiredUpdate();
	// 各攻撃状態
	void ChopUpdate();
	void SliceUpdate();
	void StabUpdate();
	void SpinUpdate();
	void SpecialUpdate();
	// 回避状態
	void DodgeUpdate();
	// 被弾状態
	void HitUpdate();
	// 死亡状態
	void DeadUpdate();

private:
	// HPの描画
	void DrawHPGauge();
	// スタミナの描画
	void DrawStaminaGauge();
	// 必殺技ゲージの描画
	void DrawSpecialGauge();

	void RotateToNearestEnemy(float radius);
	std::shared_ptr<EnemyBase> FindNearestEnemy(float radius);

private:
	// 正面ベクトル
	Vector3 m_forward;

	// プレイヤーのモデル
	int m_charModel;
	// プレイヤーの当たり判定
	float m_radius;
	// プレイヤーのHP
	int m_hp;
	// プレイヤーのスタミナ
	float m_stamina;
	// 必殺技ゲージ
	int m_specialGauge;
	// プレイヤーのフラグ
	bool m_isCombo;
	bool m_isDead;

	int m_attackPower;

	// 経過フレームを測る
	float m_frame;
	float m_attackFrame;

	// エフェクトハンドル
	int m_specialEffect;

	// プレイヤーのアニメーション
	Animator m_anim;

	Quaternion m_currentRot;

	std::shared_ptr<PlayerRightWeapon> m_rightWeapon;
	//std::shared_ptr<PlayerLeftWeapon> m_leftWeapon;

	std::weak_ptr<Effect> m_effect;
};

