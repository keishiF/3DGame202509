#include "CapsuleColliderData.h"
#include "EnemyMinionWeapon.h"
#include "SubEnemyBossLeftWeapon.h"

namespace
{
	// 当たり判定の半径
	constexpr float kRadius = 7.5f;
	constexpr float kBladeModelScale = 0.01f;
}

EnemyMinionWeapon::EnemyMinionWeapon() :
	Collidable(ObjectTag::EnemyWeapon, ObjectPriority::Low, ColliderData::Kind::Polygon)
{
}

EnemyMinionWeapon::~EnemyMinionWeapon()
{
	MV1DeleteModel(m_model);
}

void EnemyMinionWeapon::Init()
{
	Collidable::Init();
	m_rigidbody.Init();

	m_model = MV1LoadModel("Data/Model/Enemy/Minion/Blade.mv1");
	assert(m_model >= 0);
}

void EnemyMinionWeapon::Update(int model, float currentFrame, const MinionAtk::AtkTiming& timing)
{
	if (currentFrame >= timing.start && currentFrame < timing.end)
	{
		AttackUpdate(model); // 攻撃判定ON
	}
	else
	{
		IdleUpdate(model);   // 攻撃判定OFF
	}
}

void EnemyMinionWeapon::IdleUpdate(int model)
{
	// 当たり判定を無効化する
	SetActive(false);

	// アタッチするモデルのMV1SetMatrixの設定を無効化する
	MV1SetMatrix(m_model, MGetIdent());
	// アタッチするモデルのフレームの座標を取得する
	VECTOR position = MV1GetFramePosition(m_model, 0);
	// アタッチするモデルを,フレームの座標を原点にするための平行移動行列を作成
	MATRIX transMat = MGetTranslate(VScale(position, -1.0f));
	// アタッチされるモデルのフレームの行列を取得
	MATRIX frameMat = MV1GetFrameLocalWorldMatrix(model, 14);
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
	MV1SetMatrix(m_model, mixMat);
}

void EnemyMinionWeapon::AttackUpdate(int model)
{
	// 当たり判定を有効化する
	SetActive(true);

	// アタッチするモデルのMV1SetMatrixの設定を無効化する
	MV1SetMatrix(m_model, MGetIdent());
	// アタッチするモデルのフレームの座標を取得する
	VECTOR position = MV1GetFramePosition(m_model, 0);
	// アタッチするモデルを,フレームの座標を原点にするための平行移動行列を作成
	MATRIX transMat = MGetTranslate(VScale(position, -1.0f));
	// アタッチされるモデルのフレームの行列を取得
	MATRIX frameMat = MV1GetFrameLocalWorldMatrix(model, 14);
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
	MV1SetMatrix(m_model, mixMat);

	//当たり判定
	auto colData = std::dynamic_pointer_cast<CapsuleColliderData>(m_colliderData);
	colData->m_radius = kRadius;
	VECTOR start = MV1GetFramePosition(m_model, 2);
	VECTOR end = MV1GetFramePosition(m_model, 1);
	Vector3 startPos = { start.x, start.y, start.z };
	Vector3 endPos = { end.x, end.y, end.z };
	m_rigidbody.SetPos(endPos);
	colData->m_startPos = startPos;
}

void EnemyMinionWeapon::Draw()
{
	MV1DrawModel(m_model);
}

void EnemyMinionWeapon::OnCollide(std::shared_ptr<Collidable> collider)
{
}
