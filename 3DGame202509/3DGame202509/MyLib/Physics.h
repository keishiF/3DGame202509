#pragma once
#include "Collidable.h"
#include <list>
#include <memory>
#include <vector>

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

	void FixNextPosition(std::shared_ptr<Collidable> primary, std::shared_ptr<Collidable> secondary) const;
	void FixPosition();
	std::vector<OnCollideInfo> CheckCollide() const;
	bool IsCollide(std::shared_ptr<Collidable> first, std::shared_ptr<Collidable> second) const;

	// 当たり判定をそもそも取らないようにする
	bool SkipCheckCollide(std::shared_ptr<Collidable> primary, std::shared_ptr<Collidable> secondary) const;
	// 当たり判定自体はとるが押し戻しはしないようにする
	bool SkipFixPos(std::shared_ptr<Collidable> primary, std::shared_ptr<Collidable> secondary) const;
	// 位置補正はするがそれ以外の処理(ダメージを受けるなど)はしないようにする
	bool ShouldCallOnCollide(ObjectTag tagA, ObjectTag tagB) const;

	void SegmentClosestPoint(Vec3& segAStart, Vec3& segAEnd,
		Vec3& segBStart, Vec3& segBEnd, 
		Vec3* closestPtA, Vec3* closestPtB) const;

	Physics(const Physics&) = delete;
	Physics& operator =(const Physics&) = delete;
};

