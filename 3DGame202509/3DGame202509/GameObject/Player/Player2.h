//#pragma once
//#include "Animator.h"
//#include "Collidable.h"
//#include "Input.h"
//#include "Quaternion.h"
//#include "Vector3.h"
//#include <queue>
//
//enum class PlayerState
//{
//	Idle,       // 待機
//	Walk,       // 歩き
//	NormalRun,  // 走り
//	TiredRun,   // 遅い走り
//	Tired,		// 疲れ
//	Chop,       // 攻撃1段目
//	Slice,      // 攻撃2段目
//	Stab,       // 攻撃3段目
//	Spin,       // 強攻撃
//	Shot,       // 射撃
//	Special,    // 必殺技
//	Dodge,      // 回避
//	Hit,        // 被弾
//	Dead        // 死亡
//};
//
//class Effect;
//class EnemyBase;
//class PlayerBullet;
//class PlayerLeftWeapon;
//class PlayerRightWeapon;
//class Player : public Collidable
//{
//public:
//	// コンストラクタとデストラクタ
//	Player();
//	virtual ~Player();
//
//	// 初期化
//	void Init(Vector3& pos, const Vector3& rot, const Vector3& scale);
//	// 更新
//	void Update();
//	// 描画
//	void Draw();
//
//	// ゲッター
//	// 位置取得
//	Vector3 GetPos() const { return m_rigidbody.GetPos(); }
//	// 半径取得
//	float GetRadius() const { return m_radius; }
//	// モデル取得
//	int GetModel() const { return m_charModel; }
//	// HP取得
//	float GetHP() const { return m_hp; }
//	float GetHPRate() const { return m_hpRate; }
//	// スタミナ取得
//	float GetStamina() const { return m_stamina; }
//	float GetStaminaRate() const { return m_staminaRate; }
//	// 必殺技ゲージ取得
//	float GetSpecialGauge() const { return m_specialGauge; }
//	float GetSpecialGaugeRate() const { return m_specialGaugeRate; }
//	// 現在の状態を取得
//	PlayerState GetPlayerState() const { return m_state; }
//
//	// 必殺技ゲージを加算する関数
//	void SetSpecialGauge(int specialGaugePoint);
//
//	// ダメージを受けた時の処理をまとめる関数
//	void OnDamage();
//
//	// 死んでいるかどうか
//	bool IsDead() const { return m_isDead; }
//
//	// 当たった時の処理
//	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;
//
//	// プレイヤーの状態
//private:
//	PlayerState m_state;
//	void ChangeState(PlayerState newState);
//
//	// 待機状態
//	void IdleUpdate();
//	// 移動
//	// 歩き
//	void WalkUpdate();
//	// 走り
//	void RunUpdate();
//	// 遅い走り
//	void TiredRunUpdate();
//	// 疲れ
//	void TiredUpdate();
//	// 各攻撃状態
//	void ChopUpdate();
//	void SliceUpdate();
//	void StabUpdate();
//	void SpinUpdate();
//	void ShotUpdate();
//	void SpecialUpdate();
//	// 回避状態
//	void DodgeUpdate();
//	// 被弾状態
//	void HitUpdate();
//	// 死亡状態
//	void DeadUpdate();
//
//private:
//	void RotateToNearestEnemy(float radius);
//	std::shared_ptr<EnemyBase> FindNearestEnemy(float radius);
//
//private:
//	// 正面ベクトル
//	Vector3 m_forward;
//
//	// プレイヤーのモデル
//	int m_charModel;
//	// プレイヤーの当たり判定
//	float m_radius;
//	// プレイヤーのHP
//	float m_hp;
//	float m_hpRate;
//	// プレイヤーのスタミナ
//	float m_stamina;
//	float m_staminaRate;
//	// 必殺技ゲージ
//	float m_specialGauge;
//	float m_specialGaugeRate;
//	// プレイヤーのフラグ
//	bool m_isCombo;
//	bool m_isDead;
//
//	int m_attackPower;
//
//	// 経過フレームを測る
//	float m_frame;
//	int m_blinkFrame;
//	float m_attackFrame;
//
//	// エフェクトハンドル
//	int m_specialEffect;
//
//	// プレイヤーのアニメーション
//	Animator m_anim;
//
//	Quaternion m_currentRot;
//
//	std::vector<std::shared_ptr<PlayerBullet>> m_bullets;
//
//	std::shared_ptr<PlayerRightWeapon> m_rightWeapon;
//	//std::shared_ptr<PlayerLeftWeapon> m_leftWeapon;
//
//	std::weak_ptr<Effect> m_effect;
//};
//
