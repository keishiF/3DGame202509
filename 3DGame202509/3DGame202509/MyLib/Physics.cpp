#define NOMINMAX
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

Physics::Physics()
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
	//else if (primaryKind == ColliderData::Kind::Polygon && secondaryKind == ColliderData::Kind::Capsule)
	//{
	//	// どちらがPolygonか判定
	//	std::shared_ptr<PolygonColliderData> polyData;
	//	std::shared_ptr<CapsuleColliderData> capsuleData;
	//	Vec3 capsuleStart, capsuleEnd;
	//	std::shared_ptr<Collidable> capsuleObj;

	//	if (primaryKind == ColliderData::Kind::Polygon)
	//	{
	//		polyData = std::dynamic_pointer_cast<PolygonColliderData>(primary->m_colliderData);
	//		capsuleData = std::dynamic_pointer_cast<CapsuleColliderData>(secondary->m_colliderData);
	//		capsuleStart = secondary->m_nextPos;
	//		capsuleEnd = capsuleData->m_startPos;
	//		capsuleObj = secondary;
	//	}
	//	else
	//	{
	//		polyData = std::dynamic_pointer_cast<PolygonColliderData>(secondary->m_colliderData);
	//		capsuleData = std::dynamic_pointer_cast<CapsuleColliderData>(primary->m_colliderData);
	//		capsuleStart = primary->m_nextPos;
	//		capsuleEnd = capsuleData->m_startPos;
	//		capsuleObj = primary;
	//	}

	//	// 三角形ポリゴンのみ対応
	//	if (polyData->m_vertices.size() >= 3)
	//	{
	//		Vec3 p0 = polyData->m_vertices[0];
	//		Vec3 p1 = polyData->m_vertices[1];
	//		Vec3 p2 = polyData->m_vertices[2];

	//		// カプセル線分と三角形の最近接点を求める
	//		Vec3 segClosest, triClosest;
	//		Segment_Triangle_ClosestPoints(capsuleStart, capsuleEnd, p0, p1, p2, segClosest, triClosest);

	//		Vec3 diff = segClosest - triClosest;
	//		float dist = diff.Length();

	//		if (dist < capsuleData->m_radius)
	//		{
	//			// 押し戻しベクトル
	//			Vec3 pushDir = diff;
	//			if (pushDir.Length() < 1e-6f)
	//			{
	//				// 法線方向に押し戻す
	//				pushDir = polyData->m_normal;
	//			}
	//			pushDir.Normalize();
	//			Vec3 fix = pushDir * (capsuleData->m_radius - dist);

	//			// y方向の補正をしない場合はfix.y = 0.0f; なども可
	//			capsuleObj->m_nextPos += fix;
	//		}
	//	}
	//}
}

std::vector<Physics::OnCollideInfo> Physics::CheckCollide() const
{
	//当たっているものを入れる配列
	std::vector<OnCollideInfo> onCollideInfo;
	for (auto& first : m_collidables)
	{
		for (auto& second : m_collidables)
		{
			//当たり判定チェック
			if (IsCollide(first, second))
			{
				auto priority1 = first->GetPriority();
				auto priority2 = second->GetPriority();
				std::shared_ptr<Collidable> primary   = first;
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

				//一度入れたものを二度入れないようにチェック
				bool hasFirstColData = false;
				bool hasSecondColData = false;
				for (auto& item : onCollideInfo)
				{
					//すでに入れていたら弾く
					if (item.owner == primary)
					{
						hasFirstColData = true;
					}
					if (item.owner == secondary)
					{
						hasSecondColData = true;
					}
				}
				//弾かれなかった場合当たったものリストに入れる
				if (!hasFirstColData)
				{
					onCollideInfo.push_back({ first,second });
				}
				if (!hasSecondColData)
				{
					onCollideInfo.push_back({ second,first });
				}
			}
		}
	}
	return onCollideInfo;
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
		//// ポリゴンとカプセルの当たり判定
		//else if (firstKind == ColliderData::Kind::Capsule && secondKind == ColliderData::Kind::Polygon ||
		//		 firstKind == ColliderData::Kind::Polygon && secondKind == ColliderData::Kind::Capsule)
		//{
		//	std::shared_ptr<PolygonColliderData> polyData;
		//	std::shared_ptr<CapsuleColliderData> capsuleData;
		//	Vec3 capsuleStart, capsuleEnd;
		//	if (firstKind == ColliderData::Kind::Polygon)
		//	{
		//		polyData = std::dynamic_pointer_cast<PolygonColliderData>(first->m_colliderData);
		//		capsuleData = std::dynamic_pointer_cast<CapsuleColliderData>(second->m_colliderData);
		//		capsuleStart = second->m_nextPos;
		//		capsuleEnd = capsuleData->m_startPos;
		//	}
		//	else
		//	{
		//		polyData = std::dynamic_pointer_cast<PolygonColliderData>(second->m_colliderData);
		//		capsuleData = std::dynamic_pointer_cast<CapsuleColliderData>(first->m_colliderData);
		//		capsuleStart = first->m_nextPos;
		//		capsuleEnd = capsuleData->m_startPos;
		//	}
		//	
		//	// カプセル線分とポリゴン面の最近接点を求める
		//	float minDist = std::numeric_limits<float>::max();
		//	// 例：三角形ポリゴンの場合
		//	if (polyData->m_vertices.size() >= 3)
		//	{
		//		// ここでは三角形のみ対応
		//		Vec3 p0 = polyData->m_vertices[0];
		//		Vec3 p1 = polyData->m_vertices[1];
		//		Vec3 p2 = polyData->m_vertices[2];
		//		// 線分と三角形の最近接点距離を求める関数を用意する
		//		Segment_Triangle_MinLength(capsuleStart.ToDxVECTOR(), capsuleEnd.ToDxVECTOR(), p0.ToDxVECTOR(), p1.ToDxVECTOR(), p2.ToDxVECTOR());
		//	}
		//	if (minDist < capsuleData->m_radius)
		//	{
		//		return true;
		//	}
		//}
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

//void Physics::Segment_Triangle_ClosestPoints(const Vec3& segA, const Vec3& segB, const Vec3& t0, const Vec3& t1, const Vec3& t2, Vec3& outSeg, Vec3& outTri) const
//{
//	// 線分上の点P(s) = segA + (segB - segA) * s, 0 <= s <= 1
//	// 三角形上の点Q(u,v) = t0 + (t1-t0)*u + (t2-t0)*v, 0 <= u, v, u+v <= 1
//
//	// まず三角形の各辺と線分の最近接点を調べる
//	float minDistSq = std::numeric_limits<float>::max();
//
//	// 三角形の3辺
//	Vec3 triEdges[3][2] = {
//		{t0, t1},
//		{t1, t2},
//		{t2, t0}
//	};
//
//	// 線分と三角形の各辺の最近接点
//	for (int i = 0; i < 3; ++i) {
//		Vec3 edgeA = triEdges[i][0];
//		Vec3 edgeB = triEdges[i][1];
//		Vec3 c1, c2;
//		// 線分同士の最近接点
//		Physics::SegmentClosestPoint(const_cast<Vec3&>(segA), const_cast<Vec3&>(segB),
//			const_cast<Vec3&>(edgeA), const_cast<Vec3&>(edgeB),
//			&c1, &c2);
//		float d2 = (c1 - c2).SqrLength();
//		if (d2 < minDistSq) {
//			minDistSq = d2;
//			outSeg = c1;
//			outTri = c2;
//		}
//	}
//
//	// 線分と三角形面の最近接点（線分の最近接点が三角形内にある場合）
//	// 三角形面の法線
//	Vec3 n = (t1 - t0).Cross(t2 - t0);
//	n.Normalize();
//	Vec3 segDir = segB - segA;
//	float segLen = segDir.Length();
//	if (segLen < 1e-6f) return; // 線分が点
//
//	segDir /= segLen;
//	float denom = n.Dot(segDir);
//	if (std::abs(denom) > 1e-6f) {
//		// 線分と三角形面が平行でない場合
//		float t = n.Dot(t0 - segA) / denom;
//		if (t >= 0.0f && t <= segLen) {
//			Vec3 p = segA + segDir * t;
//			// pが三角形内か判定
//			Vec3 c0 = (t1 - t0).Cross(p - t0);
//			Vec3 c1 = (t2 - t1).Cross(p - t1);
//			Vec3 c2 = (t0 - t2).Cross(p - t2);
//			if (n.Dot(c0) >= 0 && n.Dot(c1) >= 0 && n.Dot(c2) >= 0) {
//				// 三角形内
//				float d2 = (p - (p - n * n.Dot(p - t0))).SqrLength();
//				if (d2 < minDistSq) {
//					minDistSq = d2;
//					outSeg = p;
//					// 三角形面上の最近接点
//					outTri = p - n * n.Dot(p - t0);
//				}
//			}
//		}
//	}
//}
