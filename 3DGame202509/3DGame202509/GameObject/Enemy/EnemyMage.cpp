#include "CapsuleColliderData.h"
#include "Bullet/EnemyMageBullet.h"
#include "Player/Player.h"
#include "EnemyMage.h"

namespace
{
	// HPの初期値、最大値
	constexpr float kHP = 3.0f;
	// 攻撃力
	constexpr float kDefaultAtk = 1.0f;

	// 移動速度
	constexpr float kRunSpeed = 0.5f;

	// モデルの拡大値
	constexpr float kModelScale = 75.0f;
	constexpr float kWeaponModelScale = 0.01f;

	// 攻撃フレーム
	constexpr float kAtkFrame = 32.0f;

	// 当たり判定
	// カプセルの半径
	constexpr float kCapsuleColRadius = 45.0f;
	// カプセルの長さ
	constexpr float kColScale = 140.0f;
	// 状態遷移に使う半径
	// プレイヤーを探知できる範囲
	constexpr float kPlayerFindRadius = 500.0f;
	// 攻撃状態に移行する範囲
	constexpr float kAtkRadius = 300.0f;

	// 視野角
	constexpr float kViewAngleRad = DX_PI_F / 2.0f; // 90度
	// 見える距離
	constexpr float kViewDistance = 1500.0f;

	// アニメーション名
	// 待機
	const char* kFindAnimName = "Idle_B";
	// 歩き
	const char* kWalkAnimName = "Walking_D_Skeletons";
	// 発見
	const char* kChaseAnimName = "Running_B";
	// 攻撃
	const char* kAttackAnimName = "1H_Melee_Attack_Stab";
	// 被弾
	const char* kHitAnimName = "Hit_B";
	// 死亡
	const char* kDeadAnimName = "Death_B";

	// アニメーションの再生速度
	// 通常速度
	constexpr float kDefaultAnimSpeed = 1.0f;
	constexpr float kAtkAnimSpeed = 0.5f;

	// 倒されたときにプレイヤーの必殺技ゲージを増やす量
	constexpr int kSpecialGaugePoint = 10;
}

EnemyMage::EnemyMage() :
	m_angle(0.0f),
	m_rotSpeed(1.5f),
	m_angleMax(DX_PI_F / 4.0f),
	EnemyBase(ObjectTag::Enemy, ObjectPriority::Low, ColliderData::Kind::Capsule)
{
}

EnemyMage::~EnemyMage()
{
	MV1DeleteModel(m_model);
}

void EnemyMage::Init(Vector3& pos, Vector3& rot, Vector3& scale)
{
	// Physicsに登録
	Collidable::Init();
	m_rigidbody.Init();
	m_rigidbody.SetPos(pos);

	auto colData = std::dynamic_pointer_cast<CapsuleColliderData>(m_colliderData);
	colData->m_startPos = pos;
	colData->m_radius = kCapsuleColRadius;

	m_playerFindRadius = kPlayerFindRadius;
	m_atkRadius = kAtkRadius;

	// 各ステータスの初期化
	m_status.m_hp = kHP;
	m_status.m_maxHP = kHP;
	m_status.m_atk = kDefaultAtk;
	m_atkFrame = 0.0f;
	m_isDead = false;

	// モデルのロード
	m_model = MV1LoadModel("Data/Model/Enemy/Mage/Mage.mv1");
	assert(m_model >= 0);
	m_weaponModel = MV1LoadModel("Data/Model/Enemy/Mage/Staff.mv1");
	assert(m_weaponModel >= 0);

	VECTOR modelScale = VGet(scale.x * kModelScale, scale.y * kModelScale, scale.z * kModelScale);
	MV1SetScale(m_model, modelScale);
	MV1SetPosition(m_model, pos.ToDxVECTOR());
	MV1SetRotationXYZ(m_model, VGet(rot.x, rot.y, rot.z));

	// アニメーター
	m_anim.Init(m_model);
	m_anim.AttachAnim(m_anim.GetNextAnim(), kFindAnimName, kDefaultAnimSpeed, true);
}

void EnemyMage::Draw()
{
	if (m_isDead && m_model < 0)
	{
		return;
	}

#if _DEBUG
	DrawSphere3D(m_rigidbody.GetPos().ToDxVECTOR(), 10.0f, 16, 0x0000ff, 0x0000ff, true);
#endif

	MV1DrawModel(m_model);
	MV1DrawModel(m_weaponModel);

	for (auto& bullet : m_bullets)
	{
		bullet->Draw();
	}

	Vector3 pos = m_rigidbody.GetPos();
	VECTOR base = pos.ToDxVECTOR();

	// 首振り角度
	float offsetAngle = std::sin(m_angle * m_rotSpeed) * m_angleMax;

	// 前方向（または Rigidbody に保存されてる向き）
	Vector3 forward = m_rigidbody.GetDir();
	if (forward.Length() == 0.0f)
	{
		forward = Vector3(0, 0, -1);
	}
	forward.Normalize();

	// スイングした方向（Y軸回転）
	Vector3 swingDir{
		forward.x * std::cos(offsetAngle) - forward.z * std::sin(offsetAngle),
		0.0f,
		forward.x * std::sin(offsetAngle) + forward.z * std::cos(offsetAngle)
	};
	swingDir.Normalize();

	// 左右端方向を計算
	float halfAngle = kViewAngleRad * 0.5f;

	Vector3 left{
		swingDir.x * std::cos(halfAngle) - swingDir.z * std::sin(halfAngle),
		0.0f,
		swingDir.x * std::sin(halfAngle) + swingDir.z * std::cos(halfAngle)
	};
	Vector3 right{
		swingDir.x * std::cos(-halfAngle) - swingDir.z * std::sin(-halfAngle),
		0.0f,
		swingDir.x * std::sin(-halfAngle) + swingDir.z * std::cos(-halfAngle)
	};

	left.Normalize();
	right.Normalize();

	Vector3 frontEnd = pos + swingDir * kViewDistance;
	Vector3 leftEnd = pos + left * kViewDistance;
	Vector3 rightEnd = pos + right * kViewDistance;

	DrawLine3D(base, frontEnd.ToDxVECTOR(), GetColor(255, 255, 0)); // 正面
	DrawLine3D(base, leftEnd.ToDxVECTOR(), GetColor(0, 255, 255));  // 左端
	DrawLine3D(base, rightEnd.ToDxVECTOR(), GetColor(0, 255, 255)); // 右端

	// 扇形の円弧を描画
	constexpr int kSegments = 24;
	for (int i = 0; i < kSegments; ++i)
	{
		float t1 = -halfAngle + (kViewAngleRad * i / kSegments);
		float t2 = -halfAngle + (kViewAngleRad * (i + 1) / kSegments);

		Vector3 d1{
			swingDir.x * std::cos(t1) - swingDir.z * std::sin(t1),
			0.0f,
			swingDir.x * std::sin(t1) + swingDir.z * std::cos(t1)
		};
		Vector3 d2{
			swingDir.x * std::cos(t2) - swingDir.z * std::sin(t2),
			0.0f,
			swingDir.x * std::sin(t2) + swingDir.z * std::cos(t2)
		};

		Vector3 p1 = pos + d1 * kViewDistance;
		Vector3 p2 = pos + d2 * kViewDistance;

		DrawLine3D(p1.ToDxVECTOR(), p2.ToDxVECTOR(), GetColor(255, 0, 255));
	}
}

void EnemyMage::Update(std::shared_ptr<Player> player)
{
	if (m_isDead && m_model < 0)
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
	MATRIX frameMat = MV1GetFrameLocalWorldMatrix(m_model, 14);
	// アタッチするモデルの拡大行列を取得
	MATRIX scaleMat = MGetScale(VGet(kWeaponModelScale, kWeaponModelScale, kWeaponModelScale));
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
	case EnemyState::Walk:
		WalkUpdate(player);
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
	Vector3 colPos = m_rigidbody.GetPos();
	colPos.y += kColScale;
	colData->m_startPos = colPos;

	MV1SetPosition(m_model, m_rigidbody.GetPos().ToDxVECTOR());

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

void EnemyMage::FindUpdate(std::shared_ptr<Player> player)
{
	SetActive(true);

	// 経過時間で左右に回転
	m_angle += 1.0f / 60.0f;  // 毎フレーム約1/60秒加算

	float offsetAngle = std::sin(m_angle * m_rotSpeed) * m_angleMax;

	// 基本の向きベクトル（前方向）
	Vector3 forward = m_rigidbody.GetDir();
	if (forward.Length() == 0.0f)
	{
		forward = Vector3(0, 0, -1);
	}
	forward.Normalize();

	// 探知中心方向（スイングで回転）
	Vector3 dir
	{
		forward.x * std::cos(offsetAngle) - forward.z * std::sin(offsetAngle),
		0.0f,
		forward.x * std::sin(offsetAngle) + forward.z * std::cos(offsetAngle)
	};
	dir.Normalize();

	// モデルの回転を設定（Z前提 → atan2）
	float angleY = std::atan2(dir.x, -dir.z); // X右/Z前の座標系
	MV1SetRotationXYZ(m_model, VGet(0.0f, -angleY, 0.0f));

	// 探知処理：この方向を中心に視野角で探す
	if (IsPlayerFind(player, dir, kViewAngleRad, kViewDistance))
	{
		ChangeState(EnemyState::Attack);
	}
}

void EnemyMage::WalkUpdate(std::shared_ptr<Player> player)
{
}

void EnemyMage::ChaseUpdate(std::shared_ptr<Player> player)
{
	SetActive(true);

	// プレイヤーへの方向ベクトル
	Vector3 myPos = m_rigidbody.GetPos();
	Vector3 toPlayerDir = player->GetPos() - myPos;
	toPlayerDir.y = 0.0f;

	// 距離が十分にある場合のみ移動
	if (toPlayerDir.Length() > 1.0f)
	{
		toPlayerDir.Normalize();
		m_rigidbody.SetVelo(toPlayerDir * kRunSpeed);
		MV1SetPosition(m_model, myPos.ToDxVECTOR());

		// 進行方向が0でなければ回転
		if (m_rigidbody.GetVelo().x != 0.0f || m_rigidbody.GetVelo().z != 0.0f)
		{
			// atan2でY軸回転角を計算（Zが前、Xが右の座標系の場合）
			float angleY = std::atan2(m_rigidbody.GetVelo().x, -m_rigidbody.GetVelo().z);
			MV1SetRotationXYZ(m_model, VGet(0.0f, -angleY, 0.0f));
		}
	}

	float distance = (myPos - player->GetPos()).Length();
	if (distance >= (m_playerFindRadius + player->GetRadius()))
	{
		ChangeState(EnemyState::Find);
		m_rigidbody.SetVelo({ 0.0f, 0.0f, 0.0f });
	}

	if (distance <= (m_atkRadius + player->GetRadius()))
	{
		ChangeState(EnemyState::Attack);
		m_rigidbody.SetVelo({ 0.0f, 0.0f, 0.0f });
	}
}

void EnemyMage::AttackUpdate(std::shared_ptr<Player> player)
{
	SetActive(true);

	if (m_atkFrame <= kAtkFrame)
	{
		++m_atkFrame;
		// プレイヤーへの方向ベクトル
		Vector3 myPos = m_rigidbody.GetPos();
		Vector3 toPlayerDir = player->GetPos() - myPos;
		toPlayerDir.y = 0.0f;
		if (toPlayerDir.x != 0.0f || toPlayerDir.z != 0.0f)
		{
			// atan2でY軸回転角を計算（Zが前、Xが右の座標系の場合）
			float angleY = std::atan2(toPlayerDir.x, -toPlayerDir.z);
			MV1SetRotationXYZ(m_model, VGet(0.0f, -angleY, 0.0f));
		}
	}

	if (m_atkFrame == kAtkFrame)
	{
		// 弾を生成
		Vector3 myPos = m_rigidbody.GetPos();
		myPos.y += 50.0f;
		Vector3 playerPos = player->GetPos();
		Vector3 playerDir = playerPos - myPos;

		auto bullet = std::make_shared<EnemyMageBullet>();
		bullet->Init(myPos, playerDir);
		m_bullets.push_back(bullet);
	}

	if (m_anim.GetNextAnim().isEnd)
	{
		ChangeState(EnemyState::Find);
		m_atkFrame = 0;
	}
}

void EnemyMage::HitUpdate(std::shared_ptr<Player> player)
{
	SetActive(false);

	MV1SetPosition(m_model, m_rigidbody.GetPos().ToDxVECTOR());
	// アニメーションが終了したら待機状態に戻る
	if (m_anim.GetNextAnim().isEnd)
	{
		ChangeState(EnemyState::Attack);
		m_rigidbody.SetVelo({ 0.0f, 0.0f, 0.0f });
	}
}

void EnemyMage::DeadUpdate(std::shared_ptr<Player> player)
{
	SetActive(false);

	/*for (auto& bullet : m_bullets)
	{
		bullet->Final();
	}
	m_bullets.clear();*/

	// アニメーションが終了したら待機状態に戻る
	if (m_anim.GetNextAnim().isEnd)
	{
		if (m_model >= 0)
		{
			MV1DeleteModel(m_model);
			m_model = -1;
		}
		m_isDead = true;
		player->SetSpecialGauge(kSpecialGaugePoint);
		return;
	}
}

bool EnemyMage::IsPlayerFind(const std::shared_ptr<Player>& player, const Vector3& centerDir, float viewAngleRad, float viewDistance)
{
	// 現在位置を取得
	Vector3 myPos = m_rigidbody.GetPos();
	// 自分からPlayerに向かうベクトルを生成
	Vector3 toPlayer = player->GetPos() - myPos;

	// Playerに向かうベクトルの長さが視界距離より長ければ見えないのでfalse
	if (toPlayer.Length() > viewDistance)
		return false;

	// Playerに向かうベクトルと自分の視線方向ベクトルを正規化
	Vector3 toPlayerNorm = toPlayer.GetNormalize();
	Vector3 centerNorm = centerDir.GetNormalize();

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
	case EnemyState::Walk:
		return kWalkAnimName;
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

float EnemyMage::GetAnimPlaySpeed(EnemyState state) const
{
	switch (state)
	{
	case EnemyState::Find:
		return kDefaultAnimSpeed;
	case EnemyState::Walk:
		return kDefaultAnimSpeed;
	case EnemyState::Chase:
		return kDefaultAnimSpeed;
	case EnemyState::Attack:
		return kAtkAnimSpeed;
	case EnemyState::Hit:
		return kDefaultAnimSpeed;
	case EnemyState::Dead:
		return kDefaultAnimSpeed;
	}
}

bool EnemyMage::IsLoopAnim(EnemyState state) const
{
	switch (state)
	{
	case EnemyState::Find:
		return true;
	case EnemyState::Walk:
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
