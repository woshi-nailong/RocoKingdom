#pragma once
#include"../Model/PetTemplate.h"
#include "Model/Player.h"

/*
 * 初始化游戏数据（宠物和技能都是硬编码的）
 * 负责人：Prospect
 * 功能:
 *     用硬编码数据填充 petDatabase（含每只宠物的技能池）
 */
extern PetTemplate petDatabase[3];
extern Player player1;
extern Player player2;//用extern告诉main我们有全局变量

//初始化宠物模板，在这里填写所有的技能，并塞进各自的模板里
void initPetTemplate();

//初始化玩家，填入宠物
void initPlayerDefault(Player& player);

//初始化总入口
void initGameData();

//初始化战斗开始值
void initBattleStart(Player& player1, Player& player2);


// 声明全局音量控制变量, 所有界面共享
extern int kumaxi_volume;
extern bool kumaxi_isMuted;
extern wchar_t kumaxi_currentAlias[32]; // 记录当前播放中的音频别名
void kumaxi_playSceneBGM(const wchar_t* kumaxi_audioPath);