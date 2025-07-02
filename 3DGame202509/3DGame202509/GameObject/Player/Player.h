#pragma once
#include "Animator.h"
#include "Collidable.h"
#include "Input.h"
#include "Vec3.h"
#include <memory>
#include <queue>
#include <string>

enum class PlayerState
{
	Idle,       // 待機
	Walk,       // 歩き
	Run,        // 走り
	Chop,       // 攻撃1段目
	Slice,      // 攻撃2段目
	Stab,       // 攻撃3段目
	Spin,       // 強攻撃
	Special,    // 必殺技
	DualChop,   // 必殺技
	DualSlice,  // 必殺技
	DualStab,   // 必殺技
	Dodge,      // 回避
	Hit,        // 被弾
	Dead        // 死亡
};

class PlayerRightWeapon;
class PlayerLeftWeapon;
class Player : public Collidable
{
public:
	// コンストラクタとデストラクタ
	Player();
	virtual ~Player();

	// 初期化、更新、描画
	void Init(std::shared_ptr<Physics> physics);
	void Init(std::shared_ptr<Physics> physics, Vec3& pos, const Vec3& rot, const Vec3& scale);
	void Update();
	void Draw();

	// プレイヤーの位置を取得
	Vec3 GetPos() const { return m_rigidbody.GetPos(); }
	float GetRadius() const { return m_radius; }
	int GetModel() const { return m_charModel; }

	// ダメージを受けた時の処理をまとめる関数
	void OnDamage();

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

	std::queue<const char*> m_ultimateAnimQueue;
	std::string m_currentSpecialAnim;

private:
	// プレイヤーのモデル
	int m_charModel;
	// プレイヤーの当たり判定
	float m_radius;
	// プレイヤーのHP
	int m_hp;
	// プレイヤーのフラグ
	bool m_isCombo;
	bool m_isDead;

	// 経過フレームを測る
	float m_attackFrame;

	// プレイヤーのアニメーション
	Animator m_anim;

	std::shared_ptr<PlayerRightWeapon> m_rightWeapon;
	std::shared_ptr<PlayerLeftWeapon> m_leftWeapon;
};

