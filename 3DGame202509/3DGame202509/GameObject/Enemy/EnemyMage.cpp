#include "Animator.h"
#include "CapsuleColliderData.h"
#include "EnemyMage.h"
#include "EnemyMageBullet.h"
#include "Physics.h"
#include "Player/Player.h"

#include "DxLib.h"
#include <algorithm>
#include <cassert>

namespace
{
	// アニメーション名
	// 待機
	const char* kFindAnimName   = "Idle_B";
	// 発見
	const char* kChaseAnimName  = "Running_B";
	// 攻撃
	const char* kAttackAnimName = "1H_Melee_Attack_Stab";
	// 被弾
	const char* kHitAnimName    = "Hit_B";
	// 死亡
	const char* kDeadAnimName   = "Death_B";

	// アニメーションの再生速度
	constexpr float kAnimSpeed = 1.0f;

	// エネミーがプレイヤーを発見できる範囲
	constexpr float kFindRadius   = 500.0f;
	constexpr float kAttackRadius = 300.0f;

	// 初期HP
	constexpr int kHp = 3;

	// エネミーの速度
	constexpr float kSpeed = 0.5f;

	constexpr float kColScale  = 70.0f;
	constexpr float kColRadius = 25.0f;

	// モデルの拡大率
	constexpr float kModelScale      = 45.0f;
	constexpr float kBladeModelScale = 0.01f;

	constexpr float kAttackFrame = 32.0f;
}

EnemyMage::EnemyMage()
{
}

EnemyMage::~EnemyMage()
{
}

void EnemyMage::Init(std::shared_ptr<Physics> physics, Vec3& pos, const Vec3& rot, const Vec3& scale)
{
	Collidable::Init(physics);

	m_rigidbody.Init();
	m_rigidbody.SetPos(pos);

	m_physics = physics;

	auto colData = std::dynamic_pointer_cast<CapsuleColliderData>(m_colliderData);
	colData->m_startPos = pos;
	colData->m_radius = kColRadius;

	// スピードの初期化
	m_findRadius = kFindRadius;
	m_attackRadius = kAttackRadius;
	m_hp = kHp;
	m_isDead = false;
	m_attackFrame = 0.0f;

	m_charModel = MV1LoadModel("Data/Model/Enemy/Mage/Mage.mv1");
	assert(m_charModel >= 0);
	m_weaponModel = MV1LoadModel("Data/Model/Enemy/Mage/Staff.mv1");
	assert(m_weaponModel >= 0);

	MV1SetScale(m_charModel, VGet(scale.x * 50.0f, scale.y * 50.0f, scale.z * 50.0f));
	MV1SetPosition(m_charModel, pos.ToDxVECTOR());

	m_anim.Init(m_charModel);
	m_anim.AttachAnim(m_anim.GetNextAnim(), kFindAnimName, kAnimSpeed, true);
}

void EnemyMage::Update(std::shared_ptr<Player> player)
{
	if (m_isDead && m_charModel < 0)
	{
		return;
	}

	// アニメーションの更新
	m_anim.UpdateAnim(m_anim.GetPrevAnim());
	m_anim.UpdateAnim(m_anim.GetNextAnim());
	m_anim.UpdateAnimBlend();

	// アタッチするモデルのMV1SetMatrixの設定を無効化する
	MV1SetMatrix(m_weaponModel, MGetIdent());
	// アタッチするモデルのフレームの座標を取得する
	VECTOR position = MV1GetFramePosition(m_weaponModel, 0);
	// アタッチするモデルを,フレームの座標を原点にするための平行移動行列を作成
	MATRIX transMat = MGetTranslate(VScale(position, -1.0f));
	// アタッチされるモデルのフレームの行列を取得
	MATRIX frameMat = MV1GetFrameLocalWorldMatrix(m_charModel, 14);
	// アタッチするモデルの拡大行列を取得
	MATRIX scaleMat = MGetScale(VGet(kBladeModelScale, kBladeModelScale, kBladeModelScale));
	// アタッチするモデルの回転行列を取得
	MATRIX yMat = MGetRotY(DX_PI_F);
	// 各行列を合成
	MATRIX mixMat = MGetIdent();
	mixMat = MMult(transMat, mixMat);
	mixMat = MMult(frameMat, mixMat);
	mixMat = MMult(scaleMat, mixMat);
	mixMat = MMult(yMat, mixMat);
	// 合成した行列をモデルにセット
	MV1SetMatrix(m_weaponModel, mixMat);

	switch (m_state)
	{
	case EnemyState::Find:
		FindUpdate(player);
		break;
	case EnemyState::Chase:
		ChaseUpdate(player);
		break;
	case EnemyState::Attack:
		AttackUpdate(player);
		break;
	case EnemyState::Hit:
		HitUpdate(player);
		break;
	case EnemyState::Dead:
		DeadUpdate(player);
		break;
	}

	//当たり判定
	auto colData = std::dynamic_pointer_cast<CapsuleColliderData>(m_colliderData);
	Vec3 colPos = m_rigidbody.GetPos();
	colPos.y += kColScale;
	colData->m_startPos = colPos;

	MV1SetPosition(m_charModel, m_rigidbody.GetPos().ToDxVECTOR());

	for (auto& bullet : m_bullets)
	{
		bullet->Update();
	}
	m_bullets.erase(std::remove_if(m_bullets.begin(), m_bullets.end(), [this](const std::shared_ptr<EnemyMageBullet>& b)
	{
		if (b->IsDead())
		{
			b->Final(m_physics);
			return true;
		}
		return false;
	}),
	m_bullets.end()
	);
}

void EnemyMage::Draw()
{
	if (m_isDead && m_charModel < 0)
	{
		return;
	}

#if _DEBUG
	DrawSphere3D(m_rigidbody.GetPos().ToDxVECTOR(), 10.0f, 16, 0x0000ff, 0x0000ff, true);
	DrawSphere3D(m_rigidbody.GetPos().ToDxVECTOR(), m_findRadius, 16, 0xff00ff, 0xff00ff, false);
	DrawSphere3D(m_rigidbody.GetPos().ToDxVECTOR(), m_attackRadius, 16, 0xff00ff, 0xff00ff, false);

#endif

	MV1DrawModel(m_charModel);
	MV1DrawModel(m_weaponModel);

	for (auto& bullet : m_bullets)
	{
		bullet->Draw();
	}

	Vec3 worldPos = m_rigidbody.GetPos();
	worldPos.y += 120.0f; // 頭上の高さ調整

	VECTOR worldVec = worldPos.ToDxVECTOR();

	// 3D→2D座標変換（戻り値がスクリーン座標）
	VECTOR screenVec = ConvWorldPosToScreenPos(worldVec);

	const int gaugeWidth = 100;
	const int gaugeHeight = 10;

	int gaugeX = static_cast<int>(screenVec.x - gaugeWidth / 2);
	int gaugeY = static_cast<int>(screenVec.y - gaugeHeight / 2);

	float hpRate = static_cast<float>(m_hp) / kHp;
	hpRate = std::clamp(hpRate, 0.0f, 1.0f);

	DrawBox(gaugeX, gaugeY,
		gaugeX + gaugeWidth,
		gaugeY + gaugeHeight,
		0x808080, true);

	int hpBarWidth = static_cast<int>(gaugeWidth * hpRate);
	DrawBox(gaugeX, gaugeY,
		gaugeX + hpBarWidth,
		gaugeY + gaugeHeight,
		0xff0000, true);

	DrawBox(gaugeX, gaugeY,
		gaugeX + gaugeWidth,
		gaugeY + gaugeHeight,
		0x000000, false);
}

void EnemyMage::OnDamage()
{
	m_hp -= 1;

	if (m_hp <= 0 && !m_isDead)
	{
		ChangeState(EnemyState::Dead, kAnimSpeed);
	}
	else
	{
		ChangeState(EnemyState::Hit, kAnimSpeed);
	}
}

const char* EnemyMage::GetAnimName(EnemyState state) const
{
	switch (state)
	{
	case EnemyState::Find:
		return kFindAnimName;
	case EnemyState::Chase:
		return kChaseAnimName;
	case EnemyState::Attack:
		return kAttackAnimName;
	case EnemyState::Hit:
		return kHitAnimName;
	case EnemyState::Dead:
		return kDeadAnimName;
	default:
		return "";
		assert(0 && "存在しないアニメーション");
	}
}

bool EnemyMage::IsLoopAnim(EnemyState state) const
{
	switch (state)
	{
	case EnemyState::Find:
		return true;
	case EnemyState::Chase:
		return true;
	case EnemyState::Attack:
		return false;
	case EnemyState::Hit:
		return false;
	case EnemyState::Dead:
		return false;
	default:
		return "";
		assert(0 && "存在しないステート");
	}
}

void EnemyMage::FindUpdate(std::shared_ptr<Player> player)
{
	SetActive(true);

	float distance = (m_rigidbody.GetPos() - player->GetPos()).Length();
	if (distance <= (m_findRadius + player->GetRadius()))
	{
		ChangeState(EnemyState::Chase, kAnimSpeed);
	}
}

void EnemyMage::ChaseUpdate(std::shared_ptr<Player> player)
{
	SetActive(true);

	// プレイヤーへの方向ベクトル
	Vec3 myPos = m_rigidbody.GetPos();
	Vec3 toPlayerDir = player->GetPos() - myPos;
	toPlayerDir.y = 0.0f;

	// 距離が十分にある場合のみ移動
	if (toPlayerDir.Length() > 1.0f) 
	{
		toPlayerDir.Normalize();
		m_rigidbody.SetVelo(toPlayerDir * kSpeed);
		MV1SetPosition(m_charModel, myPos.ToDxVECTOR());

		// 進行方向が0でなければ回転
		if (m_rigidbody.GetVelo().x != 0.0f || m_rigidbody.GetVelo().z != 0.0f)
		{
			// atan2でY軸回転角を計算（Zが前、Xが右の座標系の場合）
			float angleY = std::atan2(m_rigidbody.GetVelo().x, -m_rigidbody.GetVelo().z);
			MV1SetRotationXYZ(m_charModel, VGet(0.0f, -angleY, 0.0f));
		}
	}

	float distance = (myPos - player->GetPos()).Length();
	if (distance >= (m_findRadius + player->GetRadius()))
	{
		ChangeState(EnemyState::Find, kAnimSpeed);
		m_rigidbody.SetVelo({ 0.0f, 0.0f, 0.0f });
	}

	if (distance <= (m_attackRadius + player->GetRadius()))
	{
		ChangeState(EnemyState::Attack, kAnimSpeed);
		m_rigidbody.SetVelo({ 0.0f, 0.0f, 0.0f });
	}
}

void EnemyMage::AttackUpdate(std::shared_ptr<Player> player)
{
	SetActive(true);

	if (m_attackFrame <= kAttackFrame)
	{
		++m_attackFrame;
		// プレイヤーへの方向ベクトル
		Vec3 myPos = m_rigidbody.GetPos();
		Vec3 toPlayerDir = player->GetPos() - myPos;
		toPlayerDir.y = 0.0f;
		if (toPlayerDir.x != 0.0f || toPlayerDir.z != 0.0f)
		{
			// atan2でY軸回転角を計算（Zが前、Xが右の座標系の場合）
			float angleY = std::atan2(toPlayerDir.x, -toPlayerDir.z);
			MV1SetRotationXYZ(m_charModel, VGet(0.0f, -angleY, 0.0f));
		}
	}

	if (m_attackFrame == kAttackFrame)
	{
		// 弾を生成
		Vec3 myPos = m_rigidbody.GetPos();
		Vec3 playerPos = player->GetPos();

		auto bullet = std::make_shared<EnemyMageBullet>();
		bullet->Init(myPos, playerPos, m_physics);
		m_bullets.push_back(bullet);
	}

	if (m_anim.GetNextAnim().isEnd)
	{
		float distance = (m_rigidbody.GetPos() - player->GetPos()).Length();
		if (distance >= (m_findRadius + player->GetRadius()))
		{
			ChangeState(EnemyState::Find, kAnimSpeed);
			m_attackFrame = 0;
		}
		else
		{
			ChangeState(EnemyState::Chase, kAnimSpeed);
			m_attackFrame = 0;
		}
	}
}

void EnemyMage::HitUpdate(std::shared_ptr<Player> player)
{
	SetActive(false);

	MV1SetPosition(m_charModel, m_rigidbody.GetPos().ToDxVECTOR());
	// アニメーションが終了したら待機状態に戻る
	if (m_anim.GetNextAnim().isEnd)
	{
		ChangeState(EnemyState::Find, kAnimSpeed);
		m_rigidbody.SetVelo({ 0.0f, 0.0f, 0.0f });
	}
}

void EnemyMage::DeadUpdate(std::shared_ptr<Player> player)
{
	SetActive(false);

	// アニメーションが終了したら待機状態に戻る
	if (m_anim.GetNextAnim().isEnd)
	{
		if (m_charModel >= 0)
		{
			MV1DeleteModel(m_charModel);
			m_charModel = -1;
		}
		m_isDead = true;
	}

	for (auto& bullet : m_bullets)
	{
		bullet->Final(m_physics);
	}
	m_bullets.clear();
}
