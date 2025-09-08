#pragma once
#include "Vector3.h"
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
	Vector3 m_pos;
	float m_rotY;
	int m_model;
};

