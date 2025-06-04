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
	// 待機状態
	void IdleInit(Input& input);
	void IdleUpdate(Input& input);

	// 移動
	// 歩き
	void WalkInit(Input& input);
	void WalkUpdate(Input& input);
	// 走り
	void RunInit(Input& input);
	void RunUpdate(Input& input);

	// 各攻撃状態
	void ChopInit(Input& input);
	void ChopUpdate(Input& input);
	void SliceInit(Input& input);
	void SliceUpdate(Input& input);
	void StabInit(Input& input);
	void StabUpdate(Input& input);
	void SpinInit(Input& input);
	void SpinUpdate(Input& input);

	// 回避状態
	void DodgeInit(Input& input);
	void DodgeUpdate(Input& input);

	// 被弾状態
	void HitInit(Input& input);
	void HitUpdate(Input& input);

	// 死亡状態
	void DeadInit(Input& input);
	void DeadUpdate(Input& input);

private:
	// プレイヤーのモデル
	int m_model;
	// プレイヤーの位置
	Vec3 m_pos;
	// プレイヤーの移動量
	Vec3 m_vec;
	// プレイヤーのHP
	int m_hp;
	// プレイヤーの状態フラグ
	bool m_isWalk;
	bool m_isRun;
	bool m_isChop;
	bool m_isSlice;
	bool m_isStab;
	bool m_isSpin;
	bool m_isDodge;
	bool m_isDead;

	// 経過フレームを測る
	float m_frameCount;
	float m_walkFrameCount;

	// プレイヤーの状態を管理するための関数ポインタ
	using PlayerState = void(Player::*)(Input&);
	PlayerState m_state;

	// プレイヤーのアニメーション
	Animation m_anim;
};

