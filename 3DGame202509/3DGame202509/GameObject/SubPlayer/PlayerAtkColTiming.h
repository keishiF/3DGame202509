#pragma once
#include "SubPlayer.h"
#include <unordered_map>

struct AtkTiming
{
	float start;
	float end;
};

const std::unordered_map<SubPlayerState, AtkTiming> kColTimingTable =
{
	{SubPlayerState::Idle,		 { 0,  0}},
	{SubPlayerState::Walk,		 { 0,  0}},
	{SubPlayerState::NormalRun,  { 0,  0}},
	{SubPlayerState::Tired,	     { 0,  0}},
	{SubPlayerState::Chop,		 {16, 28}},
	{SubPlayerState::Slice,	     {16, 28}},
	{SubPlayerState::Stab,		 {16, 28}},
	{SubPlayerState::Spin,       {16, 36}},
	{SubPlayerState::Shot,       { 0,  0}},
	{SubPlayerState::Special,    { 0, 36}},
	{SubPlayerState::Dodge,	     { 0,  0}},
	{SubPlayerState::Hit,		 { 0,  0}},
	{SubPlayerState::Dead,		 { 0,  0}}
};