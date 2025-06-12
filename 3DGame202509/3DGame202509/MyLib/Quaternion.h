#pragma once
#include "Vec3.h"
#include <cmath>

class Quaternion
{
public:
	Quaternion()
	{
		w = 1.0f;
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}

	Quaternion(float _w, float _x, float _y, float _z)
	{
		w = _w;
		x = _x;
		y = _y;
		z = _z;
	}

	float w, x, y, z;

public:
	void AngleAxis(float& _angle, Vec3& _axis)
	{
		_axis = _axis.GetNormalize();
		w = cos(_angle / 2.0f);
		x = _axis.x * sin(_angle / 2.0f);
		y = _axis.y * sin(_angle / 2.0f);
		z = _axis.z * sin(_angle / 2.0f);
	}
};

Quaternion operator*(const Quaternion& leftQ, const Quaternion& rightQ)
{
	Quaternion resultQ;

	resultQ.w = leftQ.w * rightQ.w - leftQ.x * rightQ.x - leftQ.y * rightQ.y - leftQ.z * rightQ.z;//実部
	resultQ.x = leftQ.w * rightQ.x + leftQ.x * rightQ.w + leftQ.y * rightQ.z - leftQ.z * rightQ.y;//虚部x
	resultQ.y = leftQ.w * rightQ.y + leftQ.y * rightQ.w + leftQ.z * rightQ.x - leftQ.x * rightQ.z;//虚部y
	resultQ.z = leftQ.w * rightQ.z + leftQ.z * rightQ.w + leftQ.x * rightQ.y - leftQ.y * rightQ.x;//虚部z

	return resultQ;
}

Vec3 operator* (const Quaternion& qRot, const Vec3& rightVec)
{
	Quaternion qPos, qInv;
	Vec3 vPos;

	// 3次元座標をクォータニオンに変換
	qPos.w = 1.0f;
	qPos.x = rightVec.x;
	qPos.y = rightVec.y;
	qPos.z = rightVec.z;

	// 回転クォータニオンのインバースの作成
	qInv.w = qRot.w;
	qInv.x = -qRot.x;
	qInv.y = -qRot.y;
	qInv.z = -qRot.z;

	// 回転後のクォータニオンの作成
	qPos = qRot * qPos * qInv;

	// 3次元座標に戻す
	vPos.x = qPos.x;
	vPos.y = qPos.y;
	vPos.z = qPos.z;

	return vPos;
}

