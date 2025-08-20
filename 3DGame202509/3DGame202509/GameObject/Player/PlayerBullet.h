#pragma once
#include "Collidable.h"

class PlayerBullet : public Collidable
{
public:
	PlayerBullet();
	~PlayerBullet();

	void Init();
	void Update();
	void Draw();
};

