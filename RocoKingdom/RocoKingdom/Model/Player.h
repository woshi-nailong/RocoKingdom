#pragma once
#include "Pet.h"

/*
 * 玩家类
 * 负责人: 
 * 功能:
 *     存储一个玩家的数据
 *     包括三只宠物队伍、当前出战宠物
 *     每个 Player 拥有自己的 Pet 对象数组，互不干扰
 *     宠物的品种数据（PetTemplate）通过指针共享，只读无冲突
 */
class Player {
public:
	int playerID;               // 玩家编号(1或2)
	Pet playerPet[3];           // 三只宠物（值对象，每个玩家独立一份）
	int nowPetID;               // 当前出战宠物索引(0/1/2)
	bool defenseCooldown;		//防御冷却，如果放了防御，下回合进入冷却，变为true，其他情况为false
};
