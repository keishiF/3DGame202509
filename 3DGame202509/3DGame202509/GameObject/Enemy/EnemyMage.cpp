#include "Animator.h"
#include "CapsuleColliderData.h"
#include "EnemyMage.h"
#include "EnemyMageBullet.h"
#include "Physics.h"
#include "Player/Player.h"
#include <algorithm>
#include <cassert>
#include <DxLib.h>

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
	constexpr float kAttackAnimSpeed = 0.5f;

	// エネミーがプレイヤーを発見できる範囲
	constexpr float kFindRadius   = 500.0f;
	constexpr float kAttackRadius = 300.0f;

	// 初期HP
	constexpr int kHp = 3;

	// エネミーの速度
	constexpr float kRunSpeed = 0.5f;

	constexpr float kColScale  = 140.0f;
	constexpr float kColRadius = 45.0f;

	// モデルの拡大率
	constexpr float kModelScale      = 75.0f;
	constexpr float kBladeModelScale = 0.01f;

	constexpr float kAttackFrame = 32.0f;

	// 視野角
	constexpr float kViewAngleRad = DX_PI_F / 2.0f;  // 90度
	// 見える距離
	constexpr float kViewDistance = 750.0f;
}

EnemyMage::EnemyMage() :
	m_angle(0.0f),
	m_rotSpeed(1.5f),
	m_angleMax(DX_PI_F / 4.0f)
{
}

EnemyMage::~EnemyMage()
{
}

void EnemyMage::Init(Vec3& pos, Vec3& rot, Vec3& scale)
{
	Collidable::Init();

	m_rigidbody.Init();
	m_rigidbody.SetPos(pos);

	auto colData = std::dynamic_pointer_cast<CapsuleColliderData>(m_colliderData);
	colData->m_startPos = pos;
	colData->m_radius = kColRadius;

	// メンバ変数の初期化
	m_findRadius = kFindRadius;
	m_attackRadius = kAttackRadius;
	m_hp = kHp;
	m_isDead = false;
	m_attackFrame = 0.0f;
	m_forward = { 0.0f, 0.0f, -1.0f };

	m_charModel = MV1LoadModel("Data/Model/Enemy/Mage/Mage.mv1");
	assert(m_charModel >= 0);
	m_weaponModel = MV1LoadModel("Data/Model/Enemy/Mage/Staff.mv1");
	assert(m_weaponModel >= 0);

	MV1SetScale(m_charModel, VGet(scale.x * kModelScale, scale.y * kModelScale, scale.z * kModelScale));
	MV1SetPosition(m_charModel, pos.ToDxVECTOR());
	MV1SetRotationXYZ(m_charModel, VGet(rot.x, rot.y, rot.z));

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
			b->Final();
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

#ifdef _DEBUG
	Vec3 pos = m_rigidbody.GetPos();
	VECTOR base = pos.ToDxVECTOR();

	// 首振り角度
	float offsetAngle = std::sin(m_angle * m_rotSpeed) * m_angleMax;

	// 前方向（または Rigidbody に保存されてる向き）
	Vec3 forward = m_rigidbody.GetDir();
	if (forward.Length() == 0.0f)
	{
		forward = Vec3(0, 0, -1);
	}
	forward.Normalize();

	// スイングした方向（Y軸回転）
	Vec3 swingDir{
		forward.x * std::cos(offsetAngle) - forward.z * std::sin(offsetAngle),
		0.0f,
		forward.x * std::sin(offsetAngle) + forward.z * std::cos(offsetAngle)
	};
	swingDir.Normalize();

	// 左右端方向を計算
	float halfAngle = kViewAngleRad * 0.5f;

	Vec3 left{
		swingDir.x * std::cos(halfAngle) - swingDir.z * std::sin(halfAngle),
		0.0f,
		swingDir.x * std::sin(halfAngle) + swingDir.z * std::cos(halfAngle)
	};
	Vec3 right{
		swingDir.x * std::cos(-halfAngle) - swingDir.z * std::sin(-halfAngle),
		0.0f,
		swingDir.x * std::sin(-halfAngle) + swingDir.z * std::cos(-halfAngle)
	};

	left.Normalize();
	right.Normalize();

	Vec3 frontEnd = pos + swingDir * kViewDistance;
	Vec3 leftEnd = pos + left * kViewDistance;
	Vec3 rightEnd = pos + right * kViewDistance;

	DrawLine3D(base, frontEnd.ToDxVECTOR(), GetColor(255, 255, 0)); // 正面
	DrawLine3D(base, leftEnd.ToDxVECTOR(), GetColor(0, 255, 255));  // 左端
	DrawLine3D(base, rightEnd.ToDxVECTOR(), GetColor(0, 255, 255)); // 右端

	// 扇形の円弧を描画
	constexpr int kSegments = 24;
	for (int i = 0; i < kSegments; ++i)
	{
		float t1 = -halfAngle + (kViewAngleRad * i / kSegments);
		float t2 = -halfAngle + (kViewAngleRad * (i + 1) / kSegments);

		Vec3 d1{
			swingDir.x * std::cos(t1) - swingDir.z * std::sin(t1),
			0.0f,
			swingDir.x * std::sin(t1) + swingDir.z * std::cos(t1)
		};
		Vec3 d2{
			swingDir.x * std::cos(t2) - swingDir.z * std::sin(t2),
			0.0f,
			swingDir.x * std::sin(t2) + swingDir.z * std::cos(t2)
		};

		Vec3 p1 = pos + d1 * kViewDistance;
		Vec3 p2 = pos + d2 * kViewDistance;

		DrawLine3D(p1.ToDxVECTOR(), p2.ToDxVECTOR(), GetColor(255, 0, 255));
	}
#endif
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

void EnemyMage::FindUpdate(std::shared_ptr<Player> player)
{
	SetActive(true);

	// 経過時間で左右に回転
	m_angle += 1.0f / 60.0f;  // 毎フレーム約1/60秒加算

	float offsetAngle = std::sin(m_angle * m_rotSpeed) * m_angleMax;

	// 基本の向きベクトル（前方向）
	Vec3 forward = m_rigidbody.GetDir();
	if (forward.Length() == 0.0f)
	{
		forward = Vec3(0, 0, -1);
	}
	forward.Normalize();

	// 探知中心方向（スイングで回転）
	Vec3 dir
	{
		forward.x * std::cos(offsetAngle) - forward.z * std::sin(offsetAngle),
		0.0f,
		forward.x * std::sin(offsetAngle) + forward.z * std::cos(offsetAngle)
	};
	dir.Normalize();

	// モデルの回転を設定（Z前提 → atan2）
	float angleY = std::atan2(dir.x, -dir.z); // X右/Z前の座標系
	MV1SetRotationXYZ(m_charModel, VGet(0.0f, -angleY, 0.0f));

	// 探知処理：この方向を中心に視野角で探す
	if (IsPlayerFind(player, dir, kViewAngleRad, kViewDistance))
	{
		ChangeState(EnemyState::Attack, kAttackAnimSpeed);
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
		m_rigidbody.SetVelo(toPlayerDir * kRunSpeed);
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
		myPos.y += 50.0f;
		Vec3 playerPos = player->GetPos();

		auto bullet = std::make_shared<EnemyMageBullet>();
		bullet->Init(myPos, playerPos);
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
		bullet->Final();
	}
	m_bullets.clear();
}

bool EnemyMage::IsPlayerFind(const std::shared_ptr<Player>& player, const Vec3& centerDir, float viewAngleRad, float viewDistance)
{
	// 現在位置を取得
	Vec3 myPos = m_rigidbody.GetPos();
	// 自分からPlayerに向かうベクトルを生成
	Vec3 toPlayer = player->GetPos() - myPos;

	// Playerに向かうベクトルの長さが視界距離より長ければ見えないのでfalse
	if (toPlayer.Length() > viewDistance)
		return false;

	// Playerに向かうベクトルと自分の視線方向ベクトルを正規化
	Vec3 toPlayerNorm = toPlayer.GetNormalize();
	Vec3 centerNorm = centerDir.GetNormalize();

	float dot = centerNorm.Dot(toPlayerNorm);
	float cosHalfView = std::cos(viewAngleRad * 0.5f);

	return dot >= cosHalfView;
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
