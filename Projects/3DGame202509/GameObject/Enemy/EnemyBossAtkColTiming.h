#pragma once
#include "EnemyBoss.h"
#include <unordered_map>

namespace BossAtk
{
	struct AtkTiming
	{
		float start;
		float end;
	};

	const std::unordered_map<EnemyState, AtkTiming> kColTimingTable =
	{
		{EnemyState::Walk,	 { 0,  0}},
		{EnemyState::Find,	 { 0,  0}},
		{EnemyState::Chase,	 { 0,  0}},
		{EnemyState::Attack, { 0, 48}},
		{EnemyState::Chop,   {12, 64}},
		{EnemyState::Slice,  {12, 64}},
		{EnemyState::Stab,   {12, 48}},
		{EnemyState::Spin,   { 0, 180}},
		{EnemyState::Hit,	 { 0,  0}},
		{EnemyState::Dead,	 { 0,  0}},
		{EnemyState::Tired,  { 0,  0}}
	};
}
