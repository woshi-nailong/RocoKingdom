#pragma once
#include "../Model/Player.h"

/*
 * 保存玩家配置到文件
 * 负责人: Naz
 * 功能:
 *     将玩家当前宠物队伍配置写入 player_config.txt
 *     配置包括：选中的三只宠物、每只宠物的技能、性格设置
 * 参数:
 *     player: 玩家数据
 * 返回值:
 *     true: 保存成功
 *     false: 保存失败（文件无法写入）
 */
bool savePlayerConfig(const Player& player);

/*
 * 从文件加载玩家配置
 * 负责人: Naz
 * 功能:
 *     读取 player_config.txt，恢复玩家之前保存的队伍配置
 *     如果文件不存在，使用默认配置
 * 参数:
 *     player: 玩家数据（输出参数，函数内部分配宠物并填充数据）
 * 返回值:
 *     true: 加载成功
 *     false: 加载失败（使用默认配置）
 */
bool loadPlayerConfig(Player& player);

/*
 * 计算性格修正后的实际属性值
 * 负责人:
 * 功能:
 *     遍历玩家的三只宠物，根据每只宠物的 buffStat / debuffStat
 *     从 PetTemplate 的基础值计算性格加成后的实际六维属性
 *     加成属性 = 基础值 × 110%
 *     减成属性 = 基础值 × 90%
 *     其他属性保持不变
 * 参数:
 *     player: 玩家数据（会修改其三只宠物的 maxHp, atk, def, spAtk, spDef, speed）
 */
void applyPlayerConfig(Player& player);
