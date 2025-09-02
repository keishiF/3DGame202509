#pragma once
#include "Player.h"
#include <unordered_map>

struct AtkTiming
{
	float start;
	float end;
};

const std::unordered_map<PlayerState, AtkTiming> kColTimingTable =
{
	{PlayerState::Idle,		  { 0,  0}},
	{PlayerState::Walk,		  { 0,  0}},
	{PlayerState::NormalRun,  { 0,  0}},
	{PlayerState::Tired,	  { 0,  0}},
	{PlayerState::Chop,		  {16, 28}},
	{PlayerState::Slice,	  {16, 28}},
	{PlayerState::Stab,		  {16, 28}},
	{PlayerState::Spin,       {16, 36}},
	{PlayerState::Shot,       { 0,  0}},
	{PlayerState::Special,    { 0, 250}},
	{PlayerState::Dodge,	  { 0,  0}},
	{PlayerState::Hit,		  { 0,  0}},
	{PlayerState::Dead,		  { 0,  0}}
};