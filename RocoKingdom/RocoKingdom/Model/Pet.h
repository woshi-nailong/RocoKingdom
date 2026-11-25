#pragma once
#include "PetTemplate.h"
#include "StatusEffect.h"
#include <vector>

/*
 * 宠物实例类
 * 负责人: 
 * 功能:
 *     存储战斗中实际使用的宠物实例
 *     通过指针引用 PetTemplate 获取品种信息（名称、属性、技能池等）
 *     自己的字段存储该宠物的性格配置和实际属性值
 *     性格确定后立即算出修正后的六个属性值存入 Pet，战斗中直接读取
 */
class Pet {
public:
	PetTemplate* data;                      // 指向宠物模板（品种信息）

	// ── 性格配置（用于界面显示和保存/读取） ──
	NatureStat buffStat;                 // 加成属性（NATURE_HP/ATK/DEF/SP_ATK/SP_DEF/SPEED，默认NATURE_HP）
	NatureStat debuffStat;               // 减成属性（同上）

	// ── 实际属性值（基础值 + 性格加成后的结果，性格确定后立即算出） ──
	int maxHp;                              // 最大生命值
	int atk;                                // 物理攻击
	int def;                                // 物理防御
	int spAtk;                              // 魔法攻击
	int spDef;                              // 魔法防御
	int speed;                              // 速度

	// ── 战斗中变化的数值 ──
	int currentHp;                          // 当前生命值（初始 = maxHp）
	int currentEp;                          // 当前能量值（初始 = 10）
	std::vector<StatusEffect> statusList;   // 状态列表（支持多状态叠加）

	// ── 已配置的技能（固定4个技能槽，从 PetTemplate.skillPool 中复制） ──
	Skill* skills[4];                        // 配置时从 PetTemplate.skillPool 中选取复制

	//战斗增益，用attribute type，用下标表示
	//enum AttributeType {
	//	ATTRIBUTE_NONE = -1, // 无配置
	//	ATK,        // 物理攻击
	//	DEF,        // 物理防御
	//	SP_ATK,     // 魔法攻击
	//	SP_DEF,     // 魔法防御
	//	SPEED       // 速度
	//};
	float attrBuffs[5] = {};
};
