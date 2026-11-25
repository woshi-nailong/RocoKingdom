#pragma once
#include "../Model/Player.h"
#include "../Model/Action.h"

/*
 * 首发宠物选择界面
 * 负责人: kumaxi
 * 功能:
 *     1. 展示玩家三只宠物的信息（名称、属性、属性值）
 *     2. 检测键盘 1/2/3 或鼠标点击选择首发宠物
 *     3. 更新当前玩家的 nowPetID
 * 参数:
 *     player: 当前选择首发宠物的玩家（会修改 nowPetID）
 */
void petChoiceView(Player& player);


extern Player player1;
extern Player player2;//用extern告诉main我们有全局变量
void checkAlive();
