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

	//“–‚½‚è”»’èŽí•ÊŽæ“¾
	Kind GetKind()const { return m_kind; }

private:
	//“–‚½‚è”»’è‚ÌŽí—Þ
	Kind m_kind;
};

