#pragma once

#include "Input.h"
#include "Vec3.h"
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

	// プレイヤーのステート
private:
	// 待機状態
	void IdleInit(Input& input);
	void IdleUpdate(Input& input);

	// 移動状態
	void RunInit(Input& input);
	void RunUpdate(Input& input);

	// 各攻撃状態
	void ChopAttackInit(Input& input);
	void ChopAttackUpdate(Input& input);
	void SliceAttackInit(Input& input);
	void SliceAttackUpdate(Input& input);
	void StabAttackInit(Input& input);
	void StabAttackUpdate(Input& input);

	// アニメーション関連
private:
	struct AnimData
	{
		int attachNo = -1;
		float frame = 0.0f;
		bool isLoop = false;
		bool isEnd = false;
	};
	// アニメーションのアタッチ
	void AttachAnim(AnimData& data, const char* animName, bool isLoop);
	// アニメーションの更新
	void UpdateAnim(AnimData& data);
	// アニメーションブレンド率の更新
	void UpdateAnimBlend();
	// アニメーションの変更
	void ChangeAnim(const char* animName, bool isLoop);

private:
	// プレイヤーのモデル
	int m_model;
	// アニメーション
	AnimData m_nextAnim;
	AnimData m_prevAnim;
	// アニメーションを滑らかに変更するためのブレンド率
	float m_blendRate;
	// プレイヤーの位置
	Vec3 m_pos;
	// プレイヤーの移動量
	Vec3 m_vec;
	// プレイヤーのHP
	int m_hp;
	float m_jumpSpeed;
	float m_gravity;
	// プレイヤーの状態フラグ
	bool m_isChop;
	bool m_isSlice;
	bool m_isStab;
	bool m_isRun;

	// 経過フレームを測る
	float m_frameCount;

	// プレイヤーの状態を管理するための関数ポインタ
	using PlayerState = void(Player::*)(Input&);
	PlayerState m_state;
};

