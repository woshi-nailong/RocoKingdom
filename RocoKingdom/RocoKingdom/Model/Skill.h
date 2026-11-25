#pragma once
#include "type.h"
#include <string>

/*
 * 技能类
 * 负责人: 
 * 功能:
 *     存储一个技能的数据，包括名称、属性、类型、威力、消耗和描述
 *     技能的实际使用效果由 Service 层处理
 */
class Skill {
public:
	int id;						//技能序号
	std::string name;           // 技能名称
	PropertyType element;       // 技能属性(火/水/草/无)
	AttributeType attr;			// 技能是物理还是魔法伤害
	SkillType type;             // 技能类型(攻击/状态/防御)
	int power;                  // 技能威力
	int epCost;                 // 能量消耗
	std::string description;    // 技能描述
};
