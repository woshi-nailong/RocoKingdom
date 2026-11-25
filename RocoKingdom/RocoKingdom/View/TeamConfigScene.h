#pragma once
#include "../Model/Player.h"
#include "../Model/PetTemplate.h"

/*
 * 队伍配置界面
 * ���责人: iostream
 * 功能:
 *     1. 选择要配置的玩家（玩家1 / 玩家2，点击标题栏切换）
 *     2. 为每只宠物从其技能池中选择4个技能
 *     3. 设置性格（选择哪个属性+10%，哪个属性-10%）
 *     4. 保存配置到文件
 *     5. 返回主菜单
 * 参数:
 *     无（player1 / player2 为全局变量）
 */
void teamConfigView();
