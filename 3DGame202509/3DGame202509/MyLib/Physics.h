#pragma once
#include <list>
#include <vector>
#include <memory>
#include "Collidable.h"

class Physics final
{
public:
	void Entry(Collidable* collider);
	void Exit(Collidable* collider);
	void Update();
	void DebugDraw();

private:
	std::list<Collidable*> m_collidables;

	// OnCollideの衝突通知のためのデータ
	struct OnCollideInfo
	{
		Collidable* owner;
		Collidable* colider;
		void OnCollide() { owner->OnCollide(colider); }
	};

	void FixNextPosition(Collidable* primary, Collidable* secondary) const;
	void FixPosition();
	std::vector<OnCollideInfo> CheckCollide() const;
	bool IsCollide(Collidable* first, Collidable* second) const;

	void SegmentClosestPoint(Vec3& segAStart, Vec3& segAEnd,
		Vec3& segBStart, Vec3& segBEnd, 
		Vec3* closestPtA, Vec3* closestPtB) const;
};

