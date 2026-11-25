#pragma once

// ── 属性/元素类型 ──
enum PropertyType {
	FIRE,    // 火
	WATER,   // 水
	GRASS,   // 草
	NONE     // 无属性
};

// ── 技能类型 ──
enum SkillType {
	ATTACK,   // 攻击类
	STATUS,   // 状态类
	DEFENSE,  // 防御类
};

// ── 宠物状态类型 ──
enum StateType {
	STATE_NONE,
	STATE_BURN,      // 灼烧：每回合层数减半，按层数扣血
};

// ── 操作类型 ──
enum ActionType {
	ACTION_SKILL,       // 使用技能 → targetIndex 填技能索引 0~3
	ACTION_CHANGEPET,   // 更换宠物 → targetIndex 填宠物索引 0~2
	ACTION_GATHER,      // 聚能（回复能量）
	ACTION_ESCAPE       // 逃跑（放弃对战）
};

// ── 属性增益/减益类型 ──
enum AttributeType {
	ATTRIBUTE_NONE = -1, // 无配置
	ATK,        // 物理攻击
	DEF,        // 物理防御
	SP_ATK,     // 魔法攻击
	SP_DEF,     // 魔法防御
	SPEED       // 速度
};

// ── 性格加成属性类型 ──
enum NatureStat {
	NATURE_NONE,	// 无
	NATURE_HP,      // 生命
	NATURE_ATK,     // 物理攻击
	NATURE_DEF,     // 物理防御
	NATURE_SP_ATK,  // 魔法攻击
	NATURE_SP_DEF,  // 魔法防御
	NATURE_SPEED    // 速度
};
