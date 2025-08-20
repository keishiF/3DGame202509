#pragma once
#include "Animator.h"
#include "Collidable.h"
#include "Quaternion.h"
#include "Vector3.h"
#include <DxLib.h>
#include <memory>
#include <vector>

/// <summary>
/// キャラクターの基底クラス
/// </summary>
class CharacterBase : public Collidable
{
public:
	virtual void Init(Vector3& pos, Vector3& rot, Vector3& scale) abstract;
	virtual void Draw() abstract;
	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;
	virtual void OnDamage(float atk) abstract;

	// ゲッター
	// モデル取得
	int GetModel() const { return m_model; }
	// 位置取得
	Vector3 GetPos() const { return m_pos; }
	// 半径取得
	float GetRadius() const { return m_radius; }
	// HP取得
	float GetHP() const { return m_hp; }
	// HP割合取得
	float GetHPRate() const { return m_hpRate; }
	// 攻撃力取得
	float GetAtk() const { return m_atk; }
	// 死んでいるかどうか
	bool IsDead() const { return m_isDead; }

private:
	// モデルハンドル
	int m_model;
	// HP
	float m_hp = 0.0f;
	// HPの割合
	float m_hpRate = 0.0f;
	// 攻撃力
	float m_atk = 0.0f;
	// 位置情報
	Vector3 m_pos = m_rigidbody.GetPos();
	// 半径
	float m_radius = 0.0f;
	// 死んでいるかどうか
	bool m_isDead = false;
	// 攻撃フレーム
	float m_atkFrame;
	// アニメーション
	Animator m_anim;
};

