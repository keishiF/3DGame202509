#include "Quaternion.h"

Quaternion::Quaternion()
{
	w = 1.0f;
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
}

Quaternion::Quaternion(float _w, float _x, float _y, float _z)
{
	w = _w;
	x = _x;
	y = _y;
	z = _z;
}

void Quaternion::AngleAxis(float& _angle, Vector3& _axis)
{
	_axis = _axis.GetNormalize();
	w = cos(_angle / 2.0f);
	x = _axis.x * sin(_angle / 2.0f);
	y = _axis.y * sin(_angle / 2.0f);
	z = _axis.z * sin(_angle / 2.0f);
}

Quaternion Quaternion::Slerp(const Quaternion& a, const Quaternion& b, float t)
{
	float dot = a.w * b.w + a.x * b.x + a.y * b.y + a.z * b.z;

	Quaternion end = b;

	// クォータニオンの向きが逆なら補間方向を反転
	if (dot < 0.0f)
	{
		dot = -dot;
		end.w = -end.w;
		end.x = -end.x;
		end.y = -end.y;
		end.z = -end.z;
	}

	// 補間角が非常に小さいときは線形補間
	if (dot > 0.9995f)
	{
		return Quaternion(
			a.w + t * (end.w - a.w),
			a.x + t * (end.x - a.x),
			a.y + t * (end.y - a.y),
			a.z + t * (end.z - a.z)
		);
	}

	float theta_0 = acosf(dot);			// 角度
	float theta = theta_0 * t;			// tに応じた角度
	float sin_theta = sinf(theta);
	float sin_theta_0 = sinf(theta_0);

	float s0 = cosf(theta) - dot * sin_theta / sin_theta_0;
	float s1 = sin_theta / sin_theta_0;

	return Quaternion(
		s0 * a.w + s1 * end.w,
		s0 * a.x + s1 * end.x,
		s0 * a.y + s1 * end.y,
		s0 * a.z + s1 * end.z
	);
}

float Quaternion::ToEulerY() const
{
	float siny_cosp = 2.0f * (w * y + z * x);
	float cosy_cosp = 1.0f - 2.0f * (y * y + z * z);
	return std::atan2(siny_cosp, cosy_cosp);
}

Quaternion operator*(const Quaternion& leftQ, const Quaternion& rightQ)
{
	Quaternion resultQ;

	resultQ.w = leftQ.w * rightQ.w - leftQ.x * rightQ.x - leftQ.y * rightQ.y - leftQ.z * rightQ.z;//実部
	resultQ.x = leftQ.w * rightQ.x + leftQ.x * rightQ.w + leftQ.y * rightQ.z - leftQ.z * rightQ.y;//虚部x
	resultQ.y = leftQ.w * rightQ.y + leftQ.y * rightQ.w + leftQ.z * rightQ.x - leftQ.x * rightQ.z;//虚部y
	resultQ.z = leftQ.w * rightQ.z + leftQ.z * rightQ.w + leftQ.x * rightQ.y - leftQ.y * rightQ.x;//虚部z

	return resultQ;
}

Vector3 operator*(const Quaternion& qRot, const Vector3& rightVec)
{
	Quaternion qPos, qInv;
	Vector3 vPos;

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
