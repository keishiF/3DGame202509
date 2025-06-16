#pragma once

class Collidable abstract
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

	Tag		 GetTag()	   const { return tag; }
	Priority GetPriority() const { return priority; }

protected:
	float colRadius;

private:
	Tag		 tag;
	Priority priority;
};

