#pragma once
#include "type.h"

/*
 * 回合操作结构体
 * 负责人: 
 * 功能:
 *     记录玩家本回合选择的动作
 *     使用技能: type=ACTION_SKILL,  targetIndex=技能索引(0~3)
 *     更换宠物: type=ACTION_CHANGEPET, targetIndex=宠物索引(0~2)
 *     聚能:     type=ACTION_GATHER,    targetIndex=-1
 *     逃跑:     type=ACTION_ESCAPE,    targetIndex=-1
 * 参数:
 *     type: 操作类型
 *     targetIndex: 目标索引（技能位/宠物位）
 */
struct Action {
	ActionType type;
	int targetIndex;
};
