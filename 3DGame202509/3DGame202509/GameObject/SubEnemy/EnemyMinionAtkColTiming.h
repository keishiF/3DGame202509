#pragma once
#include "SubEnemyMinion.h"
#include <unordered_map>

namespace MinionAtk
{
	struct AtkTiming
	{
		float start;
		float end;
	};

	const std::unordered_map<SubEnemyState, AtkTiming> kColTimingTable =
	{
		{SubEnemyState::Find,		 { 0,  0}},
		{SubEnemyState::Walk,		 { 0,  0}},
		{SubEnemyState::Chase,		 { 0,  0}},
		{SubEnemyState::Attack,	     { 0,  0}},
		{SubEnemyState::Chop,		 {16, 28}},
		{SubEnemyState::Slice,	     {16, 28}},
		{SubEnemyState::Stab,		 {16, 28}},
		{SubEnemyState::Spin,        {16, 36}},
		{SubEnemyState::Hit,		 { 0,  0}},
		{SubEnemyState::Dead,		 { 0,  0}}
	};
}
