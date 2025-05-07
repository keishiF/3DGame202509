#pragma once
#include <cmath>

// 3次元ベクトルクラス
class Vec3
{
public:
	float x;
	float y;
	float z;

	Vec3()
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}
	Vec3(float _x, float _y, float _z) :
		x(_x),
		y(_y),
		z(_z)
	{

	}

	Vec3 operator+=(const Vec3& vec)
	{
		x += vec.x;
		y += vec.y;
		z += vec.z;
		return *this;
	}

	Vec3 operator+(const Vec3& vec) const
	{
		Vec3 temp{ x + vec.x, y + vec.y, z + vec.z };
		return temp;
	}

	Vec3 operator-=(const Vec3& vec)
	{
		x -= vec.x;
		y -= vec.y;
		z -= vec.z;
		return *this;
	}

	Vec3 operator-(const Vec3& vec) const
	{
		Vec3 temp{ x - vec.x, y - vec.y, z - vec.z };
		return temp;
	}

	Vec3 operator-() const
	{
		return Vec3(-x, -y, -z);
	}

	Vec3 operator*=(float scale)
	{
		x *= scale;
		y *= scale;
		z *= scale;
		return *this;
	}

	Vec3 operator*(float scale) const
	{
		Vec3 temp{ x * scale, y * scale, z * scale };
		return temp;
	}

	Vec3 operator/=(float scale)
	{
		x /= scale;
		y /= scale;
		z /= scale;
		return *this;
	}

	Vec3 operator/(float scale) const
	{
		Vec3 temp{ x / scale,y / scale, z / scale };
		return temp;
	}

	float Length() const
	{
		return sqrtf(x * x + y * y + z * z);
	}

	// 自身の長さを1にする(正規化)
	void Normalize()
	{
		float len = Length();
		if (len <= 0)
		{
			// ゼロベクトルはそのまま
			return;
		}
		x /= len;
		y /= len;
		z /= len;
	}

	// 自身の長さを1にしたベクトルの取得
	// (この関数を読んでも自身の長さは変化しない)
	Vec3 GetNormalize() const
	{
		float len = Length();
		if (len <= 0.0f)
		{
			return *this;
		}
		return (*this) / len;
	}

	static Vec3 Lerp(Vec3 start, Vec3 end, float t)
	{
		Vec3 temp;
		temp.x = std::lerp(start.x, end.x, t);
		temp.y = std::lerp(start.y, end.y, t);
		temp.z = std::lerp(start.z, end.z, t);
		return temp;
	}
};