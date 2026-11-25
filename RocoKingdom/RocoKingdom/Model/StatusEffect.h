#pragma once
#include "type.h"

/*
 * 状态效果结构体
 * 负责人: 
 * 功能:
 *     记录宠物身上的一个状态效果（类型 + 层数）
 *     状态的实际行为（每回合扣血、衰减、解除等）由 Service 层处理
 *     数据只负责"是什么 + 多少层"，行为交给业务逻辑
 * 参数:
 *     type: 状态类型（STATE_BURN / STATE_POISON 等）
 *     stack: 层数
 *         烧伤：每回合层数减半
 *         中毒：层数持续不变
 */
struct StatusEffect {
	StateType type;
	int stack;
};
