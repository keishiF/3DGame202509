#include "CapsuleColliderData.h"
#include "Collidable.h"
#include "Enemy/EnemyMage.h"
#include "Enemy/EnemyMinion.h"
#include "Physics.h"
#include "PolygonColliderData.h"
#include "SphereColliderData.h"
#include "Stage/StageObjectBase.h"
#include "Vec3.h"
#include <algorithm>
#include <cassert>
#include <DxLib.h>

namespace
{
	constexpr float kOverlapGap = 1.0f;
}

Physics::Physics() :
	m_wall()
{

}

Physics& Physics::Instance()
{
	static Physics instance;
	return instance;
}

void Physics::Entry(std::shared_ptr<Collidable> collider)
{
	// 登録
	bool found = (std::find(m_collidables.begin(), m_collidables.end(), collider) != m_collidables.end());
	if (!found)
	{
		m_collidables.emplace_back(collider);
	}
	// 既に登録されてたらエラー
	else
	{
		//assert(0 && "指定のcollidableは登録済です。");
	}
}

void Physics::Exit(std::shared_ptr<Collidable> collider)
{
	// 登録解除
	bool found = (std::find(m_collidables.begin(), m_collidables.end(), collider) != m_collidables.end());
	if (found)
	{
		m_collidables.remove(collider);
	}
	// 登録されてなかったらエラー
	else
	{
		//assert(0 && "指定のcollidableが登録されていません。");
	}
}

void Physics::Update()
{
	//仮処理
	for (auto& item : m_collidables)
	{
		auto pos = item->m_rigidbody.GetPos();
		auto nextPos = pos + item->m_rigidbody.GetVelo();
		item->m_nextPos = (item->m_rigidbody.GetPos() + item->m_rigidbody.GetVelo());

		item->m_nextPos = nextPos;
	}
	std::vector<OnCollideInfo> onCollideInfo = CheckCollide();
	FixPosition();

	for (auto& item : onCollideInfo)
	{
		ObjectTag tagA = item.owner->GetTag();
		ObjectTag tagB = item.collider->GetTag();

		if (ShouldCallOnCollide(tagA, tagB))
		{
			item.owner->OnCollide(item.collider);
		}
	}

	//位置修正
	FixPosition();
}

void Physics::DebugDraw()
{
	//当たり判定の描画を行う
	for (auto& item : m_collidables)
	{
		// 当たり判定がオフになっているものは無視する
		if (!item->IsActive()) continue;

		if (item->m_colliderData->GetKind() == ColliderData::Kind::Capsule)
		{
			auto capsuleData = std::dynamic_pointer_cast<CapsuleColliderData>(item->m_colliderData);

			DrawCapsule3D(capsuleData->m_startPos.ToDxVECTOR(), item->m_rigidbody.GetPos().ToDxVECTOR(),
				capsuleData->m_radius, 16, 0xff00ff, 0xff00ff, false);
		}
		else if (item->m_colliderData->GetKind() == ColliderData::Kind::Sphere)
		{
			auto sphereData = std::dynamic_pointer_cast<SphereColliderData>(item->m_colliderData);

			DrawSphere3D(item->m_rigidbody.GetPos().ToDxVECTOR(), 
				sphereData->m_radius, 16, 0xff00ff, 0xff00ff, false);
		}
		else if (item->m_colliderData->GetKind() == ColliderData::Kind::Polygon)
		{
			auto polygonData = std::dynamic_pointer_cast<PolygonColliderData>(item->m_colliderData);
			auto & hitDim = polygonData->GetHitDim();
			
			// 衝突しているポリゴンがある場合のみ描画
			if (hitDim.HitNum > 0)
			{
				for (int i = 0; i < hitDim.HitNum; ++i)
				{
					// 衝突しているポリゴンを黄色で描画
					DrawTriangle3D(
					hitDim.Dim[i].Position[0],
					hitDim.Dim[i].Position[1],
					hitDim.Dim[i].Position[2],
					GetColor(255, 255, 0),
					true);
				}
			}
		}
	}
}

void Physics::FixPosition()
{
	for (auto& item : m_collidables)
	{
		// Posを更新するので、velocityもそこに移動するvelocityに修正
		Vec3 toFixedPos = item->m_nextPos - item->m_rigidbody.GetPos();
		Vec3 nextPos = item->m_rigidbody.GetPos() + toFixedPos;

		item->m_rigidbody.SetVelo(toFixedPos);
		// 位置確定
		item->m_rigidbody.SetPos(nextPos);

		//当たり判定がカプセルだったら
		if (item->m_colliderData->GetKind() == ColliderData::Kind::Capsule)
		{
			auto capsuleCol = std::dynamic_pointer_cast<CapsuleColliderData>(item->m_colliderData);
			//伸びるカプセルかどうか取得する
			if (!capsuleCol->m_isStartPos)
			{
				//伸びないカプセルだったら初期位置を一緒に動かす
				capsuleCol->m_startPos = item->m_nextPos;
			}
		}
	}
}

void Physics::FixNextPosition(std::shared_ptr<Collidable> primary, std::shared_ptr<Collidable> secondary) const
{
	// 当たり判定がオフになっているものは無視する
	if (!primary->IsActive() || !secondary->IsActive()) return;

	auto primaryKind   = primary->m_colliderData->GetKind();
	auto secondaryKind = secondary->m_colliderData->GetKind();

	// 球同士の位置補正
	if (primaryKind == ColliderData::Kind::Sphere && secondaryKind == ColliderData::Kind::Sphere)
	{
		Vec3 primaryToSecondary = secondary->m_nextPos - primary->m_nextPos;
		primaryToSecondary.Normalize();

		auto primaryColliderData = dynamic_pointer_cast<SphereColliderData>(primary->m_colliderData);
		auto secondaryColliderData = dynamic_pointer_cast<SphereColliderData>(secondary->m_colliderData);
		float awayDist = primaryColliderData->m_radius + secondaryColliderData->m_radius;
		Vec3 primaryToNewSecondaryPos = primaryToSecondary * awayDist;
		Vec3 fixedPos = primary->m_nextPos + primaryToNewSecondaryPos;
		secondary->m_nextPos = fixedPos;
	}
	// カプセルとカプセルの位置補正
	else if (primaryKind == ColliderData::Kind::Capsule && secondaryKind == ColliderData::Kind::Capsule)
	{
		auto primaryColliderData = dynamic_pointer_cast<CapsuleColliderData>(primary->m_colliderData);
		auto secondaryColliderData = dynamic_pointer_cast<CapsuleColliderData>(secondary->m_colliderData);

		// カプセルの線分の始点と終点
		Vec3 primaryStart   = primary->m_nextPos;
		Vec3 primaryEnd     = primaryColliderData->m_startPos;

		Vec3 secondaryStart = secondary->m_nextPos;
		Vec3 secondaryEnd   = secondaryColliderData->m_startPos;

		// 線分同士の最近接点を求める
		Vec3 closestPointA, closestPointB;
		SegmentClosestPoint(primaryStart, primaryEnd, secondaryStart, secondaryEnd, &closestPointA, &closestPointB);

		// 最近接点間の距離と方向
		Vec3 closestDir = closestPointB - closestPointA;
		float dist = closestDir.Length();

		float awayDist = primaryColliderData->m_radius + secondaryColliderData->m_radius;

		if (dist == 0.0f)
		{
			closestDir = {1.0f, 0.0f, 0.0f};
			dist = 0.000001f;
		}
		else
		{
			closestDir.Normalize();
		}

		if (dist < awayDist)
		{
			Vec3 fixedPos = closestDir * (awayDist - dist);
			fixedPos.y = 0.0f;
			secondary->m_nextPos += fixedPos;
		}
	}
	// カプセルとポリゴンの位置補正
	else if (primaryKind == ColliderData::Kind::Polygon && secondaryKind == ColliderData::Kind::Capsule)
	{
		//コライダーデータ
		auto collDataA = std::dynamic_pointer_cast<CapsuleColliderData>(secondary->m_colliderData);
		auto collDataB = std::dynamic_pointer_cast<PolygonColliderData>(primary->m_colliderData);
		//リジッドボディ
		auto rbA = secondary->m_rigidbody;
		auto rbB = primary->m_rigidbody;

		//当たったポリゴンの情報
		auto& hitDim = collDataB->GetHitDim();
		//お互い動かないオブジェクトなら衝突しない(ポリゴンはスタティックなので片方がスタティックなら)
		if (primary->m_priority == ObjectPriority::Static)
		{
			// 検出したプレイヤーの周囲のポリゴン情報を開放する
			DxLib::MV1CollResultPolyDimTerminate(hitDim);
			return;
		}

		//カプセルの頭座標と足座標
		Vec3 headPos = collDataA->GetNextStartPos(rbA.GetVelo());//移動後
		Vec3 legPos = rbA.GetNextPos();//移動後
		//頭より足のほうが低い位置にあるなら入れ替える
		if (headPos.y < legPos.y)
		{
			Vec3 temp = legPos;
			legPos = headPos;
			headPos = temp;
		}

		//壁と当たっているなら
		if (m_wallNum > 0)
		{
			//補正するベクトルを返す
			Vec3 overlapVec = HitWallCP(headPos, legPos, m_wallNum, *m_wall, collDataA->m_radius);

			//ポリゴンは固定(static)なので球のみ動かす
			rbB.SetVelo(overlapVec);
		}

		// 検出したプレイヤーの周囲のポリゴン情報を開放する
		DxLib::MV1CollResultPolyDimTerminate(hitDim);
	}
}

std::vector<Physics::OnCollideInfo> Physics::CheckCollide() const
{
	//当たっているものを入れる配列
	std::vector<OnCollideInfo> onCollideInfo;
	for (auto it1 = m_collidables.begin(); it1 != m_collidables.end(); ++it1)
	{
		auto it2 = it1;
		++it2;
		for (; it2 != m_collidables.end(); ++it2)
		{
			auto & first = *it1;
			auto & second = *it2;
			
			//当たり判定チェック
			if (IsCollide(first, second))
			{
				auto priority1 = first->GetPriority();
				auto priority2 = second->GetPriority();
				std::shared_ptr<Collidable> primary = first;
				std::shared_ptr<Collidable> secondary = second;
				if (priority1 < priority2)
				{
					primary = second;
					secondary = first;
				}
				if (!SkipFixPos(primary, secondary))
				{
					FixNextPosition(primary, secondary);
				}
				
				onCollideInfo.push_back({ first, second });
				onCollideInfo.push_back({ second, first });
			}
		}
	}
	return onCollideInfo;

	////当たっているものを入れる配列
	//std::vector<OnCollideInfo> onCollideInfo;
	//for (auto& first : m_collidables)
	//{
	//	for (auto& second : m_collidables)
	//	{
	//		//当たり判定チェック
	//		if (IsCollide(first, second))
	//		{
	//			auto priority1 = first->GetPriority();
	//			auto priority2 = second->GetPriority();
	//			std::shared_ptr<Collidable> primary   = first;
	//			std::shared_ptr<Collidable> secondary = second;
	//			if (priority1 < priority2)
	//			{
	//				primary = second;
	//				secondary = first;
	//			}

	//			if (!SkipFixPos(primary, secondary))
	//			{
	//				FixNextPosition(primary, secondary);
	//			}

	//			//一度入れたものを二度入れないようにチェック
	//			bool hasFirstColData = false;
	//			bool hasSecondColData = false;
	//			for (auto& item : onCollideInfo)
	//			{
	//				//すでに入れていたら弾く
	//				if (item.owner == primary)
	//				{
	//					hasFirstColData = true;
	//				}
	//				if (item.owner == secondary)
	//				{
	//					hasSecondColData = true;
	//				}
	//			}
	//			//弾かれなかった場合当たったものリストに入れる
	//			if (!hasFirstColData)
	//			{
	//				onCollideInfo.push_back({ first,second });
	//			}
	//			if (!hasSecondColData)
	//			{
	//				onCollideInfo.push_back({ second,first });
	//			}
	//		}
	//	}
	//}
	//return onCollideInfo;
}

bool Physics::IsCollide(std::shared_ptr<Collidable> first, std::shared_ptr<Collidable> second) const
{
	// 当たり判定がオフになっているものは無視する
	if (!first->IsActive() || !second->IsActive()) return false;

	// 特定のタグ同士では当たり判定を取らずにスキップ
	if (SkipCheckCollide(first, second))
	{
		return false;
	}

	//第一の当たり判定と第二の当たり判定がおなじものでなければ
	if (first != second)
	{
		//当たり判定の種類を取得
		ColliderData::Kind firstKind  = first->m_colliderData->GetKind();
		ColliderData::Kind secondKind = second->m_colliderData->GetKind();

		//球どうしの当たり判定
		if (firstKind == ColliderData::Kind::Sphere && secondKind == ColliderData::Kind::Sphere)
		{
			//当たり判定を球にダウンキャストする
			auto firstCol = std::dynamic_pointer_cast<SphereColliderData>(first->m_colliderData);
			auto secondCol = std::dynamic_pointer_cast<SphereColliderData>(second->m_colliderData);

			//当たり判定の距離を求める
			float distance = (first->m_nextPos - second->m_nextPos).Length();

			//球の大きさを合わせたものよりも距離が短ければぶつかっている
			if (distance < firstCol->m_radius + secondCol->m_radius)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		//カプセル同士の当たり判定
		else if (firstKind == ColliderData::Kind::Capsule && secondKind == ColliderData::Kind::Capsule)
		{
			//当たり判定をカプセルにダウンキャストする
			auto firstCol = std::dynamic_pointer_cast<CapsuleColliderData>(first->m_colliderData);
			auto secondCol = std::dynamic_pointer_cast<CapsuleColliderData>(second->m_colliderData);

			//カプセル同士の最短距離
			float distance = Segment_Segment_MinLength(first->m_nextPos.ToDxVECTOR(), firstCol->m_startPos.ToDxVECTOR(),
				second->m_nextPos.ToDxVECTOR(), secondCol->m_startPos.ToDxVECTOR());

			if (distance < firstCol->m_radius + secondCol->m_radius)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		//球とカプセルの当たり判定
		else if (firstKind == ColliderData::Kind::Sphere  && secondKind == ColliderData::Kind::Capsule ||
				 firstKind == ColliderData::Kind::Capsule && secondKind == ColliderData::Kind::Sphere)
		{
			//球とカプセルのコライダーデータ作成
			std::shared_ptr<ColliderData> sphereDataBase;
			std::shared_ptr<ColliderData> capsuleDataBase;
			float distance;
			//どちらがカプセルなのか球なのか判別してからデータを入れる
			if (firstKind == ColliderData::Kind::Sphere)
			{
				sphereDataBase = first->m_colliderData;
				capsuleDataBase = second->m_colliderData;

				//カプセルの線分のデータを使うためにダウンキャスト
				auto capsuleData = std::dynamic_pointer_cast<CapsuleColliderData>(capsuleDataBase);
				//線分と点の最近距離を求める
				distance = Segment_Point_MinLength(second->m_nextPos.ToDxVECTOR(),
					capsuleData->m_startPos.ToDxVECTOR(), first->m_nextPos.ToDxVECTOR());
			}
			else
			{
				capsuleDataBase = first->m_colliderData;
				sphereDataBase = second->m_colliderData;
				//カプセルの線分のデータを使うためにダウンキャスト
				auto capsuleData = std::dynamic_pointer_cast<CapsuleColliderData>(capsuleDataBase);
				//線分と点の最近距離を求める
				distance = Segment_Point_MinLength(first->m_nextPos.ToDxVECTOR(),
					capsuleData->m_startPos.ToDxVECTOR(), second->m_nextPos.ToDxVECTOR());
			}
			//ダウンキャスト
			auto sphereData = std::dynamic_pointer_cast<SphereColliderData>(sphereDataBase);
			auto capsuleData = std::dynamic_pointer_cast<CapsuleColliderData>(capsuleDataBase);

			//円とカプセルの半径よりも円とカプセルの距離が近ければ当たっている
			if (distance < sphereData->m_radius + capsuleData->m_radius)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		// カプセルとポリゴンの当たり判定
		else if (firstKind == ColliderData::Kind::Polygon && secondKind == ColliderData::Kind::Capsule ||
			firstKind == ColliderData::Kind::Capsule && secondKind == ColliderData::Kind::Polygon)
		{
			std::shared_ptr<Collidable> capsuleCollidable;
			std::shared_ptr<Collidable> polygonCollidable;
			if (firstKind == ColliderData::Kind::Capsule)
			{
				capsuleCollidable = first;
				polygonCollidable = second;
			}
			else
			{
				capsuleCollidable = second;
				polygonCollidable = first;
			}

			//コライダーデータの取得
			auto collDataA = std::dynamic_pointer_cast<CapsuleColliderData>(capsuleCollidable->m_colliderData);
			auto collDataB = std::dynamic_pointer_cast<PolygonColliderData>(polygonCollidable->m_colliderData);
			//リジッドボディ
			auto rbA = first->m_rigidbody;

			//当たってるポリゴンの数
			auto hitDim = MV1CollCheck_Capsule(
				collDataB->GetModelHandle(),
				-1,
				rbA.GetNextPos().ToDxVECTOR(),
				collDataA->GetNextStartPos(rbA.GetVelo()).ToDxVECTOR(),
				collDataA->m_radius,
				-1);

			//当たっていないならfalse
			if (hitDim.HitNum <= 0)
			{
				// 検出したプレイヤーの周囲のポリゴン情報を開放する
				MV1CollResultPolyDimTerminate(hitDim);
				return false;
			}
			//当たり判定に使うので保存
			collDataB->SetHitDim(hitDim);

			return true;
		}
	}
	return false;
}

bool Physics::SkipCheckCollide(std::shared_ptr<Collidable> primary, std::shared_ptr<Collidable> secondary) const
{
	if ((primary->GetTag() == ObjectTag::Player       && secondary->GetTag() == ObjectTag::PlayerWeapon) ||
		(primary->GetTag() == ObjectTag::PlayerWeapon && secondary->GetTag() == ObjectTag::Player)       ||
		(primary->GetTag() == ObjectTag::Enemy        && secondary->GetTag() == ObjectTag::EnemyWeapon)  ||
		(primary->GetTag() == ObjectTag::EnemyWeapon  && secondary->GetTag() == ObjectTag::Enemy)        ||
		(primary->GetTag() == ObjectTag::Enemy        && secondary->GetTag() == ObjectTag::Bullet)       ||
		(primary->GetTag() == ObjectTag::Bullet       && secondary->GetTag() == ObjectTag::Enemy)        ||
		(primary->GetTag() == ObjectTag::Enemy        && secondary->GetTag() == ObjectTag::Enemy))
	{
		return true;
	}
	return false;
}

bool Physics::SkipFixPos(std::shared_ptr<Collidable> primary, std::shared_ptr<Collidable> secondary) const
{
	if (primary->GetTag() == ObjectTag::Player       && secondary->GetTag() == ObjectTag::Bullet       ||
		primary->GetTag() == ObjectTag::Enemy        && secondary->GetTag() == ObjectTag::Bullet       ||
		primary->GetTag() == ObjectTag::Boss         && secondary->GetTag() == ObjectTag::Bullet       ||
		primary->GetTag() == ObjectTag::Enemy        && secondary->GetTag() == ObjectTag::PlayerWeapon ||
		primary->GetTag() == ObjectTag::Boss         && secondary->GetTag() == ObjectTag::PlayerWeapon ||
		primary->GetTag() == ObjectTag::PlayerWeapon && secondary->GetTag() == ObjectTag::Enemy        ||
		primary->GetTag() == ObjectTag::PlayerWeapon && secondary->GetTag() == ObjectTag::Boss         ||
		primary->GetTag() == ObjectTag::Player       && secondary->GetTag() == ObjectTag::EnemyWeapon  ||
		primary->GetTag() == ObjectTag::EnemyWeapon  && secondary->GetTag() == ObjectTag::Player)
	{
		return true;
	}
	return false;
}

bool Physics::ShouldCallOnCollide(ObjectTag tagA, ObjectTag tagB) const
{
	if ((tagA == ObjectTag::Player && tagB == ObjectTag::Enemy)  ||
		(tagA == ObjectTag::Enemy  && tagB == ObjectTag::Player) ||
		(tagA == ObjectTag::Player && tagB == ObjectTag::Boss)   ||
		(tagA == ObjectTag::Boss   && tagB == ObjectTag::Player) ||
		(tagA == ObjectTag::Stage  && tagB == ObjectTag::Player) ||
		(tagA == ObjectTag::Player && tagB == ObjectTag::Stage)  ||
		(tagA == ObjectTag::Stage  && tagB == ObjectTag::Enemy)  ||
		(tagA == ObjectTag::Enemy  && tagB == ObjectTag::Stage))
	{
		// プレイヤーと敵は物理衝突（押し戻し）はするが、OnCollide は呼ばない
		return false;
	}

	return true;
}

void Physics::SegmentClosestPoint(Vec3& segAStart, Vec3& segAEnd,
	Vec3& segBStart, Vec3& segBEnd,
	Vec3* closestPtA, Vec3* closestPtB) const
{
	// 線分Aの方向ベクトル
	Vec3 segADir = segAEnd - segAStart;
	// 線分Bの方向ベクトル
	Vec3 segBDir = segBEnd - segBStart;
	// 線分Aの始点から線分Bの始点
	Vec3 startDiff = segAStart -segBStart;

	// 線分Aの長さ2乗
	float segASqLen = segADir.Dot(segADir);
	// 線分Bの長さ2乗
	float segBSqLen = segBDir.Dot(segBDir);
	// B始点→A始点ベクトルが、線分Bにどれだけ沿っているか
	float segBDotDiff = segBDir.Dot(startDiff);

	float paramA, paramB;

	// 両方の線分が点の場合
	if (segASqLen <= 0.000001f && segBSqLen <= 0.000001f)
	{
		// 双方の始点を最近接点にする
		*closestPtA = segAStart;
		*closestPtB = segBStart;
		return;
	}

	// 線分Aが点の場合
	if (segASqLen <= 0.000001f)
	{
		paramA = 0.0f;
		// 線分B上の最近接点の計算をし、0~1に制限
		paramB = segBDotDiff / segBSqLen;
		paramB = std::clamp(paramB, 0.0f, 1.0f);
	}
	else
	{
		// Aに向かうベクトルとA始点、B始点の内積
		float segADotDiff = segADir.Dot(startDiff);

		// 線分Bが点の場合
		if (segBSqLen <= 0.000001f)
		{
			paramB = 0.0f;
			// 線分A上の最近接点の計算をし、0~1に制限
			paramA = -segADotDiff / segASqLen;
			paramA = std::clamp(paramA, 0.0f, 1.0f);
		}
		else
		{
			// Aに向かうベクトルとBに向かうベクトルの内積
			float segABDot = segADir.Dot(segBDir);
			// 二つの線分がどれだけ平行じゃないか(0に近いほど平行)
			float segmentParallel = segASqLen * segBSqLen - segABDot * segABDot;

			// 平行じゃない場合
			if (segmentParallel != 0.0f)
			{
				paramA = (segABDot * segBDotDiff - segADotDiff * segBSqLen) / segmentParallel;
				paramA = std::clamp(paramA, 0.0f, 1.0f);
			}
			else
			{
				// 平行の場合はA始点を仮の最近接点とする
				paramA = 0.0f;
			}

			// 線分B上の最近接点の計算
			paramB = (segABDot * paramA + segBDotDiff) / segBSqLen;

			// 線分Bの外にいる場合、補正してA側を調整
			if (paramB < 0.0f)
			{
				paramB = 0.0f;
				paramA = std::clamp(-segADotDiff / segASqLen, 0.0f, 1.0f);
			}
			else if (paramB > 1.0f)
			{
				paramB = 1.0f;
				paramA = std::clamp((segABDot - segADotDiff) / segASqLen, 0.0f, 1.0f);
			}
		}
	}

	// 線分上の最近接点座標を代入
	*closestPtA = segAStart + (segADir * paramA);
	*closestPtB = segBStart + (segBDir * paramB);
}

void Physics::AnalyzeWallAndFloor(MV1_COLL_RESULT_POLY_DIM hitDim, const Vec3& nextPos)
{
	//壁ポリゴンと床ポリゴンの数を初期化する
	m_wallNum = 0;

	//検出されたポリゴンの数だけ繰り返す
	for (int i = 0; i < hitDim.HitNum; ++i)
	{
		//XZ平面に垂直かどうかはポリゴンの法線のY成分が0に近いかどうかで判断する
		if (hitDim.Dim[i].Normal.y < 0.1f && hitDim.Dim[i].Normal.y > -0.1f)
		{
			//壁ポリゴンと判断された場合でも、プレイヤーのY座標＋1.0fより高いポリゴンのみ当たり判定を行う
			//段さで突っかかるのを防ぐため
			if (hitDim.Dim[i].Position[0].y > nextPos.y + 1.0f ||
				hitDim.Dim[i].Position[1].y > nextPos.y + 1.0f ||
				hitDim.Dim[i].Position[2].y > nextPos.y + 1.0f)
			{
				//ポリゴンの数が列挙できる限界数に達していなかったらポリゴンを配列に保存する
				if (m_wallNum < kMaxHitPolygon)
				{
					//ポリゴンの構造体のアドレスを壁ポリゴン配列に保存
					m_wall[m_wallNum] = &hitDim.Dim[i];
					++m_wallNum;
				}
			}
		}
	}
}

Vec3 Physics::HitWallCP(const Vec3& headPos, const Vec3& legPos, int hitNum, MV1_COLL_RESULT_POLY* dim, float shortDis) const
{
	float maxOverlap = -1.0f;
	Vec3 pushBackVec = { 0.0f, 0.0f, 0.0f };
	
	// 衝突したすべての壁ポリゴンをチェック
	for (int i = 0; i < hitNum; ++i)
	{
		// ポリゴンの3頂点
		VECTOR p0 = dim[i].Position[0];
		VECTOR p1 = dim[i].Position[1];
		VECTOR p2 = dim[i].Position[2];
		
		// カプセルの線分とポリゴンの最短距離を計算
		float distSq = Segment_Triangle_MinLength_Square(headPos.ToDxVECTOR(), legPos.ToDxVECTOR(), p0, p1, p2);
		float dist = sqrtf(distSq);
		
		// どれだけめり込んでいるか
		float overlap = shortDis - dist;
		
		// 今までで一番めり込んでいるポリゴンなら、押し戻しベクトルを更新
		if (overlap > maxOverlap)
		{
			maxOverlap = overlap;
			Vec3 normal = Vec3(dim[i].Normal.x, dim[i].Normal.y, dim[i].Normal.z);
			normal.y = 0; // Y方向には押し出さない
			normal.Normalize();
			pushBackVec = normal * (overlap + kOverlapGap); 
			// kOverlapGapは少し余分に押し出すための値
		}
	}
	return pushBackVec;



	////垂線を下して近い点を探して最短距離を求める
	//float hitShortDis = shortDis;//最短距離
	////法線
	//Vec3 nom = {};
	//for (int i = 0; i < hitNum; ++i)
	//{
	//	//最短距離の2乗を返す
	//	float dis = Segment_Triangle_MinLength_Square(headPos.ToDxVECTOR(), legPos.ToDxVECTOR(), dim[i].Position[0], dim[i].Position[1], dim[i].Position[2]);
	//	//平方根を返す
	//	dis = sqrtf(dis);
	//	//初回または前回より距離が短いなら
	//	if (hitShortDis > dis)
	//	{
	//		//現状の最短
	//		hitShortDis = dis;
	//		//法線
	//		nom = Vec3{ dim[i].Normal.x,0.0f ,dim[i].Normal.z };
	//	}
	//}
	////押し戻し
	////どれくらい押し戻すか
	//float tempOverLap = shortDis - hitShortDis;

	//if (tempOverLap < 0)
	//{
	//	tempOverLap = 0;
	//}
	//if (tempOverLap > shortDis)
	//{
	//	tempOverLap = shortDis;
	//}
	//float overlap = tempOverLap;
	//overlap += kOverlapGap;
	////正規化
	//if (nom.Length() != 0.0f)
	//{
	//	nom = nom.GetNormalize();
	//}
	//return nom * overlap;
}
