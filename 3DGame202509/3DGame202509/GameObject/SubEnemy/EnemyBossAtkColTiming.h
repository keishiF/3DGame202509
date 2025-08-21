#pragma once
#include "SubEnemyBoss.h"
#include <unordered_map>

namespace BossAtk
{
	struct AtkTiming
	{
		float start;
		float end;
	};

	const std::unordered_map<SubEnemyState, AtkTiming> kColTimingTable =
	{
		{SubEnemyState::Walk,	 { 0,  0}},
		{SubEnemyState::Find,	 { 0,  0}},
		{SubEnemyState::Chase,	 { 0,  0}},
		{SubEnemyState::Attack, { 0, 48}},
		{SubEnemyState::Chop,   {12, 48}},
		{SubEnemyState::Slice,  {12, 56}},
		{SubEnemyState::Stab,   {12, 48}},
		{SubEnemyState::Spin,   { 0, 180}},
		{SubEnemyState::Hit,	 { 0,  0}},
		{SubEnemyState::Dead,	 { 0,  0}}
	};
}
