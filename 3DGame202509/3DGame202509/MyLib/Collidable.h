#pragma once

class Collidable
{
public:
	enum class Tag
	{
		Player,
		Boss,
		Enemy
	};

	enum class Priority : int
	{
		Low,
		Middle,
		High,
		Static
	};

	Collidable(Tag tag, Priority priority);
	virtual void OnCollide(const Collidable& collider) abstract;

private:
	Tag		 tag;
	Priority priority;
};

