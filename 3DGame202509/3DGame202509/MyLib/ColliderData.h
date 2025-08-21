#pragma once

class ColliderData abstract
{
public:
	enum class Kind
	{
		Sphere,
		Capsule,
		Polygon
	};

	ColliderData(Kind kind) { m_kind = kind; }
	virtual ~ColliderData() {}

	Kind GetKind()const { return m_kind; }

private:
	Kind m_kind;
};

