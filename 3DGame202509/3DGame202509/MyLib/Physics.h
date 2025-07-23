#pragma once
#include "Collidable.h"
#include <list>
#include <memory>
#include <vector>

namespace
{
	//ポリゴンの当たり判定の配列の最大数
	constexpr int kMaxHitPolygon = 2048;
}

class Physics final
{
public:
	Physics();
	static Physics& Instance();

	void Entry(std::shared_ptr<Collidable> collider);
	void Exit(std::shared_ptr<Collidable> collider);
	void Update();
	void DebugDraw();

private:
	std::list<std::shared_ptr<Collidable>> m_collidables;

	// OnCollideの衝突通知のためのデータ
	struct OnCollideInfo
	{
		std::shared_ptr<Collidable> owner;
		std::shared_ptr<Collidable> collider;
		void OnCollide() { owner->OnCollide(collider); }
	};

	/// <summary>
	/// オブジェクトが衝突しているかを判定する関数
	/// </summary>
	/// <param name="first"></param>
	/// <param name="second"></param>
	/// <returns></returns>
	bool IsCollide(std::shared_ptr<Collidable> first, std::shared_ptr<Collidable> second) const;
	/// <summary>
	/// スフィア同士の当たり判定
	/// </summary>
	/// <param name="first"></param>
	/// <param name="second"></param>
	/// <returns></returns>
	bool IsCollideSS(std::shared_ptr<Collidable>& first, std::shared_ptr<Collidable>& second) const;
	/// <summary>
	/// カプセル同士の当たり判定
	/// </summary>
	/// <param name="first"></param>
	/// <param name="second"></param>
	/// <returns></returns>
	bool IsCollideCC(std::shared_ptr<Collidable>& first, std::shared_ptr<Collidable>& second) const;
	/// <summary>
	/// カプセルとスフィアの当たり判定
	/// </summary>
	/// <param name="firstKind"></param>
	/// <param name="first"></param>
	/// <param name="second"></param>
	/// <returns></returns>
	bool IsCollideCS(ColliderData::Kind firstKind, std::shared_ptr<Collidable>& first, std::shared_ptr<Collidable>& second) const;
	/// <summary>
	/// カプセルとポリゴンの当たり判定
	/// </summary>
	/// <param name="firstKind"></param>
	/// <param name="first"></param>
	/// <param name="second"></param>
	/// <returns></returns>
	bool IsCollideCP(ColliderData::Kind firstKind, std::shared_ptr<Collidable>& first, std::shared_ptr<Collidable>& second) const;

	void FixPosition();
	void FixNextPosition(std::shared_ptr<Collidable> primary, std::shared_ptr<Collidable> secondary) const;
	/// <summary>
	/// スフィア同士の位置補正
	/// </summary>
	/// <param name="secondary"></param>
	/// <param name="primary"></param>
	void FixNextPositionSS(std::shared_ptr<Collidable>& secondary, std::shared_ptr<Collidable>& primary) const;
	/// <summary>
	/// カプセル同士の位置補正
	/// </summary>
	/// <param name="primary"></param>
	/// <param name="secondary"></param>
	void FixNextPositionCC(std::shared_ptr<Collidable>& primary, std::shared_ptr<Collidable>& secondary) const;
	/// <summary>
	/// ポリゴンとカプセルの位置補正
	/// </summary>
	/// <param name="primary"></param>
	/// <param name="secondary"></param>
	void FixNextPositionCP(std::shared_ptr<Collidable>& primary, std::shared_ptr<Collidable>& secondary) const;

	std::vector<OnCollideInfo> CheckCollide() const;

	// 当たり判定をそもそも取らないようにする
	bool SkipCheckCollide(std::shared_ptr<Collidable> primary, std::shared_ptr<Collidable> secondary) const;
	// 当たり判定自体はとるが押し戻しはしないようにする
	bool SkipFixPos(std::shared_ptr<Collidable> primary, std::shared_ptr<Collidable> secondary) const;
	// 位置補正はするがそれ以外の処理(ダメージを受けるなど)はしないようにする
	bool ShouldCallOnCollide(ObjectTag tagA, ObjectTag tagB) const;

	void SegmentClosestPoint(Vec3& segAStart, Vec3& segAEnd,
		Vec3& segBStart, Vec3& segBEnd, 
		Vec3* closestPtA, Vec3* closestPtB) const;

	void AnalyzeWallAndFloor(MV1_COLL_RESULT_POLY_DIM hitDim, const Vec3& nextPos);
	int m_wallNum = 0;
	MV1_COLL_RESULT_POLY* m_wall[kMaxHitPolygon];
	Vec3 HitWallCP(const Vec3& headPos, const Vec3& legPos, int hitNum, MV1_COLL_RESULT_POLY* dim, float shortDis) const;

	Physics(const Physics&) = delete;
	Physics& operator =(const Physics&) = delete;
};

