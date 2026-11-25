#pragma once
#include <string>
#include <vector>
#include "Skill.h"

/*
 * 宠物模板类
 * 负责人: 
 * 功能:
 *     存储宠物的固有数据（品种数据）
 *     一个模板对应一个宠物品种
 *     对战中的宠物实例(Pet)会通过指针引用此模板获取固有属性
 */
class PetTemplate {
public:
	//int m_id;							//宠物模板编号
	std::string m_name;                 // 宠物名称
	PropertyType m_element;             // 属性(火/水/草)
	int m_maxHp;                        // 最大生命值
	int m_atk;                          // 物理攻击
	int m_def;                          // 物理防御
	int m_spAtk;                        // 魔法攻击
	int m_spDef;                        // 魔法防御
	int m_speed;                        // 速度(决定出手顺序)
	std::vector<Skill> skillPool;           // 技能池（该品种可学的所有技能，硬编码在初始化中）
};
