#pragma once
#include "Vec3.h"
#include <memory>

class Camera;
class SkyDome
{
public:
	SkyDome();
	~SkyDome();

	void Init(std::shared_ptr<Camera> camera);
	void Update(std::shared_ptr<Camera> camera);
	void Draw();

private:
	Vec3 m_pos;
	int m_model;
};

