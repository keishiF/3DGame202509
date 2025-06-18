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

	//�����蔻���ʎ擾
	Kind GetKind()const { return m_kind; }

private:
	//�����蔻��̎��
	Kind m_kind;
};

