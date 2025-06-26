#pragma once

#include "Input.h"
#include "Vec3.h"
#include "Collidable.h"
#include "Animation.h"
#include <memory>

class PlayerWeapon;
class Player : public Collidable
{
public:
	// コンストラクタとデストラクタ
	Player();
	virtual ~Player();

	// 初期化、更新、描画
	void Init(std::shared_ptr<Physics> physics);
	void Update();
	void Draw();

	// プレイヤーの位置を取得
	Vec3 GetPos() const { return m_rigidbody.GetPos(); }
	float GetRadius() const { return m_radius; }

	// ダメージを受けた時の処理をまとめる関数
	void OnDamage();

	bool IsDead() const { return m_isDead; }

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;

	// プレイヤーの状態
private:
	enum class PlayerState
	{
		Idle,     // 待機
		Walk,     // 歩き
		Run,      // 走り
		Chop,     // 攻撃1段目
		Slice,    // 攻撃2段目
		Stab,     // 攻撃3段目
		Spin,     // 強攻撃
		Ultimate, // 必殺技
		Dodge,    // 回避
		Hit,      // 被弾
		Dead      // 死亡
	};

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
	void UltimateUpdate();

	// 回避状態
	void DodgeUpdate();

	// 被弾状態
	void HitUpdate();

	// 死亡状態
	void DeadUpdate();

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
	float m_frameCount;

	// プレイヤーのアニメーション
	Animation m_anim;

	std::shared_ptr<PlayerWeapon> m_weapon;
};

