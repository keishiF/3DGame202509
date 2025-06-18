#include "Physics.h"
#include "Vec3.h"
#include "Collidable.h"
#include "CapsuleColliderData.h"
#include "SphereColliderData.h"
#include "Enemy/EnemyMage.h"
#include "Enemy/EnemyMinion.h"
#include "DxLib.h"
#include <cassert>

void Physics::Entry(std::shared_ptr<Collidable> col)
{
	// 登録
	bool found = (std::find(m_collidables.begin(), m_collidables.end(), col) != m_collidables.end());
	if (!found)
	{
		m_collidables.emplace_back(col);
	}
	// 既に登録されてたらエラー
	else
	{
		assert(0 && "指定のcollidableは登録済です。");
	}
}

void Physics::Exit(std::shared_ptr<Collidable> col)
{
	// 登録解除
	bool found = (std::find(m_collidables.begin(), m_collidables.end(), col) != m_collidables.end());
	if (found)
	{
		m_collidables.remove(col);
	}
	// 登録されてなかったらエラー
	else
	{
		assert(0 && "指定のcollidableが登録されていません。");
	}
}

void Physics::Update()
{
	//仮処理
	for (auto& item : m_collidables)
	{
		item->m_nextPos = (item->m_rigidbody.GetPos() + item->m_rigidbody.GetVelo());
	}
	//当たっているものを入れる配列
	std::vector<OnCollideInfo> pushData;
	for (auto& first : m_collidables)
	{
		for (auto& second : m_collidables)
		{
			//当たり判定チェック
			if (CheckCollide(first, second))
			{
				//一度入れたものを二度入れないようにチェック
				bool hasFirstColData = false;
				bool hasSecondColData = false;
				for (auto& item : pushData)
				{
					//すでに入れていたら弾く
					if (item.owner == first)
					{
						hasFirstColData = true;
					}
					if (item.owner == second)
					{
						hasSecondColData = true;
					}
				}
				//弾かれなかった場合当たったものリストに入れる
				if (!hasFirstColData)
				{
					pushData.push_back({ first,second });
					//if (first->GetTag() == ObjectTag::kPlayer)
					//{
					//	printfDx("プレイヤーとぶつかった");
					//}
					//if (first->GetTag() == ObjectTag::kEnemy)
					//{
					//	printfDx("エネミーとぶつかった");
					//}
				}
				if (!hasSecondColData)
				{
					pushData.push_back({ second,first });
					//if (first->GetTag() == ObjectTag::kPlayer)
					//{
					//	printfDx("プレイヤーとぶつかった");
					//}
					//if (first->GetTag() == ObjectTag::kEnemy)
					//{
					//	printfDx("エネミーとぶつかった");
					//}
				}
			}
		}
	}
	//当たった当たり判定の当たった時の処理を呼ぶ
	for (auto& hitCol : pushData)
	{
		hitCol.OnCollide();
	}
	//位置修正
	FixPosition();
}

void Physics::DebugDraw()
{
	//当たり判定の描画を行う
	for (auto& item : m_collidables)
	{
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

		Vec3 centerPos(0, 0, 0);

		////TODO:ステージの当たり判定を作成する
		////移動制限を付ける(仮処理)
		//const float stageScale = 500.0f;
		//const float groundHeight = -50.0f;

		//if ((nextPos - centerPos).Length() > stageScale)
		//{
		//	//ぶつかった場所を保存する
		//	OnCollideInfo hitCollides;
		//	//ステージとぶつかったして
		//	std::shared_ptr<EnemyMinion> enemyMinion = std::make_shared<EnemyMinion>(GameObjectTag::Bullet);
		//	//ぶつかった場所を補正前の座標に設定
		//	enemyMinion->m_rigidbody.SetPos(nextPos);
		//	hitCollides.owner = enemyMinion;
		//	hitCollides.colider = item;
		//	//ぶつかった時の処理を呼ぶ
		//	hitCollides.OnCollide();
		//	//座標を補正
		//	nextPos = (nextPos - centerPos).GetNormalize() * stageScale;
		//}
		//if (nextPos.y < groundHeight)
		//{
		//	nextPos.y = groundHeight;
		//}

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

bool Physics::CheckCollide(std::shared_ptr<Collidable> first, std::shared_ptr<Collidable> second)
{
	//第一の当たり判定と第二の当たり判定がおなじものでなければ
	if (first != second)
	{
		//当たり判定の種類を取得
		ColliderData::Kind firstKind = first->m_colliderData->GetKind();
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
		else if (firstKind == ColliderData::Kind::Sphere && secondKind == ColliderData::Kind::Capsule ||
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
		//どれにも当てはまらなかったら
		else
		{
			assert(false);
			return false;
		}
	}
	return false;
}
