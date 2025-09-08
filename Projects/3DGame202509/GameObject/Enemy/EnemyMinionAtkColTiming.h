#pragma once
#include "EnemyMinion.h"
#include <unordered_map>

namespace MinionAtk
{
	struct AtkTiming
	{
		float start;
		float end;
	};

	const std::unordered_map<EnemyState, AtkTiming> kColTimingTable =
	{
		{EnemyState::Find,		 { 0,  0}},
		{EnemyState::Walk,		 { 0,  0}},
		{EnemyState::Chase,		 { 0,  0}},
		{EnemyState::Attack,	 {20, 48}},
		{EnemyState::Chop,		 {16, 28}},
		{EnemyState::Slice,	     {16, 28}},
		{EnemyState::Stab,		 {16, 28}},
		{EnemyState::Spin,       {16, 36}},
		{EnemyState::Hit,		 { 0,  0}},
		{EnemyState::Dead,		 { 0,  0}}
	};
}
