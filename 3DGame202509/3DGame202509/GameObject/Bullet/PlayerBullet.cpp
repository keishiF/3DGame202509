#include "PlayerBullet.h"
#include "SphereColliderData.h"
#include <EffekseerForDxLib.h>

namespace
{
	// 弾速
	constexpr float kRunSpeed = 15.0f;
	// 弾の生存時間
	constexpr float kLifeFrame = 100.0f;
	// 弾の半径
	constexpr float kColRadius = 30.0f;
}

PlayerBullet::PlayerBullet() :
	BulletBase(ObjectTag::PlayerWeapon, ObjectPriority::Low, ColliderData::Kind::Sphere)
{
}

PlayerBullet::~PlayerBullet()
{
}

void PlayerBullet::Init(Vector3 pos, Vector3 dir)
{
	Collidable::Init();
	m_rigidbody.Init();
	m_rigidbody.SetPos(pos);

	m_dir = dir;
	m_dir.y = 0.0f;
	m_dir.Normalize();

	auto colData = std::dynamic_pointer_cast<SphereColliderData>(m_colliderData);
	colData->m_radius = kColRadius;

	m_bulletEffect = LoadEffekseerEffect("Data/Effect/BulletEffect.efkefc", 50.0f);
	assert(m_bulletEffect >= 0);
}

void PlayerBullet::Update()
{
	++m_lifeFrame;
	// 移動処理
	m_rigidbody.SetVelo(m_dir * kRunSpeed);
	
	if (m_playingEffect == -1)
	{
		m_playingEffect = PlayEffekseer3DEffect(m_bulletEffect);
	}
	SetPosPlayingEffekseer3DEffect(m_playingEffect,
		m_rigidbody.GetPos().x,
		m_rigidbody.GetPos().y,
		m_rigidbody.GetPos().z);

	float angleY = std::atan2(m_dir.x, m_dir.z);
	float angleX = -DX_PI_F / 2.0f;
	// エフェクトの向きを進行方向に合わせる
	SetRotationPlayingEffekseer3DEffect(m_playingEffect, angleX, angleY, 0.0f);

	if (m_lifeFrame >= kLifeFrame)
	{
		m_isDead = true;
	}
}

void PlayerBullet::Draw()
{
	//DrawSphere3D(m_rigidbody.GetPos().ToDxVECTOR(), kColRadius, 16, 0xff0000, 0xff0000, true);
}
