#pragma once
#include "Vector3.h"
#include <cmath>

class Quaternion
{
public:
	Quaternion();

	Quaternion(float _w, float _x, float _y, float _z);

	float w, x, y, z;

public:
	void AngleAxis(float& _angle, Vector3& _axis);

	static Quaternion Slerp(const Quaternion& a, const Quaternion& b, float t);

	float ToEulerY() const;
};

Quaternion operator*(const Quaternion& leftQ, const Quaternion& rightQ);

Vector3 operator* (const Quaternion& qRot, const Vector3& rightVec);

