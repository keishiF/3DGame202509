#pragma once
#include "Animator.h"
#include "Collidable.h"
#include "ColliderData.h"
#include "ObjectPriority.h"
#include "ObjectTag.h"
#include "Quaternion.h"
#include "Vector3.h"
#include <algorithm>
#include <cassert>
#include <DxLib.h>
#include <memory>
#include <vector>

/// <summary>
/// キャラクターの基底クラス
/// </summary>
class CharacterBase : public Collidable
{
public:
	CharacterBase(ObjectTag tag, ObjectPriority priority, ColliderData::Kind colliderKind);
	~CharacterBase();

	virtual void Init(Vector3& pos, Vector3& rot, Vector3& scale) abstract;
	virtual void Draw() abstract;
	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;
	virtual void OnDamage(float atk) abstract;

	// ゲッター
	// モデル取得
	int GetModel() const { return m_model; }
	// HP取得
	float GetHP() const { return m_hp; }
	// HP割合取得
	float GetHPRate() const { return m_hpRate; }
	// 攻撃力取得
	float GetAtk() const { return m_atk; }
	// 半径取得
	float GetRadius() const { return m_radius; }
	// 死んでいるかどうか
	bool IsDead() const { return m_isDead; }

protected:
	// モデルハンドル
	int m_model = -1;
	// HP
	float m_hp = 0.0f;
	// HPの割合
	float m_hpRate = 0.0f;
	// 最大HP
	float m_maxHP = 0.0f;
	// 攻撃力
	float m_atk = 0.0f;
	// 半径
	float m_radius = 0.0f;
	// 死んでいるかどうか
	bool m_isDead = false;
	// 経過フレームを測る
	int m_frame = 0;
	// 攻撃フレーム
	int m_atkFrame = 0.0f;
	// 点滅フレーム
	int m_blinkFrame = 0.0f;
	// 正面ベクトル
	Vector3 m_forward = { 0.0f, 0.0f, 1.0f };
	// アニメーション
	Animator m_anim;
};

