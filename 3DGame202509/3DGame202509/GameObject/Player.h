#pragma once

#include "DxLib.h"
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

private:
	// プレイヤーのモデル
	int m_model;
	// プレイヤーの位置
	Vec3 m_pos;
	// プレイヤーの移動量
	Vec3 m_vec;
	// プレイヤーのHP
	int m_hp;
	float m_jumpSpeed;
	float m_gravity;

	// 経過フレームを測る
	float m_frameCount;
};

