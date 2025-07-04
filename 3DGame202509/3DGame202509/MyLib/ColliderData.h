#pragma once

class ColliderData abstract
{
public:
	enum class Kind
	{
		Sphere,
		Capsule
	};

	ColliderData(Kind kind) { m_kind = kind; }
	virtual ~ColliderData() {}

	//当たり判定種別取得
	Kind GetKind()const { return m_kind; }

private:
	//当たり判定の種類
	Kind m_kind;
};

