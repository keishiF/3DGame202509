#pragma once

#include "Input.h"
#include "Vec3.h"
#include "Animation.h"
#include <memory>

class Player
{
public:
	// コンストラクタとデストラクタ
	Player();
	virtual ~Player();

	// 更新、描画
	void Update(Input& input);
	void Draw();

	// プレイヤーの位置を取得
	Vec3 GetPos() const { return m_pos; }

	// ダメージを受けた時の処理をまとめる関数
	void OnDamage();

	bool IsDead() const { return m_isDead; }

	// プレイヤーの状態
private:
	enum class PlayerState
	{
		Idle,
		Walk,
		Run,
		Chop,
		Slice,
		Stab,
		Spin,
		Dodge, 
		Hit,
		Dead
	};

	PlayerState m_state;
	void ChangeState(PlayerState newState);

	// 待機状態
	void IdleUpdate(Input& input);

	// 移動
	// 歩き
	void WalkUpdate(Input& input);
	// 走り
	void RunUpdate(Input& input);

	// 各攻撃状態
	void ChopUpdate(Input& input);
	void SliceUpdate(Input& input);
	void StabUpdate(Input& input);
	void SpinUpdate(Input& input);

	// 回避状態
	void DodgeUpdate(Input& input);

	// 被弾状態
	void HitUpdate(Input& input);

	// 死亡状態
	void DeadUpdate(Input& input);

private:
	// プレイヤーのモデル
	int m_model;
	// プレイヤーの位置
	Vec3 m_pos;
	// プレイヤーの当たり判定
	float m_radius;
	// プレイヤーの移動量
	Vec3 m_vec;
	// プレイヤーのHP
	int m_hp;
	// プレイヤーのフラグ
	bool m_isCombo;
	bool m_isDead;

	// 経過フレームを測る
	float m_frameCount;

	// プレイヤーのアニメーション
	Animation m_anim;
};

